#pragma once
#include <optional>
#include <boost/python.hpp>
#include <indiemotion/indiemotion.hpp>

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
	PyThreadState *_save;;
	GILRelease()
	{
		_save = PyEval_SaveThread();
	}
	~GILRelease() {
		PyEval_RestoreThread(_save);
	}
};