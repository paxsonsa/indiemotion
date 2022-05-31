#pragma once
#include <memory>
#include <beast.hpp>
#include <net.hpp>

#include "net.hpp"
#include "beast.hpp"

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

namespace indiemotion::internal
{
	class Websocket : public std::enable_shared_from_this<Websocket>
	{
		beast::flat_buffer _buffer;
		websocket::stream<beast::tcp_stream> _stream;
		std::vector<std::shared_ptr<std::string const>> _queue;

		void _fail(beast::error_code ec, char const* what);
		void _on_accept(beast::error_code ec);
		void _on_read(beast::error_code ec, std::size_t bytes_transferred);
		void _on_write(beast::error_code ec, std::size_t bytes_transferred);
		void _on_send(std::shared_ptr<std::string const> const& ss);

	public:
		explicit Websocket(tcp::socket&& socket);

		~Websocket();

		template<class Body, class Allocator>
		void run(http::request<Body, http::basic_fields<Allocator>> req);

		// Send a message
		void send(std::shared_ptr<std::string const> const& ss);
	};

	template<class Body, class Allocator>
	void Websocket::run(http::request<Body, http::basic_fields<Allocator>> req)
	{
		// Set suggested timeout settings for the websocket
		_stream.set_option(
			websocket::stream_base::timeout::suggested(
				beast::role_type::server));

		// Set a decorator to change the Server of the handshake
		_stream.set_option(websocket::stream_base::decorator(
			[](websocket::response_type& res)
			{
				res.set(http::field::server,
					std::string(BOOST_BEAST_VERSION_STRING) +
						" websocket-chat-multi");
			}));

		// Accept the websocket handshake
		_stream.async_accept(
			req,
			beast::bind_front_handler(
				&Websocket::_on_accept,
				shared_from_this()));
	}

}
/*


	class Websocket : public std::enable_shared_from_this<Websocket>
	{
		beast::flat_buffer _buffer;
		websocket::stream<beast::tcp_stream> _stream;
		std::vector<std::shared_ptr<std::string const>> _queue;

		void fail(beast::error_code ec, char const* what);
		void on_accept(beast::error_code ec);
		void on_read(beast::error_code ec, std::size_t bytes_read);
		void on_write(beast::error_code ec, std::size_t bytes_write);
		void on_send(std::shared_ptr<std::string const> const& msg);

	public:

		explicit Websocket(tcp::socket&& socket);

		template<class Body, class Allocator>
		void run(http::request<Body, http::basic_fields<Allocator>> req);

		void send(std::shared_ptr<std::string const> const& msg);


		// TODO: Compile
		// TODO: Add WebSocket Echo Server
		// TODO: When socket accepts connection as to IndieMotionEngine.
	};

	template<class Body, class Allocator>
	void Websocket::run(http::request<Body, http::basic_fields<Allocator>> req)
	{
		_stream.set_option(websocket::stream_base::timeout::suggested(
			beast::role_type::server
		));

		_stream.set_option(websocket::stream_base::decorator(
			[](websocket::response_type& response)
			{
				response.set(http::field::server,
					std::string(BOOST_BEAST_VERSION_STRING) +
						" websocket-chat-multi");
			}
		));

		_stream.async_accept(
			req,
			beast::bind_front_handler(&Websocket::on_accept, shared_from_this())
		);
	}

}
 */