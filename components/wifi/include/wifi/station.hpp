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

#include "esp_netif.h"

#include "wifi/common.hpp"

namespace wifi {
namespace station {

esp_netif_t* config(wifi::config&) noexcept;

}   // namespace station
}   // namespace wifi

#endif   // COMPONENTS_WIFI_STATION_HPP_
