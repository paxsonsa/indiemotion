#include <iostream>
#include <memory>
#include <indiemotion/server.hpp>

using namespace indiemotion;

int main()
{

	auto server = std::make_unique<Server>();

	auto configuration = std::make_shared<ServerConfiguration>();
	server->start(std::move(configuration));



	return 0;
}
