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

#include "resources.cpp"

#include "wifi_args.hpp"

#include "uc/serial.hpp"

#define UART_BAUDRATE     115200
#define UART_BUFFER_SIZE  512

static constexpr const
lg::log ll{"HTTP WS"};

struct ws_cb {
  static sys::error on_open (websocket::request req) {
    ll.info("Handshake done, the new connection was opened {}", req.socket());
    return ESP_OK;
  }

  static void on_close (int sock, void*) {
    ll.info("Disconnected {}", sock);
  }

  static sys::error on_data(websocket::request req) {
    websocket::data data;
    auto ret = req.receive(data);
    if (ret) {
      ll.info("Failed to receive data {}", ret);
      return ret;
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
      }(),
      http::server::uri{
        .uri       = "/echo",
        .method    = HTTP_POST,
        .handler   = echo_post_handler,
        .user_ctx  = nullptr,
        .is_websocket = false,
        .handle_ws_control_frames = false,
        .supported_subprotocol = nullptr
      }, http::server::uri{
        .uri       = "/hello",
        .method    = HTTP_GET,
        .handler   = hello_get_handler,
        .user_ctx  = (void* )"Hello World!",
        .is_websocket = false,
        .handle_ws_control_frames = false,
        .supported_subprotocol = nullptr
      }, http::server::uri{
        .uri       = "/query",
        .method    = HTTP_GET,
        .handler   = query_get_handler,
        .user_ctx  = nullptr,
        .is_websocket = false,
        .handle_ws_control_frames = false,
        .supported_subprotocol = nullptr
      },
      http::server::error{
        .code      = HTTPD_404_NOT_FOUND,
        .handler   = http_404_error_handler
      }
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

  ll.info("Configuring UART...");

  uc::serial serial0(UART_NUM_0, uc::serial::config{UART_BAUDRATE});
  if (serial0.install_driver(UART_BUFFER_SIZE, UART_BUFFER_SIZE)) {
    ll.error("Driver installation failed");
    return;
  }

  ll.info("Uart Driver installed.");
  std::uint8_t data[UART_BUFFER_SIZE];
  while (true) {
    using namespace std::chrono_literals;
    int len = serial0.read(std::span(data, UART_BUFFER_SIZE-1), 100ms);
    if (len > 0) {
      data[len] = '\0';
      serial0.write(std::span(data, len + 1));
    }
  }
}
