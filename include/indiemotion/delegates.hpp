#pragma once
#include <indiemotion/context.hpp>
#include <indiemotion/delegate/session.hpp>
#include <indiemotion/delegate/scene.hpp>
#include <indiemotion/delegate/motion.hpp>
#include <indiemotion/delegate/server.hpp>

namespace indiemotion {
	struct DelegateInfo
	{
		std::shared_ptr <SessionDelegate> session;
		std::shared_ptr <SceneDelegate> scene;
		std::shared_ptr <MotionDelegate> motion;
		std::shared_ptr <ServerDelegate> server;
	};
}