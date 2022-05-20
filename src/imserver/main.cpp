#include <iostream>
#include <memory>
#include <indiemotion/server.hpp>

#include <fmt/core.h>

using namespace indiemotion;

int main()
{
	auto server = std::make_unique<Server>();
	auto config = std::make_shared<ServerConfiguration>();
	config->address = "127.0.0.1";
	config->port = 2255;

	server->start(config);
	fmt::print("* Server Started on http://{}:{}/ (Press CTRL+C to quit)\n", config->address, config->port);
	server->wait();
	return 0;
}
