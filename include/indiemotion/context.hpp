#pragma once
#include <any>
#include <deque>
#include <iostream>
#include <string>
#include <set>
#include <unordered_map>
#include <iterator> // For std::forward_iterator_tag
#include <vector> // For std::forward_iterator_tag

namespace indiemotion
{

    struct Context: public std::enable_shared_from_this<Context>
    {
        struct Value {
            Value() = default;

            explicit Value(std::shared_ptr<std::any> obj): wrapped_value(std::move(obj)) {}
            explicit Value(std::shared_ptr<std::any> &&obj): wrapped_value(std::move(obj)) {}

            std::shared_ptr<std::any> wrapped_value = nullptr;

            template <typename T>
            T as() const { return std::any_cast<T>(*wrapped_value); }

            bool empty() const
            { return wrapped_value == nullptr; }
        };

        using Revision = std::unordered_map<std::string, std::shared_ptr<std::any>>;
        struct RevisionView
        {
            RevisionView() = default;
            explicit RevisionView(std::vector<std::shared_ptr<Revision>> &&);
            Value operator[] (std::string) const;
            std::optional<Value> get(std::string) const;

          private:
            std::vector<std::shared_ptr<Revision>> _revs;
        };


        RevisionView pending;
        RevisionView saved;
        RevisionView current;

        Context();


        void update(std::string key, std::any value);
        void save();
        RevisionView revision(int);
        RevisionView rollback();
        void clear_pending();

        protected:
          std::shared_ptr<Revision> _next_rev = {};
          std::deque<std::shared_ptr<Revision>> _revisions;
    };
}