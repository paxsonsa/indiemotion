#include <indiemotion/message.hpp>
#include <indiemotion/message_gen.hpp>
#include <indiemotion/controllers/session.hpp>

namespace indiemotion
{
    void SessionController::initialize() {
        if (auto ctx = _ctx.lock())
        {
            auto uid = generate_new_identifier_string();
            ctx->update("session/id", uid);
            ctx->update("session/client_name", "");
            ctx->update("session/server_name", "");
        }
    }

    void SessionController::set_context(std::shared_ptr<Context> ctx) {
        _ctx = ctx;
    }

    void SessionController::run(const Message& msg)
    {
        auto ctx = _ctx.lock();
        if (!ctx)
            return;

        if (msg.payload_case() == Message::PayloadCase::kClientInfo)
        {
            ctx->update("session/client_name", msg.client_info().name());
        }
    }

}