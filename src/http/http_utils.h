#pragma once

#include <boost/beast.hpp>
#include <string>

namespace http
{
namespace beast = boost::beast; // from <boost/beast.hpp>

// Return a reasonable mime type based on the extension of a file.
beast::string_view mime_type(beast::string_view path);

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string path_cat(beast::string_view base, beast::string_view path);
} // namespace http_utils