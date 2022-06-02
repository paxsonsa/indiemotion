#pragma once
#include <indiemotion/server.hpp>

class DefaultCallbacks: public indiemotion::GlobalCallbacks
{
  public:
    void pre_websocket_session(std::shared_ptr<indiemotion::MessageSender> ptr) override {
        fmt::print("pre-websocket session: {}...\n", ptr->tag_str());

    }
    void post_websocket_session(std::shared_ptr<indiemotion::MessageSender> ptr) override {
        fmt::print("post-websocket session: {}...\n", ptr->tag_str());
    }
};
