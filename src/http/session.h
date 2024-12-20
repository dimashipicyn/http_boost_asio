#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <functional>
#include <memory>

namespace http
{
namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

// Handles an HTTP server connection

class SessionBase
{
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;

protected:
    SessionBase(tcp::socket&& socket) : stream_(std::move(socket))
    {
    }

    template <bool isRequest, class Body, class Fields>
    void write(http::message<isRequest, Body, Fields>&& msg)
    {
        // The lifetime of the message has to extend
        // for the duration of the async operation so
        // we use a shared_ptr to manage it.
        auto sp = std::make_shared<http::message<isRequest, Body, Fields>>(std::move(msg));

        // Write the response
        http::async_write(stream_, *sp,
                          [sp, self = get_shared_from_this()](beast::error_code ec, std::size_t bytes_written)
                          { self->on_write(sp->need_eof(), ec, bytes_written); });
    }

    virtual std::shared_ptr<SessionBase> get_shared_from_this() = 0;
    virtual void handle_request(http::request<http::string_body>&& req) = 0;

    // Start the asynchronous operation
private:
    void do_read();
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void on_write(bool close, beast::error_code ec, std::size_t bytes_transferred);
    void do_close();

public:
    void run()
    {
        do_read();
    }
};

template <class RequestHandler>
class Session : public std::enable_shared_from_this<Session<RequestHandler>>, public SessionBase
{
    RequestHandler request_handler_;

    std::shared_ptr<SessionBase> get_shared_from_this() override
    {
        return this->shared_from_this();
    }

    void handle_request(http::request<http::string_body>&& req) override
    {
        request_handler_(std::move(req), [self = this->shared_from_this()](auto&& resp) { self->write(std::move(resp)); });
    }

public:
    // Take ownership of the stream
    template <class Handler>
    Session(tcp::socket&& socket, Handler&& request_handler)
        : SessionBase(std::move(socket)), request_handler_(std::forward<Handler>(request_handler))
    {
    }
};
} // namespace http