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
#include <thread>

#include "esp_log.h"

#include "sys/sys.hpp"

#include "wifi/common.hpp"
#include "wifi/station.hpp"
#include "wifi/simple_wifi_retry.hpp"
#include "http/server_connect_cb.hpp"

#if CONFIG_ENABLE_MDNS
#include "mdns.h"
#endif  // CONFIG_ENABLE_MDNS

#include "adc.hpp"
#include "resources.cpp"

#include "wifi_args.hpp"

static constexpr const
char *TAG = "Eelreless";

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

extern "C" void app_main() {
  /**
   * Initiate chip
   */
  if (sys::default_net_init()) {
    ESP_LOGE(TAG, "Error initiating chip");
    return;
  }

  /**
   * Initiating ADC
   */
  auto adc = initiate_adc();
  if (!adc) {
    ESP_LOGE(TAG, "Error initiating ADC");
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
   * Starting WiFi/HTTP server
   */
  wifi::station::simple_wifi_retry wifi{};
  http::server_connect_cb([&](auto& server) {
    server.register_uri(http::server::uri{
      .uri       = "/current",
      .method    = HTTP_GET,
      .handler   = current_get_handler,
      .user_ctx  = &(*adc)
    }, http::server::error{
      .code      = HTTPD_404_NOT_FOUND,
      .handler   = http_404_error_handler
    });
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

#if CONFIG_ENABLE_MDNS
  init_mdns();
#endif

  while (true) {
    sys::delay(sys::time::max);
  }
}
