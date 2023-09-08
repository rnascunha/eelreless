/**
 * @file packets.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "esp_log.h"

#include "packets.hpp"

#include "sys/task.hpp"
#include "sys/error.hpp"
#include "websocket/server.hpp"

#include "control_flow.hpp"
#include "info.hpp"

template<typename Packet>
sys::error send_packet(websocket::request& req,
                       const Packet& packet) noexcept {
  websocket::frame pkt{};
  
  pkt.payload = (std::uint8_t*)&packet;
  pkt.len = sizeof(packet);
  pkt.type = HTTPD_WS_TYPE_BINARY;

  return req.send(pkt);
}

template<typename Packet>
sys::error send_packet(websocket::client& client, const Packet& packet) noexcept {
  websocket::frame pkt{};
  
  pkt.payload = (std::uint8_t*)&packet;
  pkt.len = sizeof(packet);
  pkt.type = HTTPD_WS_TYPE_BINARY;

  return client.send(pkt);
}

sys::error send_error(websocket::request& req, command cmd, error_description desc) noexcept {
  return send_packet(req, error_response{command::error, cmd, desc});
}

sys::error send_error(websocket::client& client, command cmd, error_description desc) noexcept {
  if (!client.is_valid()) return sys::error{};
  return send_packet(client, error_response{command::error, cmd, desc});
}

sys::error send_state(websocket::request& req) noexcept {
  control_valve& info = *((control_valve*)(req.native()->user_ctx));

  auto count = info.control.count();
  if (!count) {
    ESP_LOGW("PACKET", "State packet error reading");
    return send_error(info.client, command::state, error_description::error_reading);
  }

  return send_packet(req, state_response(command::state,
                               info.control.is_open() ? state::open : state::close,
                               *count,
                               info.volume,
                               info.limit,
                               info.freq));
}

sys::error send_state(control_valve& info) noexcept {
  if (!info.client.is_valid()) return sys::error{};

  auto count = info.control.count();
  if (!count) {
    ESP_LOGW("PACKET", "State packet error reading");
    return send_error(info.client, command::state, error_description::error_reading);
  }
  return send_packet(info.client, state_response(command::state,
                               info.control.is_open() ? state::open : state::close,
                               *count,
                               info.volume,
                               info.limit,
                               info.freq));
}

sys::error send_config(websocket::request& req) noexcept {
  control_valve& info = *((control_valve*)(req.native()->user_ctx));

  config_response cfg{command::config,
                      version,
                      info.control.k_ratio(),
                      info.control.step()};
  return send_packet(req, cfg);
}

sys::error
send_open_valve(websocket::request& req,
                const websocket::data& data) noexcept {
  if (data.packet.len != sizeof(open_valve_request)) {
    ESP_LOGW("PACKET", "Open packet size error [%d/%u]", data.packet.len, sizeof(open_valve_request));
    send_error(req, command::open_valve, error_description::size_packet_not_match);
    return ESP_ERR_INVALID_SIZE;
  }

  control_valve& info = *((control_valve*)(req.native()->user_ctx));
  const open_valve_request& r = *((open_valve_request*)data.packet.payload);
  
  info.start(req, r.freq, r.limit, r.clear_count);
  info.events.set(control_valve::bit_step);
  
  return send_state(req);
}

sys::error
send_close_valve(websocket::request& req,
                 const websocket::data& data) noexcept {
  if (data.packet.len != sizeof(close_valve_request)) {
    ESP_LOGW("PACKET", "Close packet size error [%d/%u]", data.packet.len, sizeof(close_valve_request));
    send_error(req, command::close_valve, error_description::size_packet_not_match);
    return ESP_ERR_INVALID_SIZE;
  }

  control_valve& info = *((control_valve*)(req.native()->user_ctx));
  const close_valve_request& r = *((close_valve_request*)data.packet.payload);
  
  info.close(r.clear_count);

  return send_state(req);
}
