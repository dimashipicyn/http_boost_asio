#pragma once

#include "default_responses.h"
#include "http_utils.h"

#include <boost/beast/version.hpp>
#include <boost/beast/core/string_type.hpp>
#include <boost/beast/http.hpp>

namespace http
{
namespace beast = boost::beast;         // from <boost/beast.hpp>

class RequestHandler
{
public:
    beast::string_view doc_root;

    template <class Body, class Allocator, class Send>
    void operator()(beast::http::request<Body, beast::http::basic_fields<Allocator>>&& req, Send&& send)
    {
        // Make sure we can handle the method
        if (req.method() != beast::http::verb::get && req.method() != beast::http::verb::head)
            return send(bad_request(req, "Unknown HTTP-method"));

        // Request path must be absolute and not contain "..".
        if (req.target().empty() || req.target()[0] != '/' || req.target().find("..") != beast::string_view::npos)
            return send(bad_request(req, "Illegal request-target"));

        // Build the path to the requested file
        std::string path = path_cat(doc_root, req.target());
        if (req.target().back() == '/')
            path.append("index.html");

        // Attempt to open the file
        beast::error_code ec;
        beast::http::file_body::value_type body;
        body.open(path.c_str(), beast::file_mode::scan, ec);

        // Handle the case where the file doesn't exist
        if (ec == beast::errc::no_such_file_or_directory)
            return send(not_found(req, req.target()));

        // Handle an unknown error
        if (ec)
            return send(server_error(req, ec.message()));

        // Cache the size since we need it after the move
        auto const size = body.size();

        // Respond to HEAD request
        if (req.method() == beast::http::verb::head)
        {
            beast::http::response<beast::http::empty_body> res{beast::http::status::ok, req.version()};
            res.set(beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(beast::http::field::content_type, mime_type(path));
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return send(std::move(res));
        }

        // Respond to GET request
        beast::http::response<beast::http::file_body> res{std::piecewise_construct, std::make_tuple(std::move(body)),
                                                          std::make_tuple(beast::http::status::ok, req.version())};
        res.set(beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(beast::http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return send(std::move(res));
    }
};
} // namespace http