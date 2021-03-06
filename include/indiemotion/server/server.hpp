#pragma once
#include <indiemotion/common.hpp>
#include <indiemotion/logging.hpp>
#include <indiemotion/context.hpp>
#include <indiemotion/server/connection.hpp>
#include "indiemotion/options.hpp"

namespace indiemotion
{

	/**
	 * A server for accepting and creating session connections
	 */
	class Server : public std::enable_shared_from_this<Server>
	{
		asio::io_context _io_context;
		Options _options;
		tcp::acceptor _acceptor;
		logging::Logger _logger;
		asio::deadline_timer _shutdown_timer;
		bool _stopped = false;

	public:
		/**
		 * Construct a server with the given server options.
		 * @param options
		 */
		Server(Options options)
			: _options(std::move(options)), _acceptor(_io_context), _shutdown_timer(_io_context)
		{
			_logger = logging::get_logger();
		};

		/**
		 * Start the server, blocks until finished.
		 *
		 * It is good practice call this in a new thread as this blocks
		 * The callback is called with the new session controller, this should be used
		 * to configure your runtime delegate for the session.
		 *
		 * @param on_start_callback A callback to invoke when a new session connection is activated
		 */
		void start()
		{
			_stopped = false;
			auto work = asio::require(_io_context.get_executor(),
				asio::execution::outstanding_work.tracked);

			init_listen();
			init_poll();

			switch (_options.disconnect_behavior)
			{
			case DisconnectBehavior::RestartAlways:
				while (!_stopped)
				{
					if (_io_context.stopped()) {
						_io_context.restart();
					}
					listen();
					_io_context.run();
				}
				break;

			case DisconnectBehavior::Terminate:
				_logger->trace("start listening single-shot");
				listen();
				_io_context.run();
				break;
			}

			_logger->info("server shutting down.");
			_options.delegate_info.server->on_server_shutdown();
			_logger->info("server shutdown.");
		}

		/**
		 * Returns whether the server is stopped.
		 * @return
		 */
		bool stopped()
		{
			return _stopped;
		}

		/**
		 * Stop the current server loop. Multiple calls do nothing.
		 */
		void stop()
		{
			if (!stopped()) {
				_stopped = true;
				_io_context.stop();
			}
		}

	private:
		void init_listen()
		{
			_logger->trace("Server::init_listen()");
			beast::error_code ec;
			auto const address = asio::ip::make_address(_options.address);
			auto const port = _options.port;
			auto const endpoint = tcp::endpoint{ address, port };

			// Open the acceptor
			_acceptor.open(endpoint.protocol(), ec);
			if (ec) {
				auto msg = fmt::format("failed to open listener's acceptor: {}", ec.message());
				_logger->error(msg);
				throw std::runtime_error(msg);
			}

			// Allow address reuse
			_acceptor.set_option(asio::socket_base::reuse_address(true), ec);
			if (ec) {
				auto msg = fmt::format("failed to configure address reuse: {}", ec.message());
				_logger->error(msg);
				throw std::runtime_error(msg);
			}

			// Bind to the server address
			_acceptor.bind(endpoint, ec);
			if (ec) {
				auto msg = fmt::format("failed to bind endpoint: {}", ec.message());
				_logger->error(msg);
				throw std::runtime_error(msg);
			}

			// Start listening for connections
			_acceptor.listen(
				asio::socket_base::max_listen_connections, ec);
			if (ec) {
				auto msg = fmt::format("failed to start listening: {}", ec.message());
				_logger->error(msg);
				throw std::runtime_error(msg);
			}
		}

		void listen() {
			_logger->info("Server starting");
			_options.delegate_info.server->on_server_start();
			_acceptor.async_accept(
				asio::make_strand(_io_context),
				beast::bind_front_handler(
					&Server::on_accept,
					shared_from_this()
				)
			);
		}

		void on_accept(beast::error_code ec, tcp::socket socket) {
			_logger->trace("on_accept()");
			if (ec) {
				_logger->error("encountered error accepting connection, continuing to listen: {}", ec.message());
				listen();
			} else {
				std::make_shared<Connection>(_io_context, std::move(socket), _options)->start();
			}
			_logger->trace("exit on_accept()");
		}

		void check_poll(const boost::system::error_code& error)
		{
#if defined INDIEMOTION_NOISY_TRACE
			indiemotion::logging::log_trace_scope _("indiemotion::Server::check_poll");
#endif
			if (error || stopped())
				return;
			if (_options.delegate_info.server->should_server_shutdown())
			{
				stop();
				return;
			}
			_shutdown_timer.expires_from_now(boost::posix_time::milliseconds(100));
			_shutdown_timer.async_wait(std::bind(&Server::check_poll, this, std::placeholders::_1));
		}

		void init_poll()
		{
			auto ec = boost::system::error_code();
			check_poll(ec);
		}

	};

}
