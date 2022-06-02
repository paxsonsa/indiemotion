#pragma once
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


namespace net = boost::asio;                    // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
using uuid = boost::uuids::uuid;                // from <boost/uuid/uuid.hpp>
namespace uuids = boost::uuids;
using uuid_generator = boost::uuids::random_generator;                // from <boost/uuid/uuid.hpp>
