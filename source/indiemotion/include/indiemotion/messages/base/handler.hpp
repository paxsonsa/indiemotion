// Copyright (c) 2021 Andrew Paxson. All rights reserved. Used under
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
/* handlers.hpp */
#pragma once
#include <indiemotion/common.hpp>
#include <indiemotion/messages/base/message.hpp>
#include <indiemotion/responses/base/response.hpp>
#include <indiemotion/session/session.hpp>

namespace indiemotion::messages::base
{
    class Handler
    {
    public:
        virtual ~Handler() {}
        virtual std::optional<std::unique_ptr<responses::base::Response>>
        handleMessage(std::weak_ptr<session::Session> sessionPtr,
                      std::unique_ptr<Message> messagePtr) = 0;
    };
}