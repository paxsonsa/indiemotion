#pragma once
#include <indiemotion/controllers/controller.hpp>

namespace indiemotion
{
    class DebugController: public Controller
    {
      public:
        void run(const Message &msg) override;
        void set_context(std::shared_ptr<Context> ctx) override;

        std::shared_ptr<Context> ctx;
    };
}