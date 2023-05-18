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
#include <cstdint>
#include <chrono>
#include <type_traits>
#include <string_view>

#include "lg/log.hpp"
#include "lg/format_types.hpp"

#include "sys/sys.hpp"

#include "wifi/common.hpp"
#include "wifi/station.hpp"
#include "wifi/simple_wifi_retry.hpp"
#include "http/server_connect_cb.hpp"
#include "websocket/server.hpp"

#include "wifi_args.hpp"

static constexpr const
lg::log ll{"WS Server"};

websocket::client ws_client;

static void my_ws_async_send() {
  static const char * data = "My Async data";
  if (ws_client.hd == nullptr)
    return;
  websocket::frame pkt{};
  pkt.payload = (std::uint8_t*)data;
  pkt.len = std::strlen(data);
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
    pkt.payload = (std::uint8_t*)data;
    pkt.len = std::strlen(data);
    pkt.type = HTTPD_WS_TYPE_TEXT;

    client->send(pkt);
    delete client;
}

struct ws_cb {
  static sys::error on_open (websocket::request req) {
    ws_client.hd = req.handler();
    ws_client.fd = req.socket();
    ll.info("Handshake done, the new connection was opened {}", ws_client.fd);
    return ESP_OK;
  }

  static void on_close (int sock, void*) {
    ll.info("Disconnected {}", sock);
    ws_client.hd = nullptr;
  }

  static sys::error on_data(websocket::request req) {
    websocket::data data;
    auto ret = req.receive(data);
    if (ret) {
      ll.info("Failed to receive data {}", ret);
      return ret;
    }

    if (std::string_view((char*)data.packet.payload, data.packet.len) == "Trigger async") {
      ll.info("Trigger async");
      return websocket::queue(req,
                              ws_async_send,
                              (void*)new websocket::client(req));
    }

    ret = req.send(data);
    if (ret)
      ll.error("httpd_ws_send_frame failed with {}", ret);
    return ret;
  }
};

extern "C" void app_main() {
  /**
   * Initiate chip
   */
  if (sys::default_net_init()) {
    ll.error("Error initiating chip");
    return;
  }

  /**
   * WiFi configuration
   */
  wifi::config config = wifi::station::build_config(EXAMPLE_ESP_WIFI_SSID,
                                                    EXAMPLE_ESP_WIFI_PASS)
                          .sae_h2e_identifier(EXAMPLE_H2E_IDENTIFIER)
                          .authmode(ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD)
                          .sae_pwe_h2e(ESP_WIFI_SAE_MODE);

  auto* net_handler = wifi::station::config(config);
  if (net_handler == nullptr) {
    ll.error("Configure WiFi error");
    return;
  }

  /**
   * Starting WiFi/Websocket server
   */
  wifi::station::simple_wifi_retry wifi{};
  http::server_connect_cb([](auto& server) {
    server.register_uri(
      websocket::uri<ws_cb>{
        .uri            = "/ws",
        .user_ctx       = nullptr,
        .control_frames = false,
        .supported_subprotocol = nullptr
      }()
    );
  });

  if (wifi::start()) {
    ll.error("Error connecting to network");
    return;
  }

  wifi.wait();

  if (wifi.is_connected()) {
    ll.info("Connected! IP: {}", wifi::ip(net_handler).ip);
  } else {
    ll.info("Failed");
    return;
  }

  while (true) {
    sys::delay(std::chrono::seconds(10));
    my_ws_async_send();
  }
}
