#include "base.hpp"
#include "converters.hpp"
#include "delegate.hpp"

struct TestDummy {};

void validate_scene_delegate(indiemotion::Context ctx, python::object obj)
{
	if (python::hasattr(obj, "get_scene_cameras"))
	{
		python::object cb = obj.attr("get_scene_cameras");
		python::object result = cb();

		// Test None Type
		python::list cameras = python::extract<python::list>(result);
		std::cout << "cameras: " << len(cameras) << std::endl;
		python::stl_input_iterator<indiemotion::SceneCamera> begin(cameras), end;
		for (auto it = begin; it != end; ++it)
		{
			it->name;
		}
	}
	if (python::hasattr(obj, "on_scene_updated"))
	{
		python::object cb = obj.attr("on_scene_updated");
		cb(ctx);
	}
}

void validate_motion_delegate(indiemotion::Context ctx, python::object obj)
{
	if (python::hasattr(obj, "on_motion_updated"))
	{
		python::object cb = obj.attr("on_motion_updated");
		cb(ctx);
	}
}

void validate_session_delegate(indiemotion::Context ctx, python::object obj)
{
	if (python::hasattr(obj, "on_session_startup"))
	{
		python::object cb = obj.attr("on_session_startup");
		cb(ctx);
	}

	if (python::hasattr(obj, "on_session_startup"))
	{
		python::object cb = obj.attr("on_session_startup");
		cb(ctx);
	}
	if (python::hasattr(obj, "on_session_updated"))
	{
		python::object cb = obj.attr("on_session_updated");
		cb(ctx);
	}
	if (python::hasattr(obj, "on_session_shutdown"))
	{
		python::object cb = obj.attr("on_session_shutdown");
		cb(ctx);
	}
	if (python::hasattr(obj, "should_session_shutdown"))
	{
		python::object cb = obj.attr("should_session_shutdown");
		cb(ctx);
	}
}


BOOST_PYTHON_MODULE (indiemotion)
{
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



	/*
	 * Testing Tools for clients.
	 */
	python::scope test = python::class_<TestDummy>("test");
	python::def("validate_scene_delegate", validate_scene_delegate, "test delegate implementation in C++ context");
	python::def("validate_motion_delegate", validate_motion_delegate, "test delegate implementation in C++ context");
	python::def("validate_session_delegate", validate_session_delegate, "test delegate implementation in C++ context");
}