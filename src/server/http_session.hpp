#pragma once
#include <memory>
#include <beast.hpp>
#include <net.hpp>

#include <indiemotion/callbacks.hpp>
#include <indiemotion/runtime.hpp>

namespace indiemotion::internal {
	class HttpSession: public std::enable_shared_from_this<HttpSession> {
		beast::tcp_stream _stream;
		beast::flat_buffer _buffer;
		std::string const _doc_root;
                std::shared_ptr<GlobalCallbacks> _callbacks;
                std::shared_ptr<Runtime> _runtime;

		// The parser is stored in an optional container so we can
		// construct it from scratch it at the beginning of each new message.
		boost::optional<http::request_parser<http::string_body>> _parser;

		struct _send_lambda;

		void _do_read();
		void _on_read(beast::error_code ec, std::size_t);
		void _on_write(beast::error_code ec, std::size_t, bool close);
		void _fail(beast::error_code ec, char const* what);

	public:
		HttpSession(tcp::socket&& socket,
                            std::string root,
                            std::shared_ptr<Runtime> runtime,
                            std::shared_ptr<GlobalCallbacks> callbacks);

		void run();

		std::string const&
		doc_root() const noexcept
		{
			return _doc_root;
		}
	};
}