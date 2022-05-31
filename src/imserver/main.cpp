#include <iostream>
#include <memory>
#include <filesystem>
#include <indiemotion/server.hpp>

#include <fmt/core.h>

using namespace indiemotion;

int main()
{
	auto cwd = std::filesystem::current_path();
	auto server = std::make_unique<Server>();
	auto config = std::make_shared<ServerConfiguration>();
	config->address = "127.0.0.1";
	config->port = 2255;
	config->root_path = cwd.string();

	server->start(config);
	fmt::print("* * * IndieMotion Standalone Server * * *\n");
	fmt::print("* * * {} * * *\n", cwd.string());
	fmt::print("* Server Started on http://{}:{}/ (Press CTRL+C to quit)\n", config->address, config->port);
	server->wait();
	return 0;
}
