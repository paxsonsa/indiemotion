#pragma once

namespace indiemotion
{
	struct SessionContext final
	{
		std::string name = "";
		bool initialized = false;
		bool shutdown = false;

		static SessionContext create() {
			return SessionContext();
		}
	};
}