// Copyright (c) 2021 Andrew Paxson. All rights reserved. Used under
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
/* wrapper.hpp */
#pragma once
#include <indiemotion/common.hpp>
#include <indiemotion/responses/base/payload.hpp>
#include <indiemotion/responses/kind.hpp>
#include <indiemotion/transport/header.hpp>
#include <indiemotion/transport/wrapper.hpp>

namespace indiemotion::responses::base
{
    using Response = transport::Wrapper<Payload, Kind>;

    std::unique_ptr<Response> createResponse(std::string inResponseToId,
                                             std::unique_ptr<Payload> payloadPtr)
    {
        auto id = transport::generateNewId();
        auto headerPtr = std::make_unique<transport::Header>(id, inResponseToId);
        auto containerPtr = std::make_unique<Response>(std::move(headerPtr), std::move(payloadPtr));

        return std::move(containerPtr);
    }

    std::unique_ptr<Response> createResponse(std::unique_ptr<Payload> payloadPtr)
    {
        auto id = transport::generateNewId();
        auto headerPtr = std::make_unique<transport::Header>(id);
        auto containerPtr = std::make_unique<Response>(std::move(headerPtr), std::move(payloadPtr));

        return std::move(containerPtr);
    }
}