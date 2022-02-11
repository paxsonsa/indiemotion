#pragma once
#include <indiemotion/delegates.hpp>

namespace idm = indiemotion;

struct PyDelegate: public idm::SessionDelegate, idm::SceneDelegate, idm::MotionDelegate
{
	void motion_updated(indiemotion::Context ctx) override;
	std::vector<idm::Camera> get_scene_cameras() override;
	void scene_updated(indiemotion::Context ctx) override;
	void session_updated(indiemotion::Context ctx) override;
	void on_shutdown(indiemotion::Context ctx) override;
};