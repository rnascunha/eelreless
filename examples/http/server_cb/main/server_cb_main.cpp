/**
 * @file server_cb_main.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <chrono>

#include "lg/log.hpp"
#include "lg/format_types.hpp"

#include "sys/error.hpp"
#include "sys/sys.hpp"
#include "sys/time.hpp"

#include "wifi/common.hpp"
#include "wifi/station.hpp"
#include "wifi/simple_wifi_retry.hpp"

#include "http/server_connect_cb.hpp"

#include "resources.cpp"

#include "wifi_args.hpp"

static constexpr const
lg::log ll{"HTTP Server"};

extern "C" void app_main() {
  auto err = sys::default_net_init();
  if (err) {
    ll.error("Erro initializing chip [{:b}]", err);
    return;
  }

  /**
   * WiFi configuration/connection
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

  wifi::station::simple_wifi_retry retry{EXAMPLE_ESP_MAXIMUM_RETRY};
  http::server_connect_cb http_server{[](http::server& server) {
      server.register_uri(http::server::uri{
        .uri       = "/echo",
        .method    = HTTP_POST,
        .handler   = echo_post_handler,
        .user_ctx  = nullptr
      }, http::server::uri{
        .uri       = "/hello",
        .method    = HTTP_GET,
        .handler   = hello_get_handler,
        .user_ctx  = (void* )"Hello World!"
      }, http::server::error{
        .code      = HTTPD_404_NOT_FOUND,
        .handler   = http_404_error_handler
      });
    }
  };
  http_server.config.server_port = 80;
  
  err = wifi::start();
  if (err) {
    ll.error("Connect WiFi error [{:b}]", err);
    return;
  }

  ll.info("WiFi connecting to SSID:{} password:{}",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

  retry.wait();

  if (retry.is_connected()) {
    ll.info("Connected! IP:{}", wifi::ip(net_handler).ip);
  } else {
    ll.warn("Failed");
    return;
  }

  while (true) {
    using namespace std::chrono_literals;
    sys::delay(5s);
  }
}
