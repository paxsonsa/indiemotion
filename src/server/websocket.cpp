#include "websocket.hpp"
#include <fmt/core.h>
#include <iostream>

namespace indiemotion::internal {
    Websocket::Websocket(tcp::socket &&socket,
                         std::shared_ptr<Runtime> runtime)
        : _stream(std::move(socket)), _runtime(std::move(runtime)) {
    }

    Websocket::~Websocket() {
        // Remove this session from the list of active sessions
//         _runtime->leave(_as_connection());
    }

    std::shared_ptr<Connection> Websocket::_as_connection() {
        if (!_conn)
            _conn = Connection::from(shared_from_this());
        return _conn;
    }

    void Websocket::_fail(beast::error_code ec, char const *what) {
        // Don't report these
        if (ec == net::error::operation_aborted ||
            ec == websocket::error::closed) {
            // _runtime->leave(this);
            fmt::print("connection closed.\n");
            return;
        }

        std::cerr << what << ": " << ec.message() << "\n";
    }

    void Websocket::_on_accept(beast::error_code ec) {
        // Handle the error, if any
        if (ec)
            return _fail(ec, "accept");

        // Add this session to the list of active sessions
//         _runtime->join(_as_connection());

        // Read a message
        _stream.async_read(_buffer,
                           beast::bind_front_handler(&Websocket::_on_read,
                                                               shared_from_this()));
    }

    void Websocket::_on_read(beast::error_code ec,
                             std::size_t) {
        // Handle the error, if any
        if (ec)
            return _fail(ec, "read");

        // Send to all connections
        std::string str = beast::buffers_to_string(_buffer.data());
        fmt::print("{}\n", str);
//         _runtime->receive(_as_connection(), std::move(str));

        // Clear the buffer
        _buffer.consume(_buffer.size());

        // Read another message
        _stream.async_read(_buffer,
                           beast::bind_front_handler(&Websocket::_on_read,
                                                     shared_from_this()));
    }

    void Websocket::send(std::shared_ptr<std::string const> const &ss) {
        // Post our work to the strand, this ensures
        // that the members of `this` will not be
        // accessed concurrently.
        net::post(_stream.get_executor(),
                  beast::bind_front_handler(&Websocket::_on_send,
                                            shared_from_this(), ss));
    }

    void Websocket::_on_send(std::shared_ptr<std::string const> const &ss) {
        // Always add to queue
        _queue.push_back(ss);

        // Are we already writing?
        if (_queue.size() > 1)
            return;

        // We are not currently writing, so send this immediately
        _stream.async_write(net::buffer(*_queue.front()),
                            beast::bind_front_handler(&Websocket::_on_write,
                                                      shared_from_this()));
    }

    void Websocket::_on_write(beast::error_code ec,
                              std::size_t) {
        // Handle the error, if any
        if (ec)
            return _fail(ec, "write");

        // Remove the string from the queue
        _queue.erase(_queue.begin());

        // Send the next message if any
        if (!_queue.empty())
            _stream.async_write(net::buffer(*_queue.front()),
                                beast::bind_front_handler(&Websocket::_on_write,
                                                          shared_from_this()));
    }
} // namespace indiemotion::internal
