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
#include "wifi/station.hpp"
#include "esp_netif.h"
#include "esp_wifi.h"

namespace wifi {
namespace station {

esp_netif_t* config(wifi_config_t& wifi_config) noexcept {
  auto net = esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_err_t err = esp_wifi_init(&cfg);
  if (err != ESP_OK) {
    esp_netif_destroy(net);
    return nullptr;
  }
  err = esp_wifi_set_mode(WIFI_MODE_STA);
  if (err != ESP_OK) {
    esp_netif_destroy(net);
    return nullptr;
  }
  err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
  if (err != ESP_OK) {
    esp_netif_destroy(net);
    return nullptr;
  }
  
  return net;
}

esp_err_t connect() noexcept {
  return esp_wifi_start();
}

esp_netif_ip_info_t ip(esp_netif_t* handler) noexcept {
  esp_netif_ip_info_t ip_info{};
  esp_netif_get_ip_info(handler, &ip_info);
  return ip_info;
}


}   // namespace station
}   // namespace wifi
