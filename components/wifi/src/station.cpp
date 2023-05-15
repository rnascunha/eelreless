/**
 * @file station.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <cstring>
#include <string_view>

#include "esp_netif.h"
#include "esp_wifi.h"

#include "sys/error.hpp"
#include "wifi/station.hpp"

namespace wifi {
namespace station {

esp_netif_t* config(wifi::config& wifi_config) noexcept {
  auto net = esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  sys::error err = esp_wifi_init(&cfg);
  if (err) {
    esp_netif_destroy(net);
    return nullptr;
  }
  err = esp_wifi_set_mode(WIFI_MODE_STA);
  if (err) {
    esp_netif_destroy(net);
    return nullptr;
  }
  err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
  if (err) {
    esp_netif_destroy(net);
    return nullptr;
  }
  
  return net;
}

}   // namespace station
}   // namespace wifi
