/**
 * @file resources.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <cstdint>

#include "esp_log.h"

#include "sys/error.hpp"
#include "websocket/server.hpp"

#include "version.hpp"
#include "packtes.hpp"
#include "control_flow.hpp"

static constexpr const char*
TAG = "FLOW";

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err) {
  ESP_LOGW(TAG, "Request fail");
  http::server::request(req).send_error(HTTPD_404_NOT_FOUND, "Not found");
  return ESP_FAIL;
}

template<typename Packet>
sys::error send_packet(websocket::request& req, const Packet& packet) noexcept {
  websocket::frame pkt{};
  pkt.payload = (std::uint8_t*)&packet;
  pkt.len = sizeof(packet);
  pkt.type = HTTPD_WS_TYPE_BINARY;

  return req.send(pkt);
}

sys::error send_error(websocket::request& req, command cmd, error_description desc) noexcept {
  return send_packet(req, error_response{command::error, cmd, desc});
}

sys::error send_state(websocket::request& req) noexcept {
  control_flow& control = *((control_flow*)(req.native()->user_ctx));
  auto count = control.count();
  if (!count)
    return send_error(req, command::state, error_description::error_reading);
  return send_packet(req, state_response(command::state,
                               control.is_open() ? state::open : state::close,
                               *count,
                               control.volume(*count)));
}

sys::error send_config(websocket::request& req) noexcept {
  control_flow& control = *((control_flow*)(req.native()->user_ctx));

  config_response cfg{command::config, version, control.k_ratio()};
  return send_packet(req, cfg);
}

sys::error send_open_valve(websocket::request& req, const websocket::data& data) noexcept {
  if (data.packet.len != sizeof(open_valve_request)) {
    send_error(req, command::open_valve, error_description::size_packet_not_match);
    return ESP_ERR_INVALID_SIZE;
  }

  control_flow& control = *((control_flow*)(req.native()->user_ctx));
  const open_valve_request& r = *((open_valve_request*)data.packet.payload);
  
  if (r.clear_count)
    control.clear_count();
  control.open();

  return send_state(req);
}

sys::error send_close_valve(websocket::request& req, const websocket::data& data) noexcept {
  if (data.packet.len != sizeof(open_valve_request)) {
    send_error(req, command::close_valve, error_description::size_packet_not_match);
    return ESP_ERR_INVALID_SIZE;
  }

  control_flow& control = *((control_flow*)(req.native()->user_ctx));
  const close_valve_request& r = *((close_valve_request*)data.packet.payload);
  
  if (r.clear_count)
    control.clear_count();
  control.close();

  return send_state(req);
}

struct ws_cb {
  static sys::error on_open (websocket::request req) {
    ESP_LOGI(TAG, "Handshake done, the new connection was opened %d", req.socket());

    return send_config(req);
  }

  static void on_close (int sock, void*) {
    ESP_LOGI(TAG, "Disconnected %d", sock);
  }

  static sys::error on_data(websocket::request req) {
    websocket::data data;
    auto ret = req.receive(data);
    if (ret) {
      ESP_LOGI(TAG, "Failed to receive data %d", ret.value());
      return ret;
    }

    auto cmd = (command)data.packet.payload[0];
    switch(cmd) {
      case command::config:
        ESP_LOGI(TAG, "Config packet request");
        send_config(req);
        break;
      case command::state:
        ESP_LOGI(TAG, "State packet request");
        send_state(req);
        break;
      case command::open_valve:
        send_open_valve(req, data);
        break;
      case command::close_valve:
        send_close_valve(req, data);
        break;
      default:
        ESP_LOGI(TAG, "Packet %u request", (std::uint8_t)cmd);
        send_error(req, cmd, error_description::command_not_found);
        break;
    }

    return ESP_OK;
  }
};
