#pragma once
#include <indiemotion/context.hpp>
#include <indiemotion/message.hpp>

namespace indiemotion
{
    class Controller {
      public:
        virtual void run(Message &msg) = 0;
        virtual void set_context(std::shared_ptr<Context> ctx) = 0;
    };
}