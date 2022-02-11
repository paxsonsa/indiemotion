//
// Created by Andrew Paxson on 2022-02-10.
//
#include "delegate.hpp"

void PyDelegate::motion_updated(indiemotion::Context ctx)
{
	MotionDelegate::motion_updated(ctx);
}
std::vector<idm::Camera> PyDelegate::get_scene_cameras()
{
	return std::vector<idm::Camera>();
}
void PyDelegate::scene_updated(indiemotion::Context ctx)
{
	SceneDelegate::scene_updated(ctx);
}
void PyDelegate::session_updated(indiemotion::Context ctx)
{
	SessionDelegate::session_updated(ctx);
}
void PyDelegate::on_shutdown(indiemotion::Context ctx)
{
	SessionDelegate::on_shutdown(ctx);
}
