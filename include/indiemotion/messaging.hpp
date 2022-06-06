#pragma once

#include <indiemotion/proto/messages.pb.h>

namespace indiemotion
{
    using Message = protos::Message;

    /**
     * @brief Generate is new NetIdentifier
     *
     * @return std::string
     */
    std::string msg_gen_identifier_string();
}
