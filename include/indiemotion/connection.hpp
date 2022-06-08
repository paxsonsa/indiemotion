#pragma once
#include <memory>

#include <indiemotion/message.hpp>

namespace indiemotion
{
    class ConnectionIO
    {
      public:
        virtual void send(std::shared_ptr<std::string const> const &ss) = 0;
        virtual void send(Message &&msg) = 0;
    };

    struct ConnectionInfo
    {
        std::string name;
        // TODO: Role
    };

    class Connection
    {
        std::shared_ptr<ConnectionIO> _io = nullptr;

      public:
        std::unique_ptr<ConnectionInfo> info = nullptr;
        explicit Connection(std::shared_ptr<ConnectionIO> io): _io(std::move(io)) {}

        inline void send(std::shared_ptr<std::string const> const &ss) {
            _io->send(ss);
        }

        inline void send(Message &&msg) {
            _io->send(std::move(msg));
        }

        [[nodiscard]] inline bool registered() const
        {
            return info != nullptr;
        }

        static std::shared_ptr<Connection> from(std::shared_ptr<ConnectionIO> io)
        {
            return std::make_shared<Connection>(io);
        }
    };
}