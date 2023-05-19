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

#define MAIN_USE_LG     1

#if MAIN_USE_LG == 1
#include "lg/log.hpp"
#include "lg/format_types.hpp"
#else
#include "esp_log.h"
#endif 

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

#if MAIN_USE_LG == 1
static constexpr const
lg::log ll{"Eelreless"};
#else
static constexpr const
char *TAG = "Eelreless";
#endif 

#if CONFIG_ENABLE_MDNS
void init_mdns() noexcept {
  if (mdns_init()) {
#if MAIN_USE_LG == 1
    ll.warn("MDNS Init failed");
#else
    ESP_LOGW(TAG, "MDNS Init failed");
#endif 
      return;
  }
  mdns_hostname_set(CONFIG_MDNS_HOSTNAME);
#if MAIN_USE_LG == 1
    ll.info("Hostname: " CONFIG_MDNS_HOSTNAME);
#else
    ESP_LOGI(TAG, "Hostname: " CONFIG_MDNS_HOSTNAME);
#endif
}
#endif

extern "C" void app_main() {
  /**
   * Initiate chip
   */
  if (sys::default_net_init()) {
#if MAIN_USE_LG == 1
    ll.error("Error initiating chip");
#else
    ESP_LOGE(TAG, "Error initiating chip");
#endif
    return;
  }

  /**
   * Initiating ADC
   */
  auto adc = initiate_adc();
  if (!adc) {
#if MAIN_USE_LG == 1
    ll.error("Error initiating ADC");
#else
    ESP_LOGE(TAG, "Error initiating ADC");
#endif
    return;
  }

  /**
   * WiFi configuration
   */
  wifi::config config = wifi::station::build_config(EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS)
                                              .sae_h2e_identifier(EXAMPLE_H2E_IDENTIFIER)
                                              .authmode(ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD)
                                              .sae_pwe_h2e(ESP_WIFI_SAE_MODE);

  auto* net_handler = wifi::station::config(config);
  if (net_handler == nullptr) {
#if MAIN_USE_LG == 1
    ll.error("Configure WiFi error");
#else
    ESP_LOGE(TAG,  "Configure WiFi error");
#endif
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
#if MAIN_USE_LG == 1
    ll.error("Error connecting to network");
#else
    ESP_LOGE(TAG, "Error connecting to network");
#endif
    return;
  }

  wifi.wait();

  if (wifi.is_connected()) {
#if MAIN_USE_LG == 1
    ll.info("Connected! IP:{}", wifi::ip(net_handler).ip);
#else
    auto ip_info = wifi::ip(net_handler);
    ESP_LOGI(TAG, "Connected! IP:" IPSTR, IP2STR(&ip_info.ip));
#endif
  } else {
#if MAIN_USE_LG == 1
    ll.error("Failed");
#else
    ESP_LOGE(TAG, "Failed");
#endif
    return;
  }

#if CONFIG_ENABLE_MDNS
  init_mdns();
#endif

  while (true) {
    sys::delay(sys::time::max);
  }
}
