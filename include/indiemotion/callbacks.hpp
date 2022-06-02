#pragma once

#include <indiemotion/messaging.hpp>

namespace indiemotion
{
    class GlobalCallbacks {
      public:
        virtual ~GlobalCallbacks() = default;
        virtual void pre_websocket_session(std::shared_ptr<MessageSender>) {};
        virtual void post_websocket_session(std::shared_ptr<MessageSender>) {};
    };
}
