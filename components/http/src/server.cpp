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
#include <cstring>

#include "esp_http_server.h"
#if CONFIG_ESP_HTTPS_SERVER_ENABLE == 1
#include "esp_https_server.h"
#endif  // CONFIG_ESP_HTTPS_SERVER_ENABLE == 1

#include "sys/error.hpp"
#include "http/server.hpp"

namespace http {

server::server(std::uint16_t port) noexcept {
  config config = HTTPD_DEFAULT_CONFIG();
  config.server_port = port;
  handler_ = initiate(config);
}

server::server(const config& config) noexcept {
  handler_ = initiate(config);
}

#if CONFIG_ESP_HTTPS_SERVER_ENABLE == 1
server::server(ssl_config& cfg) noexcept {
  handler_ = initiate(cfg);
}
#endif  // CONFIG_ESP_HTTPS_SERVER_ENABLE == 1

sys::error
server::start(const config& config) noexcept {
  auto ret = httpd_start(&handler_, &config);
  if (ret != ESP_OK)
    handler_ = nullptr;
  return ret;
}

#if CONFIG_ESP_HTTPS_SERVER_ENABLE == 1
sys::error
server::start(ssl_config& cfg) noexcept {
  auto ret = httpd_ssl_start(&handler_, &cfg);
  if (ret != ESP_OK)
    handler_ = nullptr;
  return ret;
}
#endif  // CONFIG_ESP_HTTPS_SERVER_ENABLE == 1

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

#if CONFIG_ESP_HTTPS_SERVER_ENABLE == 1
[[nodiscard]] server::handler
server::initiate(ssl_config& cfg) noexcept {
  handler handler = nullptr;
  httpd_ssl_start(&handler, &cfg);
  return handler;
}
#endif  // CONFIG_ESP_HTTPS_SERVER_ENABLE == 1

server::request::request(httpd_req_t* req) noexcept
: req_(req){}

server::request&
server::request::header(const char* field, const char* value) noexcept {
  httpd_resp_set_hdr(req_, field, value);
  return *this;
}

server::request&
server::request::content_type(const char* type) noexcept {
  httpd_resp_set_type(req_, type);
  return *this;
}

server::request&
server::request::allow_cors(const char* value /* = "*" */) noexcept {
  return header("Access-Control-Allow-Origin", value);
}

sys::error
server::request::send_error(httpd_err_code_t error,
                            const char *usr_msg /* = "" */) noexcept {
  return httpd_resp_send_err(req_, error, usr_msg);
}

sys::error
server::request::send(const char* data) noexcept {
  return httpd_send(req_, data, std::strlen(data));
}

[[nodiscard]] void*
server::request::context() noexcept {
  return req_->user_ctx;
}

[[nodiscard]] int
server::request::get_socket() const noexcept {
  return httpd_req_to_sockfd(req_);
}

[[nodiscard]] httpd_req_t*
server::request::native() noexcept {
  return req_;
}

[[nodiscard]] httpd_handle_t
server::request::handler() noexcept {
  return req_->handle;
}

sys::error
register_handler(esp_http_server_event_id_t id,
                 esp_event_handler_t handler,
                 void* arg /* = nullptr */) noexcept {
  return sys::event::register_handler(ESP_HTTP_SERVER_EVENT,
                                      id,
                                      handler,
                                      arg);
}

sys::error
unregister_handler(esp_http_server_event_id_t id,
                 esp_event_handler_t handler) noexcept {
  return sys::event::register_handler(ESP_HTTP_SERVER_EVENT,
                                      id,
                                      handler);
}

sys::error
queue(httpd_handle_t handler,
      httpd_work_fn_t func,
      void* arg /* = nullptr */) noexcept {
  return httpd_queue_work(handler, func, arg);
}

sys::error
queue(server::request req,
      httpd_work_fn_t func,
      void* arg /* = nullptr */) noexcept {
  return queue(req.handler(), func, arg);
}

}  // namespace http