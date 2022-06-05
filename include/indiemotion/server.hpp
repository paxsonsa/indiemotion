//
// Created by Andrew Paxson on 2022-05-17.
//
#pragma once
#include <memory>

#include <beast.hpp>
#include <net.hpp>

#include <indiemotion/callbacks.hpp>
#include <indiemotion/runtime.hpp>

namespace indiemotion
{
	struct ServerConfiguration {
		std::string address;
		unsigned short port;
		std::string root_path;
                std::shared_ptr<GlobalCallbacks> callbacks;
	};

	class Server: public std::enable_shared_from_this<Server> {

            net::io_context _io_ctx;
            tcp::acceptor _acceptor;
            std::string _root;
            std::shared_ptr<GlobalCallbacks> _callbacks;
            std::shared_ptr<Runtime> _runtime;

	public:
            explicit Server(std::shared_ptr<Runtime> rt): _acceptor(_io_ctx), _runtime(std::move(rt)) {}
            ~Server() = default;

            void start(std::shared_ptr<ServerConfiguration>);
            void wait();

            Server& operator=(const Server&) =delete;

	private:
                void do_accept();
                void on_accept(beast::error_code ec, tcp::socket socket);
                void set_endpoint(const tcp::endpoint&);
                void set_reuse_addr(bool);
                void set_root_path(std::string);
                void set_callbacks(std::shared_ptr<GlobalCallbacks>);
	};
}