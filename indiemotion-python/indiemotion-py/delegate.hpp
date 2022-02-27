#pragma once
#include "base.hpp"

/**
 * Returns method on the python object or the default function given
 * @tparam T The return type for the function (can be implied)
 * @param name The name of the method (attribute) to look up on the python object
 * @param obj The python object to check
 * @param super The default function to return.
 * @return A function with the expected return value.
 */
template <typename T>
std::function<T(indiemotion::Context)> get_func_or_super(const char * name, python::object const &obj, std::function<T(indiemotion::Context)> super)
{
	if (python::hasattr(obj, name))
	{
		auto func = obj.attr(name);
		return func;
	}
	return super;
}

template <typename T>
std::function<T(void)> get_func_or_super(const char * name, python::object const &obj, std::function<T(void)> super)
{
	if (python::hasattr(obj, name))
	{
		auto func = obj.attr(name);
		return func;
	}
	return super;
}

template <typename T>
std::function<T(void)> get_method(python::object const& obj, const char* name, std::function<T(void)> default_method)
{
	if (python::hasattr(obj, name))
	{
		auto method = obj.attr(name);
		return method;
	}
	return default_method;
}

template <typename T>
std::function<T(indiemotion::Context)> get_method(python::object const& obj, const char* name, std::function<T(indiemotion::Context)> default_method)
{
	if (python::hasattr(obj, name))
	{
		auto method = obj.attr(name);
		return method;
	}
	return default_method;
}

/**
 * A wrapper for working with Python objects as the delegate items for the session.
 */
struct PyDelegateWrapper : public
	indiemotion::ServerDelegate,
	indiemotion::SceneDelegate,
	indiemotion::SessionDelegate,
	indiemotion::MotionDelegate
{
	python::object _py_delegate;
	indiemotion::logging::Logger _logger;

	/**
	 * Construct a delegate wrapper using one object for all the delegate types.
	 * @param delegate
	 */
	PyDelegateWrapper(python::object delegate) :
		_py_delegate(delegate)
	{
		_logger = indiemotion::logging::get_logger();
	}

	bool should_server_shutdown() override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::should_server_shutdown");
		GILLock gil;
		std::function<bool()> default_m = [](){ return false; };
		return get_method(_py_delegate, "should_server_shutdown", default_m)();
	}

	void on_server_start() override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::on_server_start");
		GILLock gil;
		std::function<void()> default_m = [](){};
		return get_method(_py_delegate, "on_server_start", default_m)();
	}

	void on_server_shutdown() override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::on_server_shutdown");
		GILLock gil;
		std::function<void()> default_m = [](){};
		return get_method(_py_delegate, "on_server_shutdown", default_m)();
	}

	void on_session_startup(indiemotion::Context ctx) override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::on_session_startup");
		GILLock gil;
		std::function<void(indiemotion::Context)> default_m = [](auto ctx){};
		return get_method(_py_delegate, "on_session_startup", default_m)(ctx);
	}

	void on_session_updated(indiemotion::Context ctx) override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::on_session_updated");
		GILLock gil;
		std::function<void(indiemotion::Context)> default_m = [](auto ctx){};
		return get_method(_py_delegate, "on_session_updated", default_m)(ctx);
	}

	void on_session_shutdown(indiemotion::Context ctx) override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::on_session_shutdown");
		GILLock gil;
		std::function<void(indiemotion::Context)> default_m = [](auto ctx){};
		return get_method(_py_delegate, "on_session_shutdown", default_m)(ctx);
	}

	bool should_session_shutdown(indiemotion::Context ctx) override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::should_session_shutdown");
		GILLock gil;
		std::function<bool()> default_m = [](){ return false; };
		return get_method(_py_delegate, "should_session_shutdown", default_m)();
	}

	void on_motion_updated(indiemotion::Context ctx) override
	{
		indiemotion::logging::log_trace_scope _("python-delegate::on_motion_updated");
		GILLock gil;
		std::function<void(indiemotion::Context)> default_m = [](auto ctx){};
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
		indiemotion::logging::log_trace_scope _("python-delegate::on_scene_updated");
		GILLock gil;
		std::function<void(indiemotion::Context)> default_m = [](auto ctx){};
		return get_method(_py_delegate, "on_scene_updated", default_m)(ctx);
	}
};
