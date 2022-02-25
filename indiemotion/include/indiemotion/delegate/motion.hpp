#pragma once
#include <indiemotion/context.hpp>

namespace indiemotion
{
	struct MotionDelegate
	{
		virtual void on_motion_updated(Context ctx) {};
	};
}