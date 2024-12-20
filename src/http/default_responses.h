#pragma once

#include <boost/beast/http.hpp>

namespace http
{
namespace beast = boost::beast;

// Запрос, тело которого представлено в виде строки
using StringRequest = beast::http::request<beast::http::string_body>;
// Ответ, тело которого представлено в виде строки
using StringResponse = beast::http::response<beast::http::string_body>;

StringResponse bad_request(const StringRequest& req, beast::string_view why);
StringResponse not_found(const StringRequest& req, beast::string_view target);
StringResponse server_error(const StringRequest& req, beast::string_view what);
}