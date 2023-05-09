/**
 * @file server.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_WEBSOCKET_SERVER_HPP_
#define COMPONENTS_WEBSOCKET_SERVER_HPP_

#include <type_traits>
#include <memory>
#include <span>

#include "esp_http_server.h"

#include "http/server.hpp"
#include "http/server_connect_cb.hpp"

namespace websocket {

template<typename CallData,
         typename CallOpen = std::nullptr_t>
static esp_err_t ws_handler(httpd_req_t *req) {
  if (req->method == HTTP_GET) {
    if constexpr (!std::is_same_v<CallOpen, std::nullptr_t>)
      return CallOpen{}(req);
    else
      return ESP_OK;
  }
  return CallData{}(req);
}

template<typename CallClose>
void event_handler(void* event_handler_arg,
                    esp_event_base_t event_base,
                    std::int32_t event_id,
                    void* event_data) {
  CallClose{}(*(int*) event_data, event_handler_arg);
}

template<typename OnClose>
sys::error
register_handler(void* user_ctx = nullptr) {
  http::unregister_handler(HTTP_SERVER_EVENT_DISCONNECTED,
                         &event_handler<OnClose>);
  return http::register_handler(HTTP_SERVER_EVENT_DISCONNECTED,
                                &event_handler<OnClose>,
                                user_ctx);
}

struct uri {
  const char* uri = "/";
  void*       user_ctx = nullptr;
  bool        control_frames = false;
  const char* supported_subprotocol = nullptr;

  template<typename OnData,
           typename OnOpen = std::nullptr_t,
           typename OnClose = std::nullptr_t>
  http::server::uri get() noexcept {
    if constexpr (!std::is_same_v<OnClose, std::nullptr_t>) {
      register_handler<OnClose>(user_ctx);
    }
    return http::server::uri{
      .uri          = uri,
      .method       = HTTP_GET,
      .handler      = ws_handler<OnData, OnOpen>,
      .user_ctx     = user_ctx,
      .is_websocket = true,
      .handle_ws_control_frames = control_frames,
      .supported_subprotocol = supported_subprotocol
    };
  }
};

using frame = httpd_ws_frame_t;

struct data {
  frame packet{};
  std::unique_ptr<std::uint8_t[]> buffer = nullptr;
};

class request {
 public:
  request(httpd_req_t* req) noexcept;

  sys::error
  receive(frame& frame,
          std::span<std::uint8_t> buffer) noexcept;
  sys::error
  receive(data& d) noexcept;

  sys::error
  send(frame&) noexcept;
  sys::error
  send(data&) noexcept;

  [[nodiscard]] int
  socket() noexcept;
  [[nodiscard]] httpd_req_t*
  native() noexcept;
  [[nodiscard]] httpd_handle_t
  handler() noexcept;

 private:
  httpd_req_t* req_;
};

struct client {
  client() = default;
  client(httpd_handle_t, int) noexcept;
  client(request&) noexcept;

  sys::error send(frame&) noexcept;
  sys::error send(data&) noexcept;

  httpd_handle_t hd = nullptr;
  int            fd = 0;
};

sys::error
queue(client&, httpd_work_fn_t, void* = nullptr) noexcept;
sys::error
queue(request&, httpd_work_fn_t, void* = nullptr) noexcept;


}  // namespace websocket

#endif  // COMPONENTS_WEBSOCKET_SERVER_HPP_
