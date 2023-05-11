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

#include "esp_log.h"

#include "sys/error.hpp"
#include "sys/sys.hpp"
#include "sys/time.hpp"

#include "wifi/common.hpp"
#include "wifi/station.hpp"
#include "wifi/simple_wifi_retry.hpp"

#include "wifi_args.hpp"

static constexpr const
char *TAG = "WiFi Station";

extern "C" void app_main() {
  auto err = sys::default_net_init();
  if (err) {
    ESP_LOGE(TAG, "Erro initializing chip [%d]", err.value());
    return;
  }

  /**
   * WiFi configuration/connection
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

  wifi::station::simple_wifi_retry retry{EXAMPLE_ESP_MAXIMUM_RETRY};
  err = wifi::start();
  if (err) {
    ESP_LOGE(TAG, "Connect WiFi error %d", err.value());
    return;
  }

  ESP_LOGI(TAG, "WiFi connecting to SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

  retry.wait();

  if (retry.is_connected()) {
    auto ip_info = wifi::ip(net_handler);
    ESP_LOGI(TAG, "Connected! IP:" IPSTR, IP2STR(&ip_info.ip));
  } else if (retry.failed()) {
    ESP_LOGI(TAG, "Failed");
    return;
  } else {
    ESP_LOGI(TAG, "Unexpected event");
    return;
  }

  while (true) {
    using namespace std::chrono_literals;
    sys::delay(5s);
  }
}