#include <indiemotion/message_gen.hpp>

namespace indiemotion
{
    /**
     * @brief Generate is new NetIdentifier
     *
     * @return std::string
     */
    std::string generate_new_identifier_string()
    {
        boost::uuids::random_generator generator;
        boost::uuids::uuid uuid = generator();
        return boost::uuids::to_string(uuid);
    }

    /**
     * Make a new description and generate the ID.
     * @return unique pointer to a new description
     */
    Message make_message()
    {
        auto id = generate_new_identifier_string();
        Message m;
        m.mutable_header()->set_id(id);
        return std::move(m);
    }

    Message error_message(Error error, std::string reason) {
        auto message = make_message();
        auto err = message.mutable_error();

        err->set_description(reason);
        switch (error)
        {
        case Error::BadMessage:
            err->set_type(Payload::Error::BadMessage);
            break;
        default:
            err->set_type(Payload::Error::Unknown);
        }

        return std::move(message);
    }
}