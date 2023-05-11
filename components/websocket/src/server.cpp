#include "sdkconfig.h"

#ifdef CONFIG_HTTPD_WS_SUPPORT

#include <cstring>
#include <span>

#include "esp_http_server.h"

#include "sys/error.hpp"

#include "websocket/server.hpp"

namespace websocket {

request::request(httpd_req_t* req) noexcept
 : req_(req) {}

sys::error
request::receive(frame& frame,
                 std::span<std::uint8_t> buffer) noexcept {
  std::memset(&frame, 0, sizeof(frame));
  frame.payload = buffer.data();
  sys::error ret = httpd_ws_recv_frame(req_, &frame, buffer.size());
  if (ret)
    return ret;
  return ESP_OK;
}

sys::error
request::receive(data& d) noexcept {
  std::memset(&d.packet, 0, sizeof(d.packet));
  d.packet.type = HTTPD_WS_TYPE_TEXT;
  sys::error ret = httpd_ws_recv_frame(req_, &d.packet, 0);
  if (ret)
    return ret;

  if (d.packet.len) {
     d.buffer = std::make_unique<std::uint8_t[]>(d.packet.len);
    if (d.buffer == nullptr)
      return ESP_ERR_NO_MEM;
  }
  d.packet.payload = &d.buffer[0];
  return httpd_ws_recv_frame(req_, &d.packet, d.packet.len);
}

sys::error
request::send(frame& frame) noexcept {
  return httpd_ws_send_frame(req_, &frame);
}

sys::error
request::send(data& dt) noexcept {
  return send(dt.packet);
}

[[nodiscard]] int
request::socket() noexcept {
  return httpd_req_to_sockfd(req_);
}

[[nodiscard]] httpd_req_t*
request::native() noexcept {
  return req_;
}

[[nodiscard]] httpd_handle_t
request::handler() noexcept {
  return req_->handle;
}

client::client(httpd_handle_t hd, int fd) noexcept
 : hd(hd), fd(fd) {}

client::client(request& req) noexcept 
 : hd(req.handler()), fd(req.socket()) {} 

sys::error
client::send(frame& packet) noexcept {
  return httpd_ws_send_frame_async(hd, fd, &packet);
}

sys::error
client::send(data& data) noexcept {
  return send(data.packet);
}

sys::error
queue(client& client, httpd_work_fn_t func, void*  arg /* = nullptr */) noexcept {
  return http::queue(client.hd, func, arg);
}

sys::error
queue(request& req, httpd_work_fn_t func, void* arg /* = nullptr */) noexcept {
  return http::queue(req.handler(), func, arg);
}

}  // namespace websocket

#endif  // CONFIG_HTTPD_WS_SUPPORT