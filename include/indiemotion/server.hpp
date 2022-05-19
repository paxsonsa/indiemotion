//
// Created by Andrew Paxson on 2022-05-17.
//
#pragma once

namespace indiemotion
{
	struct ServerConfiguration {};

	class Server {
	public:
		void start(std::shared_ptr<ServerConfiguration> c);
		void wait();
		void setListeningPort(const std::string& addr);
//		int registerController(std::shared_ptr<Controller> controller);
//      GlobalCallbacks class

	private:


	};
}