#pragma once
#include <indiemotion-python/base.hpp>

/**
 * A wrapper for working with Python objects as the delegate items for the session.
 */
struct PyDelegateWrapper : public indiemotion::ServerDelegate,
						   indiemotion::SceneDelegate,
						   indiemotion::SessionDelegate,
						   indiemotion::MotionDelegate
{
	python::object _py_delegate;
	indiemotion::logging::Logger _logger;
	std::unique_ptr<indiemotion::DebugSampleCounter> _sampler;

	/**
	 * Construct a delegate wrapper using one object for all the delegate types.
	 * @param delegate
	 */
	PyDelegateWrapper(python::object delegate) :
		_py_delegate(delegate)
	{
		_logger = indiemotion::logging::get_logger();
		_sampler = std::make_unique<indiemotion::DebugSampleCounter>();
		check_delegate_methods();
	}

	void check_and_warn(std::string name, std::string message)
	{
		if (!python::hasattr(_py_delegate, name.c_str()))
		{
			_logger->warn("no attribute defined for {}, {}", name, message);
		}
	}

	void check_delegate_methods()
	{
		check_and_warn("should_server_shutdown", "will not be able to gracefully shutdown server");
		check_and_warn("should_connection_close", "will not be able to close connection");
		check_and_warn("on_session_updated", "will not be able to response to session changes (e.g. startup)");
		check_and_warn("should_session_shutdown", "will not be able to respond to session shutdowns gracefully");
		check_and_warn("on_motion_updated", "will not be able to read updates to motion data");
		check_and_warn("on_scene_updated", "will not able able to read scene updates (e.g. camera selections)");
	}

	bool should_server_shutdown() override
	{
#if defined INDIEMOTION_NOISY_TRACE
		indiemotion::logging::log_trace_scope _("python-delegate::should_server_shutdown");
#endif
		GILLock gil;
		std::function<bool()> default_m = [](){ return false; };
		return get_method(_py_delegate, "should_server_shutdown", default_m)();
	}

	void on_server_start() override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::on_server_start");
		GILLock gil;
		std::function<void()> default_m = []()
		{};
		return get_method(_py_delegate, "on_server_start", default_m)();
	}

	void on_server_shutdown() override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::on_server_shutdown");
		GILLock gil;
		std::function<void()> default_m = [](){};
		return get_method(_py_delegate, "on_server_shutdown", default_m)();
	}

	void on_connection_start() override {
		indiemotion::logging::log_trace_scope _("python-delegate::on_connection_start");
		GILLock gil;
		std::function<void()> default_m = [](){};
		return get_method(_py_delegate, "on_connection_start", default_m)();
	}

	bool should_connection_close() override
	{
#if defined INDIEMOTION_NOISY_TRACE
		indiemotion::logging::log_trace_scope _("python-delegate::should_connection_close");
#endif
		GILLock gil;
		std::function<bool()> default_m = [](){ return false; };
		return get_method(_py_delegate, "should_connection_close", default_m)();
	}

	void on_connection_close() override {
		indiemotion::logging::log_trace_scope _("python-delegate::on_connection_close");
		GILLock gil;
		std::function<void()> default_m = [](){};
		return get_method(_py_delegate, "on_connection_close", default_m)();
	}

	void on_session_startup(indiemotion::Context ctx) override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::on_session_startup");
		GILLock gil;
		std::function<void(indiemotion::Context)> default_m = [](auto ctx)
		{};
		return get_method(_py_delegate, "on_session_startup", default_m)(ctx);
	}

	void on_session_updated(indiemotion::Context ctx) override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::on_session_updated");
		GILLock gil;
		std::function<void(indiemotion::Context)> default_m = [](auto ctx)
		{};
		return get_method(_py_delegate, "on_session_updated", default_m)(ctx);
	}

	void on_session_shutdown(indiemotion::Context ctx) override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::on_session_shutdown");
		GILLock gil;
		std::function<void(indiemotion::Context)> default_m = [](auto ctx)
		{};
		return get_method(_py_delegate, "on_session_shutdown", default_m)(ctx);
	}

	bool should_session_shutdown(indiemotion::Context ctx) override
	{
#if defined INDIEMOTION_NOISY_TRACE
		indiemotion::logging::log_trace_scope _("python-delegate::should_session_shutdown");
#endif
		GILLock gil;
		std::function<bool()> default_m = []()
		{ return false; };
		return get_method(_py_delegate, "should_session_shutdown", default_m)();
	}

	void on_motion_updated(indiemotion::Context ctx) override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::on_motion_updated");
#if defined INDIEMOTION_NOISY_TRACE
		_logger->trace("motion samples/second: {}", _sampler->poll());
#endif
		GILLock gil;
		std::function<void(indiemotion::Context)> default_m = [](auto ctx)
		{};
		return get_method(_py_delegate, "on_motion_updated", default_m)(ctx);
	}

	std::vector<indiemotion::SceneCamera> get_scene_cameras(indiemotion::Context ctx) override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::get_scene_cameras");
		GILLock gil;
		if (!python::hasattr(_py_delegate, "get_scene_cameras"))
		{
			PyErr_Format(PyExc_AttributeError, "missing expected method 'get_scene_cameras'");
			throw indiemotion::ApplicationException("application scene delegate is misconfigured.", true);
		}

		auto py_func = _py_delegate.attr("get_scene_cameras");
		python::object result = py_func(ctx);

		// Extract the python list from the function call and construct a new std::vector
		// from the list.
		python::list items = python::extract<python::list>(result);
		python::stl_input_iterator<indiemotion::SceneCamera> begin(items), end;
		return std::vector<indiemotion::SceneCamera>(begin, end);
	}

	void on_scene_updated(indiemotion::Context ctx) override
	{
#if defined INDIEMOTION_NOISY_TRACE
		indiemotion::logging::log_trace_scope _("python-delegate::on_scene_updated");
#endif
		GILLock gil;
		std::function<void(indiemotion::Context)> default_m = [](auto ctx)
		{};
		return get_method(_py_delegate, "on_scene_updated", default_m)(ctx);
	}
};
