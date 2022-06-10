#pragma once
#include <memory>
#include <indiemotion/context.hpp>
#include <indiemotion/message.hpp>
#include <indiemotion/controller.hpp>

namespace indiemotion
{
    class Engine {
        std::shared_ptr<Context> _context;
        std::unordered_set<std::shared_ptr<Controller>> _controllers;

      public:
        void render(Message &&msg);
        void add(std::shared_ptr<Controller> ptr);
    };
}