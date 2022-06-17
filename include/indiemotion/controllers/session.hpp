#pragma once
#include <indiemotion/controllers/controller.hpp>

namespace indiemotion
{
    class SessionController: public Controller
    {
        std::weak_ptr<Context> _ctx;
        std::array<std::optional<std::function<void(const Message&)>>, 128> _m_callback_table{};

      public:
        void initialize() override;
        void run(const Message &msg) override;
        void set_context(std::shared_ptr<Context> ctx) override;

      private:
        void _process_client_info(const Message& msg);
    };
}