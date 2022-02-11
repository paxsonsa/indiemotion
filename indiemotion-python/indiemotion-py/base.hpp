//
// Created by Andrew Paxson on 2022-02-10.
//
#include <optional>

template<typename T>
struct to_python_optional
{
	static PyObject* convert(const std::optional<T>& obj)
	{
		if (obj) return py::incref(py::object(*obj).ptr());
		return py::object().ptr();
	}

};

template<typename T>
struct from_python_optional
{
	static void* convertible(PyObject* obj_ptr)
	{
		typename py::extract<T>(obj_ptr).check() ? obj_ptr : 0;
	}

	static void construct(PyObject* obj_ptr,
		py::converter::rvalue_from_python_stage1_data* data)
	{
		const T value = typename py::extract<T>(obj_ptr);
		void* storage = (
			(py::converter::rvalue_from_python_storage<boost::optional<T> >*)
				data)->storage.bytes;
		new(storage) std::optional<T>(value);
		data->convertible = storage;
		std::cout << data->convertible << std::endl;
	}

	from_python_optional()
	{
		py::converter::registry::push_back(
			&convertible,
			&construct,
			py::type_id<std::optional<T>>()
		);
	}
};