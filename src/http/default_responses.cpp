#include "default_responses.h"

#include <boost/beast/version.hpp>

using namespace http;

StringResponse http::bad_request(const StringRequest& req,beast::string_view why)
{
    beast::http::response<beast::http::string_body> res{beast::http::status::bad_request, req.version()};
    res.set(beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(beast::http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
}

StringResponse http::not_found(const StringRequest& req, beast::string_view target)
{
    beast::http::response<beast::http::string_body> res{beast::http::status::not_found, req.version()};
    res.set(beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(beast::http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "The resource '" + std::string(target) + "' was not found.";
    res.prepare_payload();
    return res;
}

StringResponse http::server_error(const StringRequest& req,beast::string_view what)
{
    beast::http::response<beast::http::string_body> res{beast::http::status::internal_server_error, req.version()};
    res.set(beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(beast::http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "An error occurred: '" + std::string(what) + "'";
    res.prepare_payload();
    return res;
}
