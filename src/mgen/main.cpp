//
// Created by Andrew Paxson on 2022-06-05.
//
#include <fmt/core.h>
#include <google/protobuf/util/json_util.h>
#include <indiemotion/messaging.hpp>



int main(int argc, char** argv)
{
    indiemotion::Message m;
    auto id = indiemotion::msg_gen_identifier_string();
    m.mutable_header()->set_id(id);

    std::string msg_str;
    google::protobuf::util::MessageToJsonString(m, &msg_str);
    fmt::print("{}\n", msg_str);
    return 0;
}