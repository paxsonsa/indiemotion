//
// Created by Andrew Paxson on 2022-02-03.
//
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <indiemotion/context.hpp>

using namespace indiemotion;
namespace py = boost::python;

char const* greet()
{
	return "hello, world";
}

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

struct _PySceneContextWrapper
{
	std::optional<std::string> active_camera_name;
	py::list cameras;
};

BOOST_PYTHON_MODULE (indiemotion)
{
	// Optional std::string Conversions
	py::to_python_converter<std::optional<std::string>,
							to_python_optional<std::string> >();
	from_python_optional<std::string>();

	py::class_<SessionContext>("SessionContext")
		.def_readwrite("name", &SessionContext::name)
		.def_readwrite("initialized", &SessionContext::initialized)
		.def_readwrite("shutdown", &SessionContext::shutdown);

	py::enum_<MotionStatus>("motion_status")
		.value("idle", MotionStatus::Idle)
		.value("live", MotionStatus::Live)
		.value("recording", MotionStatus::Recording);

	py::class_<Vector3>("Vector3")
		.def_readwrite("x", &Vector3::x)
		.def_readwrite("y", &Vector3::y)
		.def_readwrite("z", &Vector3::z)
		.def("__eq__", &Vector3::operator==);

	py::class_<MotionXForm>("MotionXForm")
		.def_readwrite("translation", &MotionXForm::translation)
		.def_readwrite("orientation", &MotionXForm::orientation);

	py::class_<MotionContext>("MotionContext")
		.def_readwrite("status", &MotionContext::status)
		.def_readwrite("current_xform", &MotionContext::current_xform);

	py::class_<Camera>("Camera", py::init<std::string>())
		.def_readwrite("name", &Camera::name)
		.def("__eq__", &Camera::operator==)
		.def("__ne__", &Camera::operator!=);

	py::class_<_PySceneContextWrapper>("SceneContext")
		.add_property("active_camera",
			py::make_getter(&_PySceneContextWrapper::active_camera_name,
								  py::return_value_policy<py::return_by_value>()),
			py::make_setter(&_PySceneContextWrapper::active_camera_name,
				                  py::return_value_policy<py::return_by_value>())
		)
//		("active_camera_name", &SceneContext::active_camera_name)
			// https://stackoverflow.com/questions/36485840/wrap-boostoptional-using-boostpython
			// https://misspent.wordpress.com/2009/09/27/how-to-write-boost-python-converters/

		.def_readwrite("cameras", &_PySceneContextWrapper::cameras);

	py::def("greet", greet);
}