// Copyright (c) 2021 Andrew Paxson. All rights reserved. Used under
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
/* test_message_curator.cpp

*/
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <indiemotion/common.hpp>
#include <indiemotion/messages/curator.hpp>
#include <indiemotion/messages/messages.hpp>
#include <indiemotion/transport/header.hpp>

using namespace indiemotion;

SCENARIO("Acknoledging Messages with the Curator")
{

    GIVEN("a curator with a registered message")
    {
        auto callbackCalled = false;
        auto curator = messages::Curator();
        auto id = transport::generateNewId();
        curator.queue(id, [&callbackCalled]()
                      { callbackCalled = true; });

        WHEN("the message is acknowledged")
        {
            curator.acknowledge(id);

            THEN("the callback should have been called")
            {
                REQUIRE(callbackCalled);
            }
        }
    }
}