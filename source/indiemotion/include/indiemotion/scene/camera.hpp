#pragma once

namespace indiemotion
{
	struct Camera
	{
		std::string name;

		Camera(std::string name): name(name)
		{}

		bool operator==(const Camera& other) const
		{
			return (name == other.name);
		}

		bool operator!=(const Camera& other) const
		{
			return !(name == other.name);
		}
	};
}