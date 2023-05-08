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

server::request::request(httpd_req_t* req) noexcept
: req_(req){}

server::request&
server::request::header(const char* field, const char* value) noexcept {
  httpd_resp_set_hdr(req_, field, value);
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