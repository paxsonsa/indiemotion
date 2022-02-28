#pragma once

namespace indiemotion
{
	struct ServerDelegate
	{
		virtual void on_server_start() {}
		virtual bool should_server_shutdown() { return false; }
		virtual void on_server_shutdown() {}
		virtual void on_connection_start() {}
		virtual bool should_connection_close() { return false; }
		virtual void on_connection_close() {}
	};
}