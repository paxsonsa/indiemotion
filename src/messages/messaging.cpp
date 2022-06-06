#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <indiemotion/messaging.hpp>


namespace indiemotion
{
    std::string msg_gen_identifier_string()
    {
        boost::uuids::random_generator generator;
        boost::uuids::uuid uuid = generator();
        return boost::uuids::to_string(uuid);
    }
}