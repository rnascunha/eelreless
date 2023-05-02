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
#include "esp_http_server.h"

#include "http/server.hpp"

namespace http {

server::server(uint16_t port /* = 80 */) noexcept {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = port;
  handler_ = initiate(config);
}

server::server(const httpd_config_t& config) noexcept {
  handler_ = initiate(config);
}

esp_err_t
server::start(const httpd_config_t& config) noexcept {
  auto ret = httpd_start(&handler_, &config);
  if (ret != ESP_OK)
    handler_ = nullptr;
  return ret;
}

esp_err_t
server::stop() noexcept {
  if (handler_ != nullptr) {
    auto ret = httpd_stop(handler_);
    if (ret == ESP_OK)
      handler_ = nullptr;
    return ret;
  }
  return ESP_OK;
}

esp_err_t
server::register_uri(const httpd_uri_t& uri) noexcept {
  return httpd_register_uri_handler(handler_, &uri);
}

esp_err_t
server::unregister_uri(const char* uri) noexcept {
  return httpd_unregister_uri(handler_, uri);
}

esp_err_t
server::unregister_uri(const char* uri,
               httpd_method_t method) noexcept {
  return httpd_unregister_uri_handler(handler_, uri, method);
}

esp_err_t
server::register_err(httpd_err_code_t error,
                     httpd_err_handler_func_t func) noexcept {
  return httpd_register_err_handler(handler_, error, func);
}

esp_err_t
server::register_err(const http_error& err) noexcept {
  return register_err(err.code, err.handler);
}

esp_err_t
server::unregister_err(httpd_err_code_t error) noexcept {
  return httpd_register_err_handler(handler_, error, NULL);
}

httpd_handle_t
server::initiate(const httpd_config_t& config) noexcept {
  httpd_handle_t handler = nullptr;
  httpd_start(&handler, &config);
  return handler;
}

}  // namespace http