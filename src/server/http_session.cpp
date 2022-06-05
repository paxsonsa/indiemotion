//
// Created by Andrew Paxson on 2022-05-20.
//

#include <chrono>
#include <iostream>

#include <fmt/core.h>

#include "websocket.hpp"
#include "http_session.hpp"

namespace indiemotion::internal {

    std::string path_cat(std::string_view base, beast::string_view path) {
        if (base.empty()) {
            return std::string(path);
        }
        std::string result(base);
        char constexpr path_separator = '/';
        if (result.back() == path_separator) {
            result.resize(result.size() - 1);
        }
        result.append(path.data(), path.size());
        return result;
    }

    template <class Body, class Allocator, class Send>
    void
    handle_request(std::string_view doc_root,
                   http::request<Body, http::basic_fields<Allocator>> &&req,
                   Send &&send) {
        // Returns a bad request response
        auto const bad_request = [&req](beast::string_view why) {
            http::response<http::string_body> res{http::status::bad_request,
                                                  req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = std::string(why);
            res.prepare_payload();
            return res;
        };

        // Returns a not found response
        auto const not_found = [&req](beast::string_view target) {
            http::response<http::string_body> res{http::status::not_found,
                                                  req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() =
                "The resource '" + std::string(target) + "' was not found.";
            res.prepare_payload();
            return res;
        };

        // Returns a server error response
        auto const server_error = [&req](beast::string_view what) {
            http::response<http::string_body> res{
                http::status::internal_server_error, req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "An error occurred: '" + std::string(what) + "'";
            res.prepare_payload();
            return res;
        };

        // Make sure we can handle the method
        if (req.method() != http::verb::get && req.method() != http::verb::head)
            return send(bad_request("Unknown HTTP-method"));

        // Request path must be absolute and not contain "..".
        if (req.target().empty() || req.target()[0] != '/' ||
            req.target().find("..") != beast::string_view::npos)
            return send(bad_request("Illegal request-target"));

        // Build the path to the requested file
        std::string path = path_cat(doc_root, req.target());
        if (req.target().back() == '/')
            path.append("index.html");

        // Attempt to open the file
        beast::error_code ec;
        http::file_body::value_type body;
        body.open(path.c_str(), beast::file_mode::scan, ec);

        // Handle the case where the file doesn't exist
        if (ec == boost::system::errc::no_such_file_or_directory)
            return send(not_found(req.target()));

        // Handle an unknown error
        if (ec)
            return send(server_error(ec.message()));

        // Cache the size since we need it after the move
        auto const size = body.size();

        if (req.method() == http::verb::head) {
            http::response<http::empty_body> res{http::status::ok,
                                                 req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return send(std::move(res));
        }

        // Respond to GET request
        http::response<http::file_body> res{
            std::piecewise_construct, std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, req.version())};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return send(std::move(res));
    }

        HttpSession::HttpSession(tcp::socket&& socket,
                                 std::string root,
                                 std::shared_ptr<Runtime> runtime,
                                 std::shared_ptr<GlobalCallbacks> callbacks)
		: _stream(std::move(socket)),
                  _runtime(std::move(runtime)),
                  _doc_root(std::move(root)),
                  _callbacks(std::move(callbacks))
	{
	}

	void HttpSession::run()
	{
            _do_read();
	}

	void HttpSession::_fail(beast::error_code ec, char const* what)
	{
        // Don't report on canceled operations
        if (ec == net::error::operation_aborted)
            return;

        std::cerr << what << ": " << ec.message() << "\n";
	}

	void HttpSession::_do_read()
	{
        fmt::print("_do_read()\n");
        _parser.emplace();
        _parser->body_limit(10000);
        _stream.expires_after(std::chrono::seconds(5));

        http::async_read(_stream, _buffer, _parser->get(),
                         beast::bind_front_handler(&HttpSession::_on_read,
                                                   shared_from_this()));
	}

	void HttpSession::_on_read(beast::error_code ec, std::size_t)
	{
        if (ec == http::error::end_of_stream) {
            _stream.socket().shutdown(tcp::socket::shutdown_send, ec);
            return;
        }

        // Handle the error, if any
        if (ec)
            return _fail(ec, "read");

        // See if it is a WebSocket Upgrade
        if (websocket::is_upgrade(_parser->get())) {
            // Create a websocket session, transferring ownership
            // of both the socket and the HTTP request.
            std::make_shared<Websocket>(std::move(_stream.release_socket()),
                                        _runtime)->run(_parser->release());
            return;
        }

        handle_request(_doc_root, _parser->release(), [this](auto &&response) {
            // The lifetime of the message has to extend
            // for the duration of the async operation so
            // we use a shared_ptr to manage it.
            using response_type = typename std::decay<decltype(response)>::type;
            auto sp = std::make_shared<response_type>(
                std::forward<decltype(response)>(response));

            // Write the response
            auto self = shared_from_this();
            http::async_write(
                _stream, *sp,
                [self, sp](beast::error_code ec, std::size_t bytes) {
                    self->_on_write(ec, bytes, sp->need_eof());
                });
        });
	}

	void HttpSession::_on_write(beast::error_code ec, std::size_t, bool close)
	{
        // Handle the error, if any
        if (ec)
            return _fail(ec, "write");

        if (close) {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            _stream.socket().shutdown(tcp::socket::shutdown_send, ec);
            return;
        }

        // Read another request
        _do_read();
	}
} // namespace indiemotion::internal