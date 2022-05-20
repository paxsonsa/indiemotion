//
// Created by Andrew Paxson on 2022-05-20.
//
#include <iostream>
#include <chrono>
#include "http_session.hpp"

namespace indiemotion::internal {

	template<class Body, class Allocator, class Send>
	void handle_request(http::request<Body, http::basic_fields<Allocator>>&& req,
						Send&& send) {
		// Returns a bad request response
		auto const bad_request =
			[&req](beast::string_view why)
			{
				http::response<http::string_body> res{http::status::bad_request, req.version()};
				res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
				res.set(http::field::content_type, "text/html");
				res.keep_alive(req.keep_alive());
				res.body() = std::string(why);
				res.prepare_payload();
				return res;
			};

		send(bad_request("Not Implemented Currently."));
	}


	HttpSession::HttpSession(tcp::socket&& socket): _stream(std::move(socket))
	{}

	void HttpSession::run()
	{
		_do_read();
	}

	void HttpSession::_fail(beast::error_code ec, char const* what)
	{
		// Don't report on canceled operations
		if(ec == net::error::operation_aborted)
			return;

		std::cerr << what << ": " << ec.message() << "\n";
	}

	void HttpSession::_do_read()
	{
		_parser.emplace();
		_parser->body_limit(10000);
		_stream.expires_after(std::chrono::seconds(5));

		http::async_read(
			_stream,
			_buffer,
			_parser->get(),
			beast::bind_front_handler(
				&HttpSession::_on_read,
				shared_from_this()));
	}

	void HttpSession::_on_read(beast::error_code ec, std::size_t)
	{
		if (ec == http::error::end_of_stream)
		{
			_stream.socket().shutdown(tcp::socket::shutdown_send, ec);
			return;
		}

		// Handle the error, if any
		if(ec)
			return _fail(ec, "read");

		// See if it is a WebSocket Upgrade
//		if(websocket::is_upgrade(parser_->get()))
//		{
//			// Create a websocket session, transferring ownership
//			// of both the socket and the HTTP request.
//			boost::make_shared<websocket_session>(
//				stream_.release_socket(),
//				state_)->run(parser_->release());
//			return;
//		}

		handle_request(
//			state_->doc_root(),
			_parser->release(),
			[this](auto&& response)
			{
				// The lifetime of the message has to extend
				// for the duration of the async operation so
				// we use a shared_ptr to manage it.
				using response_type = typename std::decay<decltype(response)>::type;
				auto sp = boost::make_shared<response_type>(std::forward<decltype(response)>(response));


				// Write the response
				auto self = shared_from_this();
				http::async_write(_stream, *sp,
					[self, sp](
						beast::error_code ec, std::size_t bytes)
					{
						self->_on_write(ec, bytes, sp->need_eof());
					});
			});
	}

	void HttpSession::_on_write(beast::error_code ec, std::size_t, bool close)
	{
		// Handle the error, if any
		if(ec)
			return _fail(ec, "write");

		if(close)
		{
			// This means we should close the connection, usually because
			// the response indicated the "Connection: close" semantic.
			_stream.socket().shutdown(tcp::socket::shutdown_send, ec);
			return;
		}

		// Read another request
		_do_read();
	}
}