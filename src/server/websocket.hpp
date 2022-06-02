#pragma once
#include <memory>
#include <string>
#include <vector>
#include <beast.hpp>
#include <net.hpp>


#include <indiemotion/callbacks.hpp>
#include <indiemotion/messaging.hpp>



namespace indiemotion::internal
{
	class Websocket : public MessageSender, public std::enable_shared_from_this<Websocket>
	{
		beast::flat_buffer _buffer;
		websocket::stream<beast::tcp_stream> _stream;
		std::vector<std::shared_ptr<std::string const>> _queue;
                std::shared_ptr<GlobalCallbacks> _callbacks;
                boost::uuids::uuid _tag;

		void _fail(beast::error_code ec, char const* what);
		void _on_accept(beast::error_code ec);
		void _on_read(beast::error_code ec, std::size_t bytes_transferred);
		void _on_write(beast::error_code ec, std::size_t bytes_transferred);
		void _on_send(std::shared_ptr<std::string const> const& ss);

	public:
		explicit Websocket(tcp::socket&& socket, std::shared_ptr<GlobalCallbacks> callbacks);

		~Websocket();

		template<class Body, class Allocator>
		void run(http::request<Body, http::basic_fields<Allocator>> req);

		// Send a message
                void send(std::shared_ptr<std::string const> const& ss) override;

                // Return the unique tag for this Websocket session
                uuid tag() const override{ return _tag; }
	};

	template<class Body, class Allocator>
	void Websocket::run(http::request<Body, http::basic_fields<Allocator>> req)
	{
		// Set suggested timeout settings for the websocket
		_stream.set_option(
			websocket::stream_base::timeout::suggested(
				beast::role_type::server));

		// Set a decorator to change the Server of the handshake
		_stream.set_option(websocket::stream_base::decorator(
			[](websocket::response_type& res)
			{
				res.set(http::field::server,
					std::string(BOOST_BEAST_VERSION_STRING) +
						" websocket-chat-multi");
			}));

		// Accept the websocket handshake
		_stream.async_accept(
			req,
			beast::bind_front_handler(
				&Websocket::_on_accept,
				shared_from_this()));
	}

}
