#include <beast.hpp>
#include <memory>
#include <net.hpp>

#include <fmt/core.h>

#include "http_session.hpp"
#include <indiemotion/server.hpp>

namespace indiemotion {

    void Server::start(std::shared_ptr<ServerConfiguration> c) {
        fmt::print("start()\n");
        auto const address = net::ip::make_address(c->address);
        auto const port = c->port;
        auto const endpoint = tcp::endpoint{address, port};
        set_endpoint(endpoint);
        set_reuse_addr(true);
        set_root_path(c->root_path);
        set_callbacks(c->callbacks);

        beast::error_code ec;
        _acceptor.listen(net::socket_base::max_listen_connections, ec);
        if (ec) {
            auto msg =
                fmt::format("failed to start listening: {}", ec.message());
            fmt::print(msg);
            throw std::runtime_error(msg);
        }

        do_accept();
    }

    void Server::wait() { _io_ctx.run(); }

    void Server::set_endpoint(const tcp::endpoint &endpoint) {
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

    void Server::set_reuse_addr(bool b) {
        beast::error_code ec;
        _acceptor.set_option(net::socket_base::reuse_address(b), ec);
        if (ec) {
            auto msg = fmt::format("failed to configure address reuse: {}",
                                   ec.message());
            fmt::print(msg);
            throw std::runtime_error(msg);
        }
    }

    void Server::set_root_path(std::string path) { _root = std::move(path); }

    void Server::set_callbacks(std::shared_ptr<GlobalCallbacks> callbacks) {
        _callbacks = std::move(callbacks);
    }

    void Server::do_accept() {
        _acceptor.async_accept(
            net::make_strand(_io_ctx),
            beast::bind_front_handler(&Server::on_accept, shared_from_this()));
    }

    void Server::on_accept(beast::error_code ec, tcp::socket socket) {
        if (ec) {
            fmt::print("failed while accepting connection...\n");
        } else {
            std::make_shared<internal::HttpSession>(std::move(socket),
                                                    _root,
                                                    _runtime,
                                                    _callbacks)->run();
        }
        do_accept();
    }

} // namespace indiemotion
