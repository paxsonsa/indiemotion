#pragma once
#include <memory>

#include <beast.hpp>
#include <net.hpp>

namespace indiemotion::internal {
	class _ServerImpl {
		net::io_context _io_ctx;
		tcp::acceptor _acceptor;

	public:
		_ServerImpl(): _acceptor(_io_ctx) {}
		~_ServerImpl() = default;
		_ServerImpl(const _ServerImpl&) =delete;
		_ServerImpl& operator=(const _ServerImpl&) =delete;

		static std::unique_ptr<_ServerImpl> create()
		{
			return std::make_unique<_ServerImpl>();
		}

		void set_endpoint(tcp::endpoint);
		void set_resuse_addr(bool);

	};
}