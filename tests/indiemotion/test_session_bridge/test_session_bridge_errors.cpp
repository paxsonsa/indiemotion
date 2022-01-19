#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <indiemotion/common.hpp>
#include <indiemotion/net/dispatch.hpp>
#include <indiemotion/session.hpp>
#include <indiemotion/net/message.hpp>

using namespace indiemotion;

struct DummyDispatcher : NetMessageDispatcher
{
	std::vector<Message> messages{};

	void dispatch(Message&& message)
	{
		messages.push_back(std::move(message));
	}
};

struct DummyDelegate : Application
{
};

SCENARIO("Send a message without a payload case")
{
	GIVEN("a new controller object")
	{
		auto delegate = std::make_shared<DummyDelegate>();
		auto session = std::make_shared<SessionCon>(delegate);
		auto dispatcher = std::make_shared<DummyDispatcher>();
		auto bridge = SessionService(dispatcher, session);

		Message message;

		WHEN("message without a payload is processed")
		{
			bridge.process_message(std::move(message));

			THEN("A 'bad message' error message should be returned should be returned")
			{
				REQUIRE(dispatcher->messages.size() == 1);
				auto response = dispatcher->messages[0];
				REQUIRE(response.has_error());
				auto error = response.error();
				REQUIRE(error.type() == message_payloads::Error::BadMessageError);
			}
		}
	}
}
