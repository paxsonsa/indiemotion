#include <fmt/core.h>
#include <indiemotion/impl/server_runtime.hpp>

#include "http_session.hpp"

namespace indiemotion::internal {
	void _ServerRuntime::set_endpoint(const tcp::endpoint& endpoint)
	{
		beast::error_code ec;
		_acceptor.open(endpoint.protocol(), ec);
		if (ec) {
			auto msg = fmt::format("failed to open acceptor: {}", ec.message());
			fmt::print(msg);
			throw std::runtime_error(msg);
		}

		_acceptor.bind(endpoint, ec);
		if (ec) {
			auto msg = fmt::format("failed to bind endpoint: {}", ec.message());
			fmt::print(msg);
			throw std::runtime_error(msg);
		}
	}

	void _ServerRuntime::set_reuse_addr(bool b)
	{
		beast::error_code ec;
		_acceptor.set_option(net::socket_base::reuse_address(b), ec);
		if (ec) {
			auto msg = fmt::format("failed to configure address reuse: {}", ec.message());
			fmt::print(msg);
			throw std::runtime_error(msg);
		}
	}

	void _ServerRuntime::start()
	{
		// Build Runtime Threads

		// Start listening for connections
		beast::error_code ec;
		_acceptor.listen(
			net::socket_base::max_listen_connections, ec);
		if (ec)
		{
			auto msg = fmt::format("failed to start listening: {}", ec.message());
			fmt::print(msg);
			throw std::runtime_error(msg);
		}

		do_accept();
	}

	void _ServerRuntime::wait_until_exit()
	{
		_io_ctx.run();
	}


	void _ServerRuntime::do_accept()
	{
		_acceptor.async_accept(
			net::make_strand(_io_ctx),
			beast::bind_front_handler(
				&_ServerRuntime::on_accept,
				shared_from_this()
			)
		);
	}

	void _ServerRuntime::on_accept(beast::error_code ec, tcp::socket socket) {
		if (ec) {
			fmt::print("failed while accepting connection...");
		} else {
			fmt::print("Accepting a new connection!");
			std::make_shared<HttpSession>(std::move(socket))->run();
		}
		do_accept();
	}


}

