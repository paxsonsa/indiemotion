#pragma once
#include <memory>

#include <beast.hpp>
#include <net.hpp>

namespace indiemotion::internal {
	class _ServerRuntime: public std::enable_shared_from_this<_ServerRuntime> {
	private:
		net::io_context _io_ctx;
		tcp::acceptor _acceptor;
		std::string _root;
	public:
		_ServerRuntime(): _acceptor(_io_ctx) {}
		~_ServerRuntime() = default;
//		_ServerRuntime(const _ServerRuntime&) =delete;
		_ServerRuntime& operator=(const _ServerRuntime&) =delete;

		static std::shared_ptr<_ServerRuntime> create()
		{
			return std::make_shared<_ServerRuntime>();
		}

		void set_endpoint(const tcp::endpoint&);
		void set_reuse_addr(bool);
		void set_root_path(std::string);
		void start();
		void wait_until_exit();
	private:
		void do_accept();
		void on_accept(beast::error_code ec, tcp::socket socket);
	};
}