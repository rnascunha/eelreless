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
#include <memory>
#include <optional>

#include "esp_http_server.h"
#if CONFIG_ESP_HTTPS_SERVER_ENABLE == 1
#include "esp_https_server.h"
#endif  // CONFIG_ESP_HTTPS_SERVER_ENABLE == 1

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

    /**
     * Request
     */
    [[nodiscard]] std::size_t
    content_length() noexcept;
    [[nodiscard]] std::size_t
    header_size(const char* field) noexcept;
    std::unique_ptr<char[]>
    header_value(const char* field) noexcept;
    [[nodiscard]] std::size_t
    query_size() noexcept;
    std::unique_ptr<char[]>
    query() noexcept;

    [[nodiscard]] const char*
    uri() const noexcept;

    /**
     * Response
     */
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
    sys::error
    send(std::span<T, N> data) noexcept {
      return httpd_resp_send(req_, (const char*)data.data(), data.size_bytes());
    }
    sys::error send(const char*) noexcept;
    template<typename T, std::size_t N>
    sys::error
    send_chunk(std::span<T, N> data) noexcept {
      return httpd_resp_send_chunk(req_, data.data(), data.size_bytes());
    }
    sys::error
    end_chunk() noexcept;

   private:
    httpd_req_t* req_;
  };

  class query {
   public:
    query(const char* uri) noexcept;
    query(const request& req) noexcept;

    operator bool() const noexcept;

    bool has(const char*) const noexcept;
    std::optional<std::string_view>
    value(const char*) const noexcept;

    const char* operator()() const noexcept;

   private:
    const char* end_key(const char*) const noexcept;
    const char* query_;
  };

  using config = httpd_config_t;

#if CONFIG_ESP_HTTPS_SERVER_ENABLE == 1
  using ssl_config = httpd_ssl_config_t;
#endif  // CONFIG_ESP_HTTPS_SERVER_ENABLE == 1
  using handler = httpd_handle_t;

  using uri = httpd_uri_t;
  using method = httpd_method_t;

  server() noexcept = default;
  server(std::uint16_t port) noexcept;
  server(const config&) noexcept;
#if CONFIG_ESP_HTTPS_SERVER_ENABLE == 1
  server(ssl_config&) noexcept;
#endif  // CONFIG_ESP_HTTPS_SERVER_ENABLE == 1

  handler native() const noexcept {
    return handler_;
  }

  sys::error start(const config&) noexcept;
#if CONFIG_ESP_HTTPS_SERVER_ENABLE == 1
  sys::error start(ssl_config&) noexcept;
#endif  // CONFIG_ESP_HTTPS_SERVER_ENABLE == 1

  template<bool IsSecure = false>
  sys::error stop() noexcept {
    auto ret = ESP_OK;
    if (handler_ == nullptr)
      return ret;
#if CONFIG_ESP_HTTPS_SERVER_ENABLE == 1
    if constexpr (IsSecure)
      ret = httpd_ssl_stop(handler_);
    else
#else
      ret = httpd_stop(handler_);
#endif  // CONFIG_ESP_HTTPS_SERVER_ENABLE == 1
    if (ret == ESP_OK)
      handler_ = nullptr;
    return ret;
  }

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

  sys::error
  client_list(std::size_t& size, int*) noexcept;

#if CONFIG_ESP_HTTPS_SERVER_ENABLE == 1
  [[nodiscard]] static handler
  initiate(ssl_config&) noexcept;
#endif  // CONFIG_ESP_HTTPS_SERVER_ENABLE == 1
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
