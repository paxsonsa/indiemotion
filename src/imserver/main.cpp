#include <iostream>
#include <memory>
#include <filesystem>

#include <fmt/core.h>
#include <indiemotion/server.hpp>

using namespace indiemotion;

class DefaultCallbacks: public GlobalCallbacks
{
};


int main()
{
	auto cwd = std::filesystem::current_path();
        auto controllers = Engine::default_controllers();
        auto engine = std::make_shared<Engine>(std::move(controllers));
        auto runtime = std::make_shared<EmbeddedRuntime>(std::move(engine));
	auto server = std::make_shared<Server>(runtime);
	auto config = std::make_shared<ServerConfiguration>();
        auto callbacks = std::make_shared<DefaultCallbacks>();

	config->address = "127.0.0.1";
	config->port = 2255;
	config->root_path = cwd.string();
        config->callbacks = callbacks;

//        engine->initialize();
	server->start(config);
	fmt::print("* * * IndieMotion Standalone Server * * *\n");
	fmt::print("* * * {} * * *\n", cwd.string());
	fmt::print("* Server Started on http://{}:{}/ (Press CTRL+C to quit)\n", config->address, config->port);
	server->wait();
	return 0;
}
