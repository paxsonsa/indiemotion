#include <indiemotion/engine.hpp>

namespace indiemotion
{
    void Engine::add(std::shared_ptr<Controller> ptr) {
        ptr->set_context(_context);
        _controllers.insert(ptr);
    }

    void Engine::render(Message &&msg) {
        std::vector<std::weak_ptr<Controller>> v;
        {
            v.reserve(_controllers.size());
            for (auto p : _controllers)
                v.emplace_back(std::weak_ptr<Controller>(p));
        }

        // For each session in our local list, try to acquire a strong
        // pointer. If successful, then send the message on that session.
        try {
            for (auto const &wp : v)
                if (auto sp = wp.lock())
                    sp->run(msg);
        } catch (std::exception& exception) {
            _context->revert();
            throw exception;
        }
        _context->commit();
    }
}