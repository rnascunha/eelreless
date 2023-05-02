/**
 * @file helper.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_HTTP_HELPER_HPP_
#define COMPONENTS_HTTP_HELPER_HPP_

#include <variant>
#include <array>

#include "esp_http_server.h"

#include "helper.hpp"
#include "http/server.hpp"

namespace http {

using http_uri_err = std::variant<httpd_uri_t, http_error>;

template<unsigned int N>
void
register_uris(server& handler, const std::array<http_uri_err, N>& uris) noexcept {
  for (const auto& uri : uris) {
    std::visit(helper::overloaded{
      [&handler](const httpd_uri_t& uri) { handler.register_uri(uri); },
      [&handler](const http_error& err) { handler.register_err(err.code, err.handler); }
    }, uri);
  }
}

}  // namespace

#endif  // COMPONENTS_HTTP_HELPER_HPP_
