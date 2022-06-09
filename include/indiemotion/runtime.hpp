#pragma once
#include <memory>
#include <unordered_set>

#include <google/protobuf/util/json_util.h>

#include <indiemotion/error.hpp>
#include <indiemotion/connection.hpp>
#include <indiemotion/message.hpp>
#include <indiemotion/message_gen.hpp>

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

        void _leave(std::shared_ptr<Connection> ptr) {
            _connections.erase(ptr);
        }

      public:
        explicit EmbeddedRuntime() = default;

        void join(std::shared_ptr<Connection> ptr) override {
            std::scoped_lock<std::mutex> lock(mutex_);
            _connections.insert(ptr);
        }
        void leave(std::shared_ptr<Connection> ptr) override {
            std::scoped_lock<std::mutex> lock(mutex_);
            _leave(ptr);
        }

        void receive(std::shared_ptr<Connection> conn,
                     Message &&msg) override {
            std::scoped_lock<std::mutex> lock(mutex_);

            /**
             * TODO -- Client Connection Protocol
             * - Register Client
             *    - Name
             *    - Role (Observer, Application, Controller)
             */
            if (!conn->registered()) {
                // if message is not client id, fail else compute client info
                if (!msg.has_client_info()) {
//                    TODO: conn->prepare_to_close();
                    conn->send(
                        error_message(Error::BadMessage, "client is not registered.")
                    );
                    _leave(conn);
                } else {
                    // Register Client Information with Connection
                    // - mark connection as registered.
                    auto client_info = std::make_unique<ConnectionInfo>();
                    client_info->name = msg.client_info().name();
                    conn->info = std::move(client_info);
                }
                return;
            }
            /**
             * ContextTree
             *
             * session/id
             * session/name
             * session/device/id
             * session/device/name
             * session/host/id
             * session/host/name
             * session/user
             *
             * scene/origin
             * scene/camera/id
             * scene/camera/origin/x
             * scene/camera/origin/y
             * scene/camera/origin/x
             * scene/camera/fov
             * scene/camera/resolution/x
             * scene/camera/resolution/y
             *
             * motion/frame/number
             * motion/frame/timecode
             * motion/frame/channel/image
             * motion/frame/channel/focal
             * motion/frame/channel/aperture
             * motion/frame/channel/xform
             * motion/frame/channel/
             */

            /**
             * Context Lifecycle
             *
             * ContextView
             *    get()
             *
             * Context: ContextView
             *    - current: ContextView
             *    - previous: ContextView
             *    - update(ContextEntry)
             *    - diff() -> [ContextEntry]
             *    - commit() -> bool
             *    - revert() -> bool
             *    - update()
             *    - get(T)
             *    - current(T)
             *    - previous(T)
             *
             * ContextEntry<T>
             *      name: std::string
             *      animatable: bool
             *      prev: T
             *      current: T
             *
             *      update(T)
             *
             *
             * context->update(key, T)
             * context->get(key)
             *
             * context.commit()
             *
             * context.previous() -> ContextView
             * context.current() -> ContextView
             *
             *
             */

            /**
             * - Compute State Changes (as object? up front?)
             *     - Seperate object or apart of the controller?
             *     - Should controllers just take incoming messages instead of a
             *       pre-calcing it? Yes: Simpler to prototype, No: Reverting changes is more difficult,
         *
             * - Iterate through controllers sharing state changes
             * - Gather messages and errors (one error poisons messages)
             * - Queue Responses
             */

        }
    };
} // namespace indiemotion