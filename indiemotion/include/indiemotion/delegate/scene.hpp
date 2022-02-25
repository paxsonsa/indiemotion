#pragma once
#include <indiemotion/context.hpp>
#include <indiemotion/scene/camera.hpp>

namespace indiemotion {
	struct SceneDelegate
	{
		virtual std::vector<SceneCamera> get_scene_cameras(Context ctx) = 0;
		virtual void on_scene_updated(Context ctx) {};
	};
};