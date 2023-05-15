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
#include <chrono>
#include <inttypes.h>

#include "esp_log.h"

#include "sys/error.hpp"
#include "sys/sys.hpp"
#include "sys/time.hpp"
#include "sys/nvs.hpp"

#include "wifi/common.hpp"
#include "wifi/station.hpp"
#include "wifi/simple_wifi_retry.hpp"

#include "uc/gpio.hpp"
#include "facility/debounce.hpp"

#include "func.hpp"

#include "wifi_args.hpp"

static constexpr const
char *TAG = "LoginWifi";

extern "C" void app_main() {
  auto err = sys::default_net_init();
  if (err) {
    ESP_LOGE(TAG, "Erro initializing chip [%d]", err.value());
    return;
  }

  sys::nvs storage;
  err = storage.open(NVS_NAMESPACE);
  if (err) {
    ESP_LOGE(TAG, "Error initiating NVS storage");
    return;
  }

  auto reset_reboot = [&storage]() {
    storage.erase(NVS_KEY_SSID);
    storage.erase(NVS_KEY_PASS);
    storage.commit();
    ESP_LOGI(TAG, "Erased NVS SSID and PASSWORD. Rebooting...");
    sys::reboot();
  };

  char ssid[64];
  std::size_t size = 64;
  err = storage.get(NVS_KEY_SSID, ssid, size);
  if (err) {
    ESP_LOGI(TAG, "SSID not found %d/%s", err.value(), err.message());
    configure_wifi(storage);
    return;
  }

  /**
   * WiFi configuration/connection
   */
  wifi::station::build_config builder({ssid, size});
  size = 64;
  err = storage.get(NVS_KEY_PASS, ssid, size);
  if (err) {
    ESP_LOGE(TAG, "Password not configured %d/%s",
                        err.value(), err.message());
    reset_reboot();
    return;
  }
  wifi::config config = builder
                          .password({ssid, size})
                          .sae_h2e_identifier(EXAMPLE_H2E_IDENTIFIER)
                          .authmode(ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD)
                          .sae_pwe_h2e(ESP_WIFI_SAE_MODE);

  auto* net_handler = wifi::station::config(config);
  if (net_handler == nullptr) {
    ESP_LOGE(TAG,  "Configure WiFi error");
    sys::reboot();
    return;
  }

  wifi::station::simple_wifi_retry retry{EXAMPLE_ESP_MAXIMUM_RETRY};
  err = wifi::start();
  if (err) {
    ESP_LOGE(TAG, "Connect WiFi error %d", err.value());
    sys::reboot();
    return;
  }

  ESP_LOGI(TAG, "WiFi connecting to SSID:%s password:%s",
                 config.sta.ssid, config.sta.password);

  retry.wait();

  if (retry.failed()) {
    ESP_LOGW(TAG, "Failed to connect to SSID:%s password:%s. Reseting parameters and rebooting",
                 config.sta.ssid, config.sta.password);
    reset_reboot();
    return;
  }

  auto ip_info = wifi::ip(net_handler);
  ESP_LOGI(TAG, "Connected! IP:" IPSTR, IP2STR(&ip_info.ip));

#if CONFIG_ENABLE_MDNS == 1
  init_mdns();
#endif  // CONFIG_ENABLE_MDNS == 1

  facility::debounce btn(uc::gpio((gpio_num_t)CONFIG_PIN_BUTTON, GPIO_MODE_INPUT),
                         CONFIG_TIME_HOLD_BUTTON);
  using namespace std::chrono_literals;
  btn.wait(1s, reset_reboot);
}
