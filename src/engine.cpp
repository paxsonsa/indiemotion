#include <fmt/core.h>

#include <indiemotion/engine.hpp>
#include <indiemotion/controllers/debug.hpp>
#include <indiemotion/controllers/session.hpp>

namespace indiemotion
{
    void Engine::add(std::shared_ptr<Controller> ptr) {
        ptr->set_context(_context);
        _controllers.push_back(ptr);
    }

    void Engine::render(Message &&msg) {
        fmt::print("render() \n");
        std::vector<std::weak_ptr<Controller>> v;
        {
            v.reserve(_controllers.size());
            for (auto p : _controllers)
                v.emplace_back(std::weak_ptr<Controller>(p));
        }

        // For each session in our local list, try to acquire a strong
        // pointer. If successful, then run that controller with the given message.
        try {
            for (auto const &wp : v)
                if (auto sp = wp.lock())
                    sp->run(msg);
        } catch (std::exception& exception) {
            _context->clear_pending();
            throw exception;
        }
        _context->save();
    }
    std::vector<std::shared_ptr<Controller>>
    Engine::default_controllers() {
        auto controllers = std::vector<std::shared_ptr<Controller>>();

        controllers.push_back(std::make_shared<SessionController>());
//        controllers.push_back(std::make_shared<SceneController>());
//        controllers.push_back(std::make_shared<TrackController>());
//        controllers.push_back(std::make_shared<TrackController>());
        controllers.push_back(std::make_shared<DebugController>());

        return controllers;
    }
}