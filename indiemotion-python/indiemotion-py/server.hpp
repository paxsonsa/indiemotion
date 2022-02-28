#pragma once
#include "base.hpp"
#include "delegate.hpp"

struct PyServer
{
	std::thread bg_thread;
	std::shared_ptr<indiemotion::Server> server_ptr;
	indiemotion::logging::Logger _logger;
	std::unique_ptr<GILRelease> gil = nullptr;

	PyServer(python::object delegate_obj) {
		indiemotion::logging::log_trace_scope _("python-server::init");
		_logger = indiemotion::logging::get_logger();

		_logger->trace("creating delegates...");
		auto delegate = std::make_shared<PyDelegateWrapper>(delegate_obj);
		indiemotion::DelegateInfo delegate_info;
		delegate_info.session = delegate;
		delegate_info.scene = delegate;
		delegate_info.motion = delegate;
		delegate_info.server = delegate;

		_logger->info("using server values: bind: 0.0.0.0, port: 5566, disconnect policy: restart-always");
		indiemotion::Options server_options;
		server_options.address = "0.0.0.0";
		server_options.port = 5566;
		server_options.delegate_info = delegate_info;
		server_options.disconnect_behavior = indiemotion::DisconnectBehavior::RestartAlways;

		server_options.on_connect = [&]() {};
		server_options.on_disconnect = [&]() {};

		_logger->trace("creating server instances");
		server_ptr = std::make_shared<indiemotion::Server>(server_options);
	}

	~PyServer() {
		_logger->trace("deleting python-server");
	}

	void start_blocking()
	{
		indiemotion::logging::log_trace_scope _("python-server::start_blocking");
		start();
		wait();
	}

	void start()
	{
		indiemotion::logging::log_trace_scope _("python-server::start");
		gil = std::make_unique<GILRelease>();
		bg_thread = std::thread{[&]() {
			indiemotion::logging::log_trace_scope _("python-server::start::thread");
			server_ptr->start();
		}};
		_logger->trace("server thread joinable: {}", bg_thread.joinable());
	}

	void wait()
	{
		indiemotion::logging::log_trace_scope _("python-server::wait");
		bg_thread.join();
		gil = nullptr;
	}
};