#include <memory>
#include <net.hpp>
#include <indiemotion/server.hpp>
#include <indiemotion/server_runtime.hpp>

namespace indiemotion
{

	Server::Server() {
		_runtime = std::make_shared<ServerRuntime>();
	}

	void Server::start(std::shared_ptr<ServerConfiguration> c)
	{
		auto const address = net::ip::make_address(c->address);
		auto const port = c->port;
		auto const endpoint = tcp::endpoint{ address, port};
		_runtime->set_endpoint(endpoint);
		_runtime->set_reuse_addr(true);
		_runtime->set_root_path(c->root_path);
                _runtime->set_callbacks(c->callbacks);
		_runtime->start();
	}

	void Server::wait()
	{
		_runtime->wait_until_exit();
	}

}
