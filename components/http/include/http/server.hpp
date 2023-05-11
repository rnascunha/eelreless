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
#include <span>

#include "esp_http_server.h"

#include "sys/error.hpp"
#include "sys/event.hpp"

namespace http {

class server {
 public:
  using error_code = httpd_err_code_t;
  using error_func = httpd_err_handler_func_t;

  struct error {
    error_code code;
    error_func handler;
  };

  class request {
   public:
    request(httpd_req_t* req) noexcept;

    request&
    header(const char* field, const char* value) noexcept;
    request&
    content_type(const char*) noexcept;
    request&
    allow_cors(const char* value = "*") noexcept;
    sys::error
    send_error(httpd_err_code_t error, const char *usr_msg = "") noexcept;
    
    [[nodiscard]] void*
    context() noexcept;

    [[nodiscard]] int
    get_socket() const noexcept;
    [[nodiscard]] httpd_req_t*
    native() noexcept;
    [[nodiscard]] httpd_handle_t
    handler() noexcept;

    template<typename T, std::size_t N>
    int receive(std::span<T, N> data) noexcept {
      return httpd_req_recv(req_, data.data(), data.size_bytes());
    }

    template<typename T, std::size_t N>
    sys::error send(std::span<T, N> data) noexcept {
      return httpd_resp_send(req_, (const char*)data.data(), data.size_bytes());
    }
    sys::error send(const char*) noexcept;

   private:
    httpd_req_t* req_;
  };

  using config = httpd_config_t;
  using handler = httpd_handle_t;

  using uri = httpd_uri_t;
  using method = httpd_method_t;

  server() noexcept = default;
  server(std::uint16_t port) noexcept;
  server(const config&) noexcept;

  handler native() const noexcept {
    return handler_;
  }

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

sys::error
register_handler(esp_http_server_event_id_t,
                 esp_event_handler_t,
                 void* = nullptr) noexcept;

sys::error
unregister_handler(esp_http_server_event_id_t,
                   esp_event_handler_t) noexcept;

sys::error
queue(httpd_handle_t,
      httpd_work_fn_t,
      void* = nullptr) noexcept;

sys::error
queue(server::request,
      httpd_work_fn_t,
      void* = nullptr) noexcept;

}  // namespace http

#endif  // COMPONENTS_HTTP_SERVER_HPP_
