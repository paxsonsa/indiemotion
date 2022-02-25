#pragma once

namespace indiemotion
{
	struct SceneCamera
	{
		std::string name;

		SceneCamera(std::string name): name(name)
		{}

		bool operator==(const SceneCamera& other) const
		{
			return (name == other.name);
		}

		bool operator!=(const SceneCamera& other) const
		{
			return !(name == other.name);
		}
	};
}