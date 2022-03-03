#pragma once
#include <optional>
#include <boost/python.hpp>
#include <indiemotion/indiemotion.hpp>
#include <indiemotion/debug.hpp>

namespace idm = indiemotion;

namespace boost::python {
	bool hasattr(object o, const char* name)
	{
		return PyObject_HasAttrString(o.ptr(), name);
	}
}
namespace python = boost::python;

struct GILLock
{
	PyGILState_STATE gstate;
	GILLock()
	{
		gstate = PyGILState_Ensure();
	}
	~GILLock() {
		PyGILState_Release(gstate);
	}
};

struct GILRelease
{
	PyThreadState *_save;
	GILRelease()
	{
		_save = PyEval_SaveThread();
	}
	~GILRelease() {
		PyEval_RestoreThread(_save);
	}
};

template<typename T>
std::function<T(void)> get_method(python::object const& obj, const char* name, std::function<T(void)> default_method)
{
	if (python::hasattr(obj, name))
	{
		auto method = obj.attr(name);
		return method;
	}
	return default_method;
}

template<typename T>
std::function<T(indiemotion::Context)> get_method(python::object const& obj,
	const char* name,
	std::function<T(indiemotion::Context)> default_method)
{
	if (python::hasattr(obj, name))
	{
		auto method = obj.attr(name);
		return method;
	}
	return default_method;
}