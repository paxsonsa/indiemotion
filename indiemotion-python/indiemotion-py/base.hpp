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


