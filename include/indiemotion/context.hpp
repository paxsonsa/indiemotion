#pragma once
#include <any>
#include <string>
#include <unordered_map>

namespace indiemotion
{

    struct ContextEntry
    {
        ContextEntry() = default;

        ContextEntry(std::shared_ptr<std::any> &&obj):
            wrapped_value(std::move(obj)) {}

        std::shared_ptr<std::any> wrapped_value = nullptr;

        template <typename T>
        T as() const {
            return std::any_cast<T>(*wrapped_value);
        }

        bool empty() const
        {
            return wrapped_value == nullptr;
        }
    };

    using ContextMap = std::unordered_map<std::string, ContextEntry>;

    struct Context
    {
        Context();
        void commit();
        void revert();

        void update(std::string key, std::any value);
        ContextEntry get(std::string key) const;
        ContextEntry current(std::string key) const;
        ContextEntry previous(std::string key) const;

      private:
        std::shared_ptr<ContextMap> _previous;
        std::shared_ptr<ContextMap> _diff;
        std::shared_ptr<ContextMap> _current;

    };
}