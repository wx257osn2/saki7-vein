﻿#ifndef VEIN_LISTENER_HPP
#define VEIN_LISTENER_HPP

#include "vein/Error.hpp"
#include "vein/HTTPSession.hpp"

#include <boost/beast/core/error.hpp>
#include <boost/beast/core/bind_handler.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

#include <memory>


namespace vein {

namespace beast = boost::beast;
namespace net = beast::net;

using tcp = boost::asio::ip::tcp;

class Router;

// Accepts incoming connections and launches the sessions
class Listener : public std::enable_shared_from_this<Listener>
{
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    std::shared_ptr<std::string const> doc_root_;

public:
    Listener(net::io_context& ioc, tcp::endpoint endpoint, std::unique_ptr<Router> router)
        : ioc_(ioc)
        , acceptor_(net::make_strand(ioc))
        , router_(std::move(router))
    {
        beast::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if (ec) {
            fail(ec, "open");
            return;
        }

        // Allow address reuse
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if (ec) {
            fail(ec, "set_option");
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if (ec) {
            fail(ec, "bind");
            return;
        }

        // Start listening for connections
        acceptor_.listen(
            net::socket_base::max_listen_connections, ec);
        if (ec) {
            fail(ec, "listen");
            return;
        }
    }

    ~Listener();

    // Start accepting incoming connections
    void
        run()
    {
        // We need to be executing within a strand to perform async operations
        // on the I/O objects in this session. Although not strictly necessary
        // for single-threaded contexts, this example code is written to be
        // thread-safe by default.
        net::dispatch(
            acceptor_.get_executor(),
            beast::bind_front_handler(
                &Listener::do_accept,
                this->shared_from_this()));
    }

private:
    void
        do_accept()
    {
        // The new connection gets its own strand
        acceptor_.async_accept(
            net::make_strand(ioc_),
            beast::bind_front_handler(
                &Listener::on_accept,
                shared_from_this()));
    }

    void
        on_accept(beast::error_code ec, tcp::socket socket);

    std::unique_ptr<Router> router_;
};

}

#endif
