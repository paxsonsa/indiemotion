#pragma once
#include <indiemotion-python/base.hpp>

template<typename T>
struct to_python_optional
{
	static PyObject* convert(const std::optional<T>& obj)
	{
		if (obj) return python::incref(python::object(*obj).ptr());
		return python::object().ptr();
	}
};

template<typename T>
struct from_python_optional
{
	static void* convertible(PyObject* obj_ptr)
	{
		return typename python::extract<T>(obj_ptr).check() ? obj_ptr : 0;
	}

	static void construct(PyObject* obj_ptr,
		python::converter::rvalue_from_python_stage1_data* data)
	{
		const T value = typename python::extract<T>(obj_ptr);
		void* storage = (
			(python::converter::rvalue_from_python_storage<boost::optional<T> >*)
				data)->storage.bytes;
		new(storage) std::optional<T>(value);
		data->convertible = storage;
		std::cout << data->convertible << std::endl;
	}

	from_python_optional()
	{
		python::converter::registry::push_back(
			&convertible,
			&construct,
			python::type_id<std::optional<T>>()
		);
	}
};
