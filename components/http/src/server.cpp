/**
 * @file server.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <cstdint>

#include "esp_http_server.h"

#include "sys/error.hpp"
#include "http/server.hpp"

namespace http {

server::server(std::uint16_t port /* = 80 */) noexcept {
  config config = HTTPD_DEFAULT_CONFIG();
  config.server_port = port;
  handler_ = initiate(config);
}

server::server(const config& config) noexcept {
  handler_ = initiate(config);
}

sys::error
server::start(const config& config) noexcept {
  auto ret = httpd_start(&handler_, &config);
  if (ret != ESP_OK)
    handler_ = nullptr;
  return ret;
}

sys::error
server::stop() noexcept {
  if (handler_ != nullptr) {
    auto ret = httpd_stop(handler_);
    if (ret == ESP_OK)
      handler_ = nullptr;
    return ret;
  }
  return ESP_OK;
}

sys::error
server::register_uri(const uri& uri) noexcept {
  return httpd_register_uri_handler(handler_, &uri);
}

sys::error
server::unregister_uri(const char* uri) noexcept {
  return httpd_unregister_uri(handler_, uri);
}

sys::error
server::unregister_uri(const char* uri,
                       method method) noexcept {
  return httpd_unregister_uri_handler(handler_, uri, method);
}

sys::error
server::register_uri(error_code error,
                     error_func func) noexcept {
  return httpd_register_err_handler(handler_, error, func);
}

sys::error
server::register_uri(const error& err) noexcept {
  return register_uri(err.code, err.handler);
}

sys::error
server::unregister_uri(httpd_err_code_t error) noexcept {
  return httpd_register_err_handler(handler_, error, NULL);
}

sys::error
server::unregister_uri(const error& err) noexcept {
  return unregister_uri(err.code);
}

[[nodiscard]] server::handler
server::initiate(const config& config) noexcept {
  handler handler = nullptr;
  httpd_start(&handler, &config);
  return handler;
}

}  // namespace http