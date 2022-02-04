//
// Created by Andrew Paxson on 2022-02-03.
//
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <indiemotion/context.hpp>

using namespace indiemotion;
using namespace boost::python;


char const* greet()
{
	return "hello, world";
}

BOOST_PYTHON_MODULE(indiemotion)
{
	class_<SessionContext>("SessionContext")
		.def_readwrite("name", &SessionContext::name)
		.def_readwrite("initialized", &SessionContext::initialized)
		.def_readwrite("shutdown", &SessionContext::shutdown);

	enum_<MotionStatus>("motion_status")
	    .value("idle", MotionStatus::Idle)
		.value("live", MotionStatus::Live)
		.value("recording", MotionStatus::Recording);

	class_<Vector3>("Vector3")
	    .def_readwrite("x", &Vector3::x)
	    .def_readwrite("y", &Vector3::y)
	    .def_readwrite("z", &Vector3::z);

	class_<MotionXForm>("MotionXForm")
	    .def_readwrite("translation", &MotionXForm::translation)
		.def_readwrite("orientation", &MotionXForm::orientation);

	class_<MotionContext>("MotionContext")
		.def_readwrite("status", &MotionContext::status)
		.def_readwrite("current_xform", &MotionContext::current_xform);

	class_<Camera>("Camera", init<std::string>())
	    .def_readwrite("name", &Camera::name);

//	class_<std::vector<Camera>>("_CameraList")
//		.def(vector_indexing_suite<std::vector<Camera>>());
//	    .def(vector_index_suite<std::vector<Camera>());

	class_<SceneContext>("SceneContext")
		.def_readwrite("active_camera_name", &SceneContext::active_camera_name)
		// https://stackoverflow.com/questions/36485840/wrap-boostoptional-using-boostpython
		// https://misspent.wordpress.com/2009/09/27/how-to-write-boost-python-converters/

		.def_readwrite("cameras", &SceneContext::cameras);

	def("greet", greet);
}