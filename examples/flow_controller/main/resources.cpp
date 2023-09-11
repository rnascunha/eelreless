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
#include "esp_http_server.h"

#include "sys/error.hpp"
#include "websocket/server.hpp"

#include "version.hpp"
#include "packets.hpp"
#include "info.hpp"

static constexpr const char*
TAG = "FLOW";

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err) {
  ESP_LOGW(TAG, "Request fail");
  http::server::request(req).send_error(HTTPD_404_NOT_FOUND, "Not found");
  return ESP_FAIL;
}

struct ws_cb {
  static sys::error on_open (websocket::request req) {
    ESP_LOGI(TAG, "Handshake done, the new connection was opened %d", req.socket());

    send_config(req);
    send_state(req);
    return sys::error{};
  }

  static void on_close (int sock, void* data) {
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
        send_config(req, data);
        break;
      case command::state:
        ESP_LOGI(TAG, "State packet request");
        send_state(req);
        break;
      case command::open_valve:
        ESP_LOGI(TAG, "Open valve packet request");
        send_open_valve(req, data);
        break;
      case command::close_valve:
        ESP_LOGI(TAG, "Close valve packet request");
        send_close_valve(req);
        break;
      default:
        ESP_LOGI(TAG, "Packet %u request", (std::uint8_t)cmd);
        send_error(req, cmd, error_description::command_not_found);
        break;
    }
    return ESP_OK;
  }
};
