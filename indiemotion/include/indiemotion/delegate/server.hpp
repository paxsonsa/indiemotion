#pragma once

namespace indiemotion
{
	struct ServerDelegate
	{
		virtual bool should_server_shutdown() { return false; }
		virtual void on_server_start() {}
		virtual void on_server_shutdown() {}
	};
}