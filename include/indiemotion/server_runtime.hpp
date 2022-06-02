#pragma once
#include <memory>

#include <beast.hpp>
#include <net.hpp>

#include <indiemotion/callbacks.hpp>

namespace indiemotion {

	class ServerRuntime: public std::enable_shared_from_this<ServerRuntime> {
	private:
		net::io_context _io_ctx;
		tcp::acceptor _acceptor;
		std::string _root;
                std::shared_ptr<GlobalCallbacks> _callbacks;
	public:
                ServerRuntime(): _acceptor(_io_ctx) {}
		~ServerRuntime() = default;
//		ServerRuntime(const _ServerRuntime&) = delete;
                ServerRuntime& operator=(const ServerRuntime&) =delete;

		static std::shared_ptr<ServerRuntime> create()
		{
			return std::make_shared<ServerRuntime>();
		}

		void set_endpoint(const tcp::endpoint&);
		void set_reuse_addr(bool);
		void set_root_path(std::string);
                void set_callbacks(std::shared_ptr<GlobalCallbacks>);
		void start();
		void wait_until_exit();
	private:
		void do_accept();
		void on_accept(beast::error_code ec, tcp::socket socket);
	};
}