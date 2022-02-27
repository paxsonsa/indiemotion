#include "base.hpp"
#include "converters.hpp"
#include "delegate.hpp"
#include "server.hpp"

struct TestDummy {};

void validate_scene_delegate(indiemotion::Context ctx, python::object obj)
{

	auto delegate = PyDelegateWrapper(obj);
	delegate.get_scene_cameras(ctx);
	delegate.on_scene_updated(ctx);
}

void validate_motion_delegate(indiemotion::Context ctx, python::object obj)
{
	auto delegate = PyDelegateWrapper(obj);
	delegate.on_motion_updated(ctx);
}

void validate_session_delegate(indiemotion::Context ctx, python::object obj)
{
	auto delegate = PyDelegateWrapper(obj);
	delegate.on_session_startup(ctx);
	delegate.on_session_updated(ctx);
	delegate.on_session_shutdown(ctx);
	delegate.should_session_shutdown(ctx);
}


BOOST_PYTHON_MODULE (indiemotion)
{
	indiemotion::logging::init_logging();

	// Optional std::string Conversions
	python::to_python_converter<std::optional<std::string>,
							to_python_optional<std::string> >();
	from_python_optional<std::string>();

	python::enum_<indiemotion::MotionStatus>("motion_status")
		.value("idle", indiemotion::MotionStatus::Idle)
		.value("live", indiemotion::MotionStatus::Live)
		.value("recording", indiemotion::MotionStatus::Recording);

	python::class_<indiemotion::Vector3>("Vector3")
		.def_readwrite("x", &idm::Vector3::x)
		.def_readwrite("y", &idm::Vector3::y)
		.def_readwrite("z", &idm::Vector3::z)
		.def("__eq__", &idm::Vector3::operator==);

	python::class_<indiemotion::MotionXForm>("MotionXForm")
		.def_readwrite("translation", &idm::MotionXForm::translation)
		.def_readwrite("orientation", &idm::MotionXForm::orientation);

	python::class_<indiemotion::MotionContext>("MotionContext", python::init<>())
		.def_readwrite("status", &indiemotion::MotionContext::status)
		.def_readwrite("current_xform", &indiemotion::MotionContext::current_xform);

	python::class_<idm::SceneContext>("SceneContext", python::init<>());

	python::class_<idm::SessionContext>("SessionContext", python::init<>());

	python::class_<idm::Context>("Context", python::init<>());

	python::class_<idm::SceneCamera>("SceneCamera", python::init<std::string>())
		.def_readwrite("name", &idm::SceneCamera::name, "The display name of the camera");

	python::class_<PyServer, boost::noncopyable>("Server", python::init<python::object>())
	    .def("start", &PyServer::start)
		.def("wait", &PyServer::wait)
		.def("start_blocking", &PyServer::start_blocking);

	/*
	 * Testing Tools for clients.
	 */
	python::scope test = python::class_<TestDummy>("test");
	python::def("validate_scene_delegate", validate_scene_delegate, "test delegate implementation in C++ context");
	python::def("validate_motion_delegate", validate_motion_delegate, "test delegate implementation in C++ context");
	python::def("validate_session_delegate", validate_session_delegate, "test delegate implementation in C++ context");
}