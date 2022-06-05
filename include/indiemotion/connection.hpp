#pragma once
#include <memory>

namespace indiemotion
{
    class ConnectionIO
    {
      public:
        virtual void send(std::shared_ptr<std::string const> const &ss) = 0;
    };

    class Connection
    {
        std::shared_ptr<ConnectionIO> _io = nullptr;

      public:
        explicit Connection(std::shared_ptr<ConnectionIO> io): _io(std::move(io)) {}

        void send(std::shared_ptr<std::string const> const &ss);

        static std::shared_ptr<Connection> from(std::shared_ptr<ConnectionIO> io)
        {
            return std::make_shared<Connection>(io);
        }
    };
}