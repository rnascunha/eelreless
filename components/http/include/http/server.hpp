/**
 * @file http.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_HTTP_SERVER_HPP_
#define COMPONENTS_HTTP_SERVER_HPP_

#include "esp_http_server.h"

namespace http {

struct http_error {
  httpd_err_code_t code;
  httpd_err_handler_func_t handler;
};

class server {
 public:
  server() noexcept = default;
  server(uint16_t port) noexcept;
  server(const httpd_config_t&) noexcept;

  ~server() noexcept {
    stop();
  }

  esp_err_t start(const httpd_config_t&) noexcept;
  esp_err_t stop() noexcept;

  esp_err_t
  register_uri(const httpd_uri_t&) noexcept;
  esp_err_t
  unregister_uri(const char* uri) noexcept;
  esp_err_t
  unregister_uri(const char* uri,
                 httpd_method_t method) noexcept;

  esp_err_t
  register_err(httpd_err_code_t, httpd_err_handler_func_t) noexcept;
  esp_err_t
  register_err(const http_error& err) noexcept;
  esp_err_t
  unregister_err(httpd_err_code_t) noexcept;

  bool
  is_connected() const noexcept {
    return handler_ != nullptr;
  }

  static httpd_handle_t
  initiate(const httpd_config_t&) noexcept;
 private:
  httpd_handle_t handler_ = nullptr;
};

}  // namespace http

#endif  // COMPONENTS_HTTP_SERVER_HPP_
