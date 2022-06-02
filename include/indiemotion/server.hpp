//
// Created by Andrew Paxson on 2022-05-17.
//
#pragma once
#include <memory>
#include <indiemotion/server_runtime.hpp>
#include <indiemotion/callbacks.hpp>

namespace indiemotion
{
	struct ServerConfiguration {
		std::string address;
		unsigned short port;
		std::string root_path;
                std::shared_ptr<GlobalCallbacks> callbacks;
	};

	class Server: public std::enable_shared_from_this<Server> {
	public:
            Server();
            ~Server() = default;
            void start(std::shared_ptr<ServerConfiguration> c);
            void wait();

	private:
		std::shared_ptr<ServerRuntime> _runtime;

	};
}