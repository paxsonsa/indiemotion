#pragma once
#include "base.hpp"

struct PyMotionDelegate: indiemotion::MotionDelegate, python::wrapper<indiemotion::MotionDelegate>
{
	void on_motion_updated(indiemotion::Context ctx) override
	{
		if (python::override on_motion_updated = this->get_override("on_motion_updated")) {
			on_motion_updated(ctx);
		} else {
			indiemotion::MotionDelegate::on_motion_updated(ctx);
		}
	}

	void default_on_motion_updated(indiemotion::Context ctx)
	{
		this->indiemotion::MotionDelegate::on_motion_updated(ctx);
	}
};

// TODO: Create Python Class Binding
// TODO: Convert Python List of Cameras into Vector of Cameras.
struct PySceneDelegate: indiemotion::SceneDelegate, python::wrapper<indiemotion::SceneDelegate>
{
	std::vector<indiemotion::SceneCamera> get_scene_cameras() override
	{
		std::cout << "Called...." << std::endl;
		return this->get_override("get_scene_cameras")();
	}

	void on_scene_updated(indiemotion::Context ctx) override
	{
		SceneDelegate::on_scene_updated(ctx);
		if (python::override on_scene_updated = this->get_override("on_scene_updated")) {
			on_scene_updated(ctx);
		} else {
			indiemotion::SceneDelegate::on_scene_updated(ctx);
		}
	}

	void default_on_scene_updated(indiemotion::Context ctx)
	{
		this->indiemotion::SceneDelegate::on_scene_updated(ctx);
	}
};

struct PySessionDelegate
{
	virtual void on_started(python::object ctx) {}
	virtual void on_session_updated(python::object ctx) {}
	virtual void on_shutdown(python::object ctx) {}
};