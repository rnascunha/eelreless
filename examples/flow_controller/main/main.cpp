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
#include <inttypes.h>

#include "esp_log.h"

#include "sys/sys.hpp"

#include "wifi/common.hpp"
#include "wifi/station.hpp"
#include "wifi/simple_wifi_retry.hpp"
#include "http/server_connect_cb.hpp"
#include "websocket/server.hpp"

#include "resources.cpp"

#include "wifi_args.hpp"

#include "info.hpp"

#if CONFIG_ENABLE_MDNS
#include "mdns.h"
#endif  // CONFIG_ENABLE_MDNS

#if CONFIG_ENABLE_MDNS
void init_mdns() noexcept {
  if (mdns_init()) {
    ESP_LOGW(TAG, "MDNS Init failed");
    return;
  }
  mdns_hostname_set(CONFIG_MDNS_HOSTNAME);
  ESP_LOGI(TAG, "Hostname: " CONFIG_MDNS_HOSTNAME);
}
#endif

#define VALVE_GPIO      ((gpio_num_t)CONFIG_VALVE_GPIO)
#define SENSOR_GPIO     ((gpio_num_t)CONFIG_SENSOR_GPIO)

# define K_SENSOR       CONFIG_FLOW_CONTROL_PULSE_COUNT
# define STEP           CONFIG_FLOW_CONTROL_STEP

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
  wifi::config config = wifi::station::build_config(EXAMPLE_ESP_WIFI_SSID,
                                                    EXAMPLE_ESP_WIFI_PASS)
                          .sae_h2e_identifier(EXAMPLE_H2E_IDENTIFIER)
                          .authmode(ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD)
                          .sae_pwe_h2e(ESP_WIFI_SAE_MODE);

  auto* net_handler = wifi::station::config(config);
  if (net_handler == nullptr) {
    ESP_LOGE(TAG, "Configure WiFi error");
    return;
  }

  control_valve info(VALVE_GPIO,
                     SENSOR_GPIO,
                     K_SENSOR,
                     STEP);

  /**
   * Starting WiFi/Websocket server
   */
  wifi::station::simple_wifi_retry wifi{};
  http::server_connect_cb([&info](auto& server) {
    server.register_uri(
      websocket::uri<ws_cb>{
        .uri            = "/ws",
        .user_ctx       = &info,
        .control_frames = false,
        .supported_subprotocol = nullptr
      }(),
      http::server::error{
        .code      = HTTPD_404_NOT_FOUND,
        .handler   = http_404_error_handler
      }
    );
  });

  if (wifi::start()) {
    ESP_LOGE(TAG, "Error connecting to network");
    return;
  }

  wifi.wait();

  if (wifi.is_connected()) {
    auto ip = wifi::ip(net_handler).ip;
    ESP_LOGI(TAG, "Connected! IP: %" PRIu32 ".%" PRIu32 ".%" PRIu32 ".%" PRIu32, ip.addr & 0xFF,
                                                ip.addr >> 8 & 0xFF,
                                                ip.addr >> 16 & 0xFF,
                                                ip.addr >> 24);
  } else {
    ESP_LOGI(TAG, "Failed");
    return;
  }

#if CONFIG_ENABLE_MDNS
  init_mdns();
#endif

  while (true) {
    info.check();
  }
}
