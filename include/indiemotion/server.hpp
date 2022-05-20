//
// Created by Andrew Paxson on 2022-05-17.
//
#pragma once
#include <memory>
#include <indiemotion/impl/server_runtime.hpp>

namespace indiemotion
{
	struct ServerConfiguration {
		std::string address;
		unsigned short port;
	};

	class Server: public std::enable_shared_from_this<Server> {
	public:
		Server();
		~Server() = default;
		void start(std::shared_ptr<ServerConfiguration> c);
		void wait();
//		int registerController(std::shared_ptr<Controller> controller);
//      GlobalCallbacks class

	private:
		std::shared_ptr<internal::_ServerRuntime> _runtime;
	};
}