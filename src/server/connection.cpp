#include <indiemotion/connection.hpp>

namespace indiemotion
{
    void Connection::send(std::shared_ptr<std::string const> const &ss)
    {
        _io->send(std::move(ss));
    }
}
