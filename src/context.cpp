#include <indiemotion/context.hpp>

namespace indiemotion
{
    Context::Context() {
        _previous = std::make_shared<ContextMap>();
        _current = std::make_shared<ContextMap>();
        _diff = std::make_shared<ContextMap>();
    }

    void Context::commit() {
        _diff->insert(_current->begin(), _current->end());
        _previous = std::move(_current);
        _current = std::move(_diff);
        _diff = std::make_shared<ContextMap>();
    }

    void Context::revert() {
        _diff = std::make_shared<ContextMap>();
    }

    void Context::update(std::string key, std::any value) {
        auto ptr = std::make_shared<std::any>(value);
        auto entry = ContextEntry(std::move(ptr));
        _diff->insert_or_assign(key, std::move(entry));
    }

    ContextEntry Context::get(std::string key) const {
        auto search = _diff->find(key);
        if (search != _diff->end()) {
            return search->second;
        }
        return current(key);
    }

    ContextEntry Context::current(std::string key) const {
        auto search = _current->find(key);
        if (search != _current->end()) {
            return search->second;
        }
        return ContextEntry();
    }

    ContextEntry Context::previous(std::string key) const {
        auto search = _previous->find(key);
        if (search != _previous->end()) {
            return search->second;
        }
        return ContextEntry();
    }


}