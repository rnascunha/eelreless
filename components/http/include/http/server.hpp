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

#include <cstdint>
#include <type_traits>
#include <functional>

#include "esp_http_server.h"

#include "sys/error.hpp"

namespace http {

class server {
 public:
  using error_code = httpd_err_code_t;
  using error_func = httpd_err_handler_func_t;

  struct error {
    error_code code;
    error_func handler;
  };

  using config = httpd_config_t;
  using handler = httpd_handle_t;

  using uri = httpd_uri_t;
  using method = httpd_method_t;

  server() noexcept = default;
  server(std::uint16_t port) noexcept;
  server(const config&) noexcept;

  ~server() noexcept {
    stop();
  }

  sys::error start(const config&) noexcept;
  sys::error stop() noexcept;

  sys::error
  register_uri(const uri&) noexcept;
  sys::error
  register_uri(error_code, error_func) noexcept;
  sys::error
  register_uri(const error& err) noexcept;

  template<typename ...Uris>
  std::enable_if_t<std::greater{}(sizeof...(Uris), 1)> 
  register_uri(Uris&&... uris) noexcept {
    (register_uri(uris), ...);
  }

  sys::error
  unregister_uri(const char* uri) noexcept;
  sys::error
  unregister_uri(const char* uri,
                  method method) noexcept;
  sys::error
  unregister_uri(error_code) noexcept;
  sys::error
  unregister_uri(const error&) noexcept;

  [[nodiscard]] bool
  is_connected() const noexcept {
    return handler_ != nullptr;
  }

  [[nodiscard]] static handler
  initiate(const config&) noexcept;
 private:
  handler handler_ = nullptr;
};

}  // namespace http

#endif  // COMPONENTS_HTTP_SERVER_HPP_
