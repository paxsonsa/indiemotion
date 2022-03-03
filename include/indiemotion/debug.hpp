#pragma once
#include <indiemotion/common.hpp>

namespace indiemotion
{
	struct DebugSampleCounter
	{
		long _last_time;
		int running_frame_count = 0;
		int current_frame_count = 60;

		DebugSampleCounter() : _last_time(
			std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000)
		{
		}

		int poll()
		{
			auto time = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000;
			if ((time - _last_time) >= 1000)
			{
				_last_time = std::move(time);
				current_frame_count = running_frame_count;
				running_frame_count = 0;
			}
			running_frame_count += 1;
			return current_frame_count;
		}
	};
}