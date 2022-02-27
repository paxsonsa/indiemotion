#pragma once
#include "base.hpp"
#include "delegate.hpp"

struct PyServer
{
	std::optional<std::thread> bg_thread;
	std::unique_ptr<indiemotion::Server> server_ptr;

	PyServer(python::object delegate_obj) {
		auto delegate = std::make_shared<PyDelegateWrapper>(delegate_obj);
		indiemotion::DelegateInfo delegate_info;
		delegate_info.session = delegate;
		delegate_info.scene = delegate;
		delegate_info.motion = delegate;

		// TODO - Parameterize Server Options
		indiemotion::Options server_options;
		server_options.address = "0.0.0.0";
		server_options.port = 5566;
		server_options.delegate_info = delegate_info;
		server_options.disconnect_behavior = indiemotion::DisconnectBehavior::RestartAlways;

		server_options.on_connect = [&]() {};
		server_options.on_disconnect = [&]() {};

		server_ptr = std::make_unique<indiemotion::Server>(server_options);
	}

	void start_blocking()
	{
		start();
		wait();
	}

	void start()
	{
		bg_thread = std::thread{[&]() {
			server_ptr->start();
		}};
	}

	void wait()
	{
		if (bg_thread.has_value())
		{
			bg_thread.value().join();
		}
	}
};