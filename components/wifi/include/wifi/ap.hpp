/**
 * @file ap.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_WIFI_AP_HPP_
#define COMPONENTS_WIFI_AP_HPP_

#include <string_view>

#include "esp_netif.h"
#include "esp_wifi.h"

#include "sys/error.hpp"

namespace wifi {
namespace ap {

esp_netif_t* config(wifi::config&) noexcept;
sys::error ip(esp_netif_t*, const esp_netif_ip_info_t&) noexcept;

[[nodiscard]] wifi::config
default_init(std::string_view ssid,
             std::string_view password = "") noexcept;

}  // namespace ap
}  // namespace wifi

#endif  // COMPONENTS_WIFI_AP_HPP_