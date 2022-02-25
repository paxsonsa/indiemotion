#pragma once
#include "base.hpp"

template <typename T>
std::function<T(indiemotion::Context)> get_func_or_super(const char * name, python::object obj, std::function<T(indiemotion::Context)> super)
{
	if (python::hasattr(obj, name))
	{
		auto func = obj.attr(name);
		return func;
	}
	return super;
}

struct PyDelegateWrapper : indiemotion::SceneDelegate, indiemotion::SessionDelegate, indiemotion::MotionDelegate
{
	python::object _session_delegate;
	python::object _scene_delegate;
	python::object _motion_delegate;

	PyDelegateWrapper(python::object session_delegate, python::object scene_delegate, python::object motion_delegate) :
		_session_delegate(session_delegate),
		_scene_delegate(scene_delegate),
		_motion_delegate(motion_delegate)
	{
	}

	PyDelegateWrapper(python::object delegate) :
		_session_delegate(delegate),
		_scene_delegate(delegate),
		_motion_delegate(delegate)
	{
	}

	void on_session_startup(indiemotion::Context ctx) override
	{
		PyGILLock gil;
		std::function<void(indiemotion::Context)> super = std::bind(&SessionDelegate::on_session_startup, this, std::placeholders::_1);
		auto func = get_func_or_super("on_session_startup", _session_delegate, super);
		func(ctx);
	}

	void on_session_updated(indiemotion::Context ctx) override
	{
		PyGILLock gil;
		std::function<void(indiemotion::Context)> super = std::bind(&SessionDelegate::on_session_updated, this, std::placeholders::_1);
		auto func = get_func_or_super("on_session_updated", _session_delegate, super);
		func(ctx);
	}

	void on_session_shutdown(indiemotion::Context ctx) override
	{
		PyGILLock gil;
		std::function<void(indiemotion::Context)> super = std::bind(&SessionDelegate::on_session_shutdown, this, std::placeholders::_1);
		auto func = get_func_or_super("on_session_shutdown", _session_delegate, super);
		std::cout << "on_session_shutdown() here" << std::endl;
		func(ctx);
	}

	bool should_session_shutdown(indiemotion::Context ctx) override
	{
		PyGILLock gil;
		std::function<bool(indiemotion::Context)> super = std::bind(&SessionDelegate::should_session_shutdown, this, std::placeholders::_1);
		auto func = get_func_or_super("should_session_shutdown", _session_delegate, super);
		return func(ctx);
	}

	void on_motion_updated(indiemotion::Context ctx) override
	{
		PyGILLock gil;
		std::function<void(indiemotion::Context)> super = std::bind(&MotionDelegate::on_motion_updated, this, std::placeholders::_1);
		auto func = get_func_or_super("on_motion_updated", _motion_delegate, super);
		return func(ctx);
	}

	std::vector<indiemotion::SceneCamera> get_scene_cameras(indiemotion::Context ctx) override
	{
		PyGILLock gil;
		if (!python::hasattr(_scene_delegate, "get_scene_cameras"))
		{
			PyErr_Format(PyExc_AttributeError, "missing expected method 'get_scene_cameras'");
			throw indiemotion::ApplicationException("application scene delegate is misconfigured.", true);
		}

		auto py_func = _scene_delegate.attr("get_scene_cameras");
		python::object result = py_func(ctx);
		python::list items = python::extract<python::list>(result);
		python::stl_input_iterator<indiemotion::SceneCamera> begin(items), end;
		return std::vector<indiemotion::SceneCamera>(begin, end);
	}

	void on_scene_updated(indiemotion::Context ctx) override
	{
		PyGILLock gil;
		std::function<void(indiemotion::Context)> super = std::bind(&SceneDelegate::on_scene_updated, this, std::placeholders::_1);
		auto func = get_func_or_super("on_scene_updated", _motion_delegate, super);
		return func(ctx);
	}
};
