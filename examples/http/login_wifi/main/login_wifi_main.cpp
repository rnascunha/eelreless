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

#include "lg/log.hpp"
#include "lg/format_types.hpp"

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
lg::log ll{"LoginWifi"};

extern "C" void app_main() {
  auto err = sys::default_net_init();
  if (err) {
    ll.error("Erro initializing chip [{:b}]", err);
    return;
  }

  sys::nvs storage;
  err = storage.open(NVS_NAMESPACE);
  if (err) {
    ll.error("Error initiating NVS storage");
    return;
  }

  auto reset_reboot = [&storage]() {
    storage.erase(NVS_KEY_SSID);
    storage.erase(NVS_KEY_PASS);
    storage.commit();
    ll.info("Erased NVS SSID and PASSWORD. Rebooting...");
    sys::reboot();
  };

  char ssid[64];
  std::size_t size = 64;
  err = storage.get(NVS_KEY_SSID, ssid, size);
  if (err) {
    ll.info("SSID not found {:b}", err);
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
    ll.error("Password not configured {:b}", err);
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
    ll.error("Configure WiFi error");
    sys::reboot();
    return;
  }

  wifi::station::simple_wifi_retry retry{EXAMPLE_ESP_MAXIMUM_RETRY};
  err = wifi::start();
  if (err) {
    ll.error("Connect WiFi error {:b}", err);
    sys::reboot();
    return;
  }

  ll.info("WiFi connecting to SSID:{} password:{}",
                 (const char*)config.sta.ssid, (const char*)config.sta.password);

  retry.wait();

  if (retry.failed()) {
    ll.warn("Failed to connect to SSID:{} password:{}. Reseting parameters and rebooting",
                 (const char*)config.sta.ssid, (const char*)config.sta.password);
    reset_reboot();
    return;
  }

  ll.info("Connected! IP: {}", wifi::ip(net_handler).ip);

#if CONFIG_ENABLE_MDNS == 1
  init_mdns();
#endif  // CONFIG_ENABLE_MDNS == 1

  facility::debounce btn(uc::gpio((gpio_num_t)CONFIG_PIN_BUTTON, GPIO_MODE_INPUT),
                         CONFIG_TIME_HOLD_BUTTON);
  using namespace std::chrono_literals;
  btn.wait(1s, reset_reboot);
}
