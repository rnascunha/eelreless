/**
 * @file wifi.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_WIFI_STATION_HPP_
#define COMPONENTS_WIFI_STATION_HPP_

#include "esp_system.h"
#include "esp_netif.h"
#include "esp_wifi.h"

namespace wifi {
namespace station {

esp_netif_t* config(wifi_config_t& wifi_config) noexcept;
esp_err_t connect() noexcept;
esp_netif_ip_info_t ip(esp_netif_t* handler) noexcept;


}   // namespace station
}   // namespace wifi

#endif   // COMPONENTS_WIFI_STATION_HPP_
