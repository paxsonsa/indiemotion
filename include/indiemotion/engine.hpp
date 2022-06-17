#pragma once
#include "indiemotion/controllers/controller.hpp"
#include <indiemotion/context.hpp>
#include <indiemotion/message.hpp>
#include <memory>

namespace indiemotion
{
    class Engine {
        std::shared_ptr<Context> _context;
        std::vector<std::shared_ptr<Controller>> _controllers;

      public:
        static std::vector<std::shared_ptr<Controller>> default_controllers();

        explicit Engine(std::vector<std::shared_ptr<Controller>> &&controllers): _controllers(std::move(controllers)) {
            _context = std::make_shared<Context>();
            for (auto const &ptr : _controllers)
                ptr->set_context(_context);
        }
        void render(Message &&msg);
        void add(std::shared_ptr<Controller> ptr);
    };
}