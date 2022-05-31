#include "websocket.hpp"
#include <iostream>
#include <fmt/core.h>

namespace indiemotion::internal
{
	Websocket::
	Websocket(tcp::socket&& socket) : _stream(std::move(socket))
	{
	}

	Websocket::~Websocket()
	{
		// Remove this session from the list of active sessions
	}

	void Websocket::_fail(beast::error_code ec, char const* what)
	{
		// Don't report these
		if (ec == net::error::operation_aborted ||
			ec == websocket::error::closed)
		{
			fmt::print("connection closed.\n");
			return;
		}

		std::cerr << what << ": " << ec.message() << "\n";
	}

	void Websocket::_on_accept(beast::error_code ec)
	{
		// Handle the error, if any
		if (ec)
			return _fail(ec, "accept");

		// Add this session to the list of active sessions
//	state_->join(this);

		// Read a message
		_stream.async_read(
			_buffer,
			beast::bind_front_handler(
				&Websocket::_on_read,
				shared_from_this()));
	}

	void Websocket::_on_read(beast::error_code ec, std::size_t bytes_transferred)
	{
		// Handle the error, if any
		if (ec)
			return _fail(ec, "read");

		// Send to all connections
		// TODO: Process incomming message from socket into some session state manager.
		// TODO: Allow for a 'handler' to be attached to change websocket behavior.
		// TODO: Generate Server Runtime to handle all message processing and pass to SessionManager
		// SessionManager will manage the different websockets and the communicate between them when handling
		// the processing.
		// CODE GOES HERE
		std::string str = beast::buffers_to_string(_buffer.data());
		auto const msg = std::make_shared<std::string const>(std::move(str));
		send(msg);
		fmt::print("{}\n", str);

		// Clear the buffer
		_buffer.consume(_buffer.size());

		// Read another message
		_stream.async_read(
			_buffer,
			beast::bind_front_handler(
				&Websocket::_on_read,
				shared_from_this()));
	}

	void Websocket::send(std::shared_ptr<std::string const> const& ss)
	{
		// Post our work to the strand, this ensures
		// that the members of `this` will not be
		// accessed concurrently.

		net::post(
			_stream.get_executor(),
			beast::bind_front_handler(
				&Websocket::_on_send,
				shared_from_this(),
				ss));
	}

	void Websocket::_on_send(std::shared_ptr<std::string const> const& ss)
	{
		// Always add to queue
		_queue.push_back(ss);

		// Are we already writing?
		if (_queue.size() > 1)
			return;

		// We are not currently writing, so send this immediately
		_stream.async_write(
			net::buffer(*_queue.front()),
			beast::bind_front_handler(
				&Websocket::_on_write,
				shared_from_this()));
	}

	void Websocket::_on_write(beast::error_code ec, std::size_t bytes_transferred)
	{
		// Handle the error, if any
		if (ec)
			return _fail(ec, "write");

		// Remove the string from the queue
		_queue.erase(_queue.begin());

		// Send the next message if any
		if (!_queue.empty())
			_stream.async_write(
				net::buffer(*_queue.front()),
				beast::bind_front_handler(
					&Websocket::_on_write,
					shared_from_this()));
	}
}
