#include <iostream>
#include <memory>
#include <filesystem>

#include <fmt/core.h>

#include <indiemotion/server.hpp>
#include "callbacks.hpp"

using namespace indiemotion;

int main()
{
	auto cwd = std::filesystem::current_path();
        auto runtime = std::shared_ptr<EchoRuntime>();
	auto server = std::make_shared<Server>(runtime);
	auto config = std::make_shared<ServerConfiguration>();
        auto callbacks = std::make_shared<DefaultCallbacks>();

	config->address = "127.0.0.1";
	config->port = 2255;
	config->root_path = cwd.string();
        config->callbacks = callbacks;
	server->start(config);
	fmt::print("* * * IndieMotion Standalone Server * * *\n");
	fmt::print("* * * {} * * *\n", cwd.string());
	fmt::print("* Server Started on http://{}:{}/ (Press CTRL+C to quit)\n", config->address, config->port);
	server->wait();
	return 0;
}
