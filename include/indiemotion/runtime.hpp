#pragma once
#include <memory>
#include <unordered_set>

#include <google/protobuf/util/json_util.h>

#include <indiemotion/connection.hpp>
#include <indiemotion/messaging.hpp>

namespace indiemotion {
    class Runtime {
      public:
        virtual void join(std::shared_ptr<Connection>) = 0;
        virtual void leave(std::shared_ptr<Connection>) = 0;
        virtual void receive(std::shared_ptr<Connection>, Message &&msg) = 0;
    };

    class EchoRuntime : public Runtime {
        // This mutex synchronizes all access to sessions_
        std::mutex mutex_;

        // Keep a list of all the connected clients
        std::unordered_set<std::shared_ptr<Connection>> _connections;

      public:
        void join(std::shared_ptr<Connection> ptr) override {
            std::lock_guard<std::mutex> lock(mutex_);
            _connections.insert(ptr);
        }
        void leave(std::shared_ptr<Connection> ptr) override {
            std::lock_guard<std::mutex> lock(mutex_);
            _connections.erase(ptr);
        }
        void receive(std::shared_ptr<Connection> ptr,
                     Message &&message) override {

            // Put the message in a shared pointer so we can re-use it for each
            // client
            std::string msg_str;
            google::protobuf::util::MessageToJsonString(message, &msg_str);
            auto const ss =
                std::make_shared<std::string const>(std::move(msg_str));

            // Make a local list of all the weak pointers representing
            // the sessions, so we can do the actual sending without
            // holding the mutex:
            std::vector<std::weak_ptr<Connection>> v;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                v.reserve(_connections.size());
                for (auto p : _connections)
                    v.emplace_back(std::weak_ptr<Connection>(p));
            }

            // For each session in our local list, try to acquire a strong
            // pointer. If successful, then send the message on that session.
            for (auto const &wp : v)
                if (auto sp = wp.lock())
                    sp->send(ss);
        }
    };

    class EmbeddedRuntime : public Runtime {
        // This mutex synchronizes all access to sessions_
        std::mutex mutex_;

        // Keep a list of all the connected clients
        std::unordered_set<std::shared_ptr<Connection>> _connections;

      public:
        explicit EmbeddedRuntime() = default;

        void join(std::shared_ptr<Connection> ptr) override {
            std::lock_guard<std::mutex> lock(mutex_);
            _connections.insert(ptr);
        }
        void leave(std::shared_ptr<Connection> ptr) override {
            std::lock_guard<std::mutex> lock(mutex_);
            _connections.erase(ptr);
        }
        void receive(std::shared_ptr<Connection> ptr,
                     Message &&msg) override {

        }
    };
} // namespace indiemotion