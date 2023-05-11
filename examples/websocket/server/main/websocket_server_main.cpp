/**
 * @file main.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <cstring>
#include <chrono>
#include <type_traits>
#include <inttypes.h>
#include <string_view>

#include "esp_log.h"

#include "sys/sys.hpp"

#include "wifi/common.hpp"
#include "wifi/station.hpp"
#include "wifi/simple_wifi_retry.hpp"
#include "http/server_connect_cb.hpp"
#include "websocket/server.hpp"

#include "wifi_args.hpp"

static constexpr const
char *TAG = "WS Server";

websocket::client ws_client;

static void my_ws_async_send() {
  static const char * data = "My Async data";
  if (ws_client.hd == nullptr)
    return;
  websocket::frame pkt{};
  pkt.payload = (std::uint8_t*)data;
  pkt.len = strlen(data);
  pkt.type = HTTPD_WS_TYPE_TEXT;

  ws_client.send(pkt);
}

/*
 * async send function, which we put into the httpd work queue
 */
static void ws_async_send(void *arg) {
    static const char * data = "Async data";
    auto *client = reinterpret_cast<websocket::client*>(arg);

    websocket::frame pkt{};
    pkt.payload = (uint8_t*)data;
    pkt.len = std::strlen(data);
    pkt.type = HTTPD_WS_TYPE_TEXT;

    client->send(pkt);
    delete client;
}

struct ws_open {
  esp_err_t operator()(websocket::request req) {
    ws_client.hd = req.handler();
    ws_client.fd = req.socket();
    ESP_LOGI(TAG, "Handshake done, the new connection was opened %d", ws_client.fd);
    return ESP_OK;
  }
};

struct ws_close {
  void operator()(int sock, void*) {
    ESP_LOGI(TAG, "Disconnected %d", sock);
    ws_client.hd = nullptr;
  }
};

struct ws_data {
  esp_err_t operator()(websocket::request req) {
    websocket::data data;
    auto ret = req.receive(data);
    if (ret) {
      ESP_LOGI(TAG, "Failed to receive data %d/%s", ret.value(), ret.message());
      return ret;
    }

    if (std::string_view((char*)data.packet.payload, data.packet.len) == "Trigger async") {
      ESP_LOGI(TAG, "Trigger async");
      return websocket::queue(req,
                              ws_async_send,
                              (void*)new websocket::client(req));
    }

    ret = req.send(data);
    if (ret)
      ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret.value());
    return ret;
  }
};

extern "C" void app_main() {
  /**
   * Initiate chip
   */
  if (sys::default_net_init()) {
    ESP_LOGE(TAG, "Error initiating chip");
    return;
  }

  /**
   * WiFi configuration
   */
  wifi::config config = {};
  std::strcpy((char*)config.sta.ssid, EXAMPLE_ESP_WIFI_SSID);
  std::strcpy((char*)config.sta.password, EXAMPLE_ESP_WIFI_PASS);
  std::strcpy((char*)config.sta.sae_h2e_identifier, EXAMPLE_H2E_IDENTIFIER);
  config.sta.threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD;
  config.sta.sae_pwe_h2e = ESP_WIFI_SAE_MODE;

  auto* net_handler = wifi::station::config(config);
  if (net_handler == nullptr) {
    ESP_LOGE(TAG,  "Configure WiFi error");
    return;
  }

  /**
   * Starting WiFi/Websocket server
   */
  wifi::station::simple_wifi_retry wifi{};
  http::server_connect_cb([](auto& server) {
    server.register_uri(
      websocket::uri{
        .uri            = "/ws",
        .user_ctx       = nullptr,
        .control_frames = false,
        .supported_subprotocol = nullptr
      }.get<ws_data, ws_open, ws_close>()
    );
  });

  if (wifi::start()) {
    ESP_LOGI(TAG, "Error connecting to network");
    return;
  }

  wifi.wait();

  if (wifi.is_connected()) {
    auto ip_info = wifi::ip(net_handler);
    ESP_LOGI(TAG, "Connected! IP:" IPSTR, IP2STR(&ip_info.ip));
  } else {
    ESP_LOGI(TAG, "Failed");
    return;
  }

  while (true) {
    sys::delay(std::chrono::seconds(10));
    my_ws_async_send();
  }
}
