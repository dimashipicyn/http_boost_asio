#pragma once

#include "session.h"

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <iostream>
#include <memory>

namespace http
{
namespace net = boost::asio;      // from <boost/asio.hpp>
namespace beast = boost::beast;   // from <boost/beast.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

// Accepts incoming connections and launches the sessions
template <class RequestHandler>
class Listener : public std::enable_shared_from_this<Listener<RequestHandler>>
{
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    RequestHandler req_handler_;

public:
    template<class Handler>
    Listener(net::io_context& ioc, tcp::endpoint endpoint, Handler handler)
        : ioc_(ioc)
        , acceptor_(net::make_strand(ioc))
        , req_handler_(std::move(handler))
    {
        beast::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if (ec)
        {
            fail(ec, "open");
            return;
        }

        // Allow address reuse
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if (ec)
        {
            fail(ec, "set_option");
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if (ec)
        {
            fail(ec, "bind");
            return;
        }

        // Start listening for connections
        acceptor_.listen(net::socket_base::max_listen_connections, ec);
        if (ec)
        {
            fail(ec, "listen");
            return;
        }
    }

    // Start accepting incoming connections
    void run()
    {
        do_accept();
    }

private:
    // Report a failure
    static void fail(beast::error_code ec, char const* what)
    {
        std::cerr << what << ": " << ec.message() << "\n";
    }

    void do_accept()
    {
        // The new connection gets its own strand
        acceptor_.async_accept(net::make_strand(ioc_),
                               beast::bind_front_handler(&Listener::on_accept, this->shared_from_this()));
    }

    void on_accept(beast::error_code ec, tcp::socket socket)
    {
        if (ec)
        {
            fail(ec, "accept");
        }
        else
        {
            // Create the session and run it
            std::make_shared<Session<decltype(req_handler_)>>(std::move(socket), req_handler_)->run();
        }

        // Accept another connection
        do_accept();
    }
};
} // namespace http