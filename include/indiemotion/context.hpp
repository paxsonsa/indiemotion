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

        /**
         * Stage an update to the given key with the given value
         *
         * The change to the key is not saved permanently until
         * save() is called.
         *
         * @param key
         * @param value
         */
        void update(std::string key, std::any value);

        /**
         * Save the current pending changes.
         *
         * If no changes are pending, it's a NoOp.
         *
         */
        void save();

        /**
         * Retrieve a revision that is N versions back from the back.
         *
         * If the revision request is too far back, an empty revision is
         * returned.
         *
         * @return
         */
        RevisionView revision(int);

        /**
         * Rollback the latest revision and return it.
         *
         * If there is nothing to rollback, an empty revision view is returned.
         *
         * @return the last revision removed.
         */
        RevisionView rollback();

        /**
         * Clear any pending changes.
         */
        void clear_pending();

        protected:
          std::shared_ptr<Revision> _next_rev = {};
          std::deque<std::shared_ptr<Revision>> _revisions;
    };
}