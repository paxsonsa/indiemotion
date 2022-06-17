#pragma once
#include <any>
#include <deque>
#include <iterator>
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

            friend std::ostream& operator<<(std::ostream& os, const Value& vt)
            {
                auto &value = *vt.wrapped_value;
                if (int* i = std::any_cast<int>(&value)) {
                    os << *i;
                } else if (auto* l = std::any_cast<long>(&value)) {
                    os << *l;
                } else if (auto* d = std::any_cast<double>(&value)) {
                    os << *d;
                } else if (auto* s = std::any_cast<std::string>(&value)) {
                    os << *s;
                } else {
                    os << "<unknown>";
                }
                return os;
            }
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

        struct KeyEntry
        {
            std::string name;
            Value current;
            std::optional<Value> next;
        };

        class Iterator: public std::iterator<
                             std::input_iterator_tag,   // iterator_category
                             KeyEntry,                  // value_type
                             std::ptrdiff_t,            // difference_type
                             const KeyEntry*,           // pointer
                             KeyEntry&                  // reference
                             >
        {
          public:
            Iterator(Context* ctx) : _ctx(ctx)
            {
                if (_ctx == nullptr)
                    return;

                _cur = new KeyEntry{};
                std::set<std::string> keys;

                if (!ctx->_revisions.empty())
                {
                    auto rev = _ctx->_revisions.back();
                    for (const auto pair : *rev) {
                        keys.insert(pair.first);
                    }
                }

                auto rev = _ctx->_next_rev;
                for (const auto pair : *rev)
                {
                    keys.insert(pair.first);
                }

                _keys = std::vector<std::string>(keys.begin(), keys.end());
                load_key();
            }

            reference operator*() const { return *_cur; }
            pointer operator->() { return _cur; }

            // Prefix increment
            Iterator& operator++() {
                _idx++;
                load_key();
                return *this;
            }

            // Postfix increment
            Iterator operator++(int)
            {
                Iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            friend bool operator== (const Iterator& a, const Iterator& b)
            {
                return a._cur == b._cur;
            };
            friend bool operator!= (const Iterator& a, const Iterator& b)
            {
                return a._cur != b._cur;
            };

          private:
            Context *_ctx = nullptr;
            int _idx = 0;
            KeyEntry *_cur = nullptr;
            std::vector<std::string> _keys;

            void load_key()
            {
                if (_idx >= _keys.size())
                {
                    _cur = nullptr;
                    return;
                }

                auto key = _keys[_idx];
                _cur->name = key;
                _cur->current = _ctx->current[key];
                _cur->next = _ctx->pending.get(key);
            }
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

        Iterator begin() { return {this}; }
        Iterator end() { return {nullptr}; }

        protected:
          std::shared_ptr<Revision> _next_rev = {};
          std::deque<std::shared_ptr<Revision>> _revisions;
    };
}