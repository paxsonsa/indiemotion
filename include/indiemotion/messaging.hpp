#pragma once
#include <net.hpp>

namespace indiemotion {

    class TaggedObject
    {
      public:
        [[ nodiscard ]]
        virtual uuid tag() const = 0;

        [[ nodiscard ]]
        virtual std::string tag_str() const {
            return uuids::to_string(tag());
        };
    };

    class MessageSender: public TaggedObject {
      public:
        virtual void send(std::shared_ptr<std::string const> const &msg) {}
    };

}