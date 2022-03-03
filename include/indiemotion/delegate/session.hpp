#pragma once
#include <indiemotion/context.hpp>

namespace indiemotion
{
	struct SessionDelegate
	{
		virtual void on_session_startup(Context ctx)
		{
		}
		virtual void on_session_updated(Context ctx)
		{
		}
		virtual void on_session_shutdown(Context ctx)
		{
		}
		virtual bool should_session_shutdown(Context ctx)
		{
			return false;
		}
	};
};