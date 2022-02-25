#pragma once
#include <indiemotion/scene/camera.hpp>

namespace indiemotion
{
	struct SceneContext final
	{
		std::optional<std::string> active_camera_name;
		std::vector<SceneCamera> cameras = {};

		static SceneContext create() {
			return SceneContext();
		}
	};
}