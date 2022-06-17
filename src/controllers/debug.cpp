#include <fmt/core.h>
#include <google/protobuf/util/json_util.h>

#include <indiemotion/controllers/debug.hpp>

namespace indiemotion
{
    void DebugController::run(const Message &msg) {
        std::string json;
        google::protobuf::util::MessageToJsonString(msg, &json);
        fmt::print("-----------------------------------\n", json);
        fmt::print("MSG: {}\n", json);
        for (auto record : *ctx)
        {
            std::cout << record.name << " = " <<
                record.current;

            if (record.next)
                std::cout << " [ changed ]";

            std::cout << std::endl;
        }
        fmt::print("-----------------------------------\n", json);
    }

    void DebugController::set_context(std::shared_ptr<Context> ctx) {
        this->ctx = ctx;
    }
}

