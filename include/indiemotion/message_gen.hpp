#pragma once
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <indiemotion/error.hpp>
#include <indiemotion/message.hpp>



namespace indiemotion {

    /**
     * @brief Generate is new NetIdentifier
     *
     * @return std::string
     */
    std::string generate_new_identifier_string();

    /**
     * Make a new description and generate the ID.
     * @return unique pointer to a new description
     */
    Message make_message();

    Message error_message(Error error, std::string reason);
}
