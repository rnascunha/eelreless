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

#include <cstdint>
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

class build_config {
 public:
  constexpr
  build_config(std::string_view ssid,
               std::string_view password = "") noexcept;

  constexpr
  operator wifi::config() noexcept;

  constexpr build_config&
  ssid(std::string_view) noexcept;
  constexpr build_config&
  password(std::string_view) noexcept;
  constexpr build_config&
  channel(std::uint8_t) noexcept;
  constexpr build_config&
  authmode(wifi_auth_mode_t) noexcept;
  constexpr build_config&
  hidden(std::uint8_t) noexcept;
  constexpr build_config&
  max_connection(std::uint8_t) noexcept;
  constexpr build_config&
  beacon_interval(std::uint16_t) noexcept;
  constexpr build_config&
  pairwise_cipher(wifi_cipher_type_t) noexcept;
  constexpr build_config&
  ftm_responder(bool) noexcept;
  constexpr build_config&
  pmf_cfg(const wifi_pmf_config_t&) noexcept;
  constexpr build_config&
  sae_pwe_h2e(wifi_sae_pwe_method_t) noexcept;

  [[nodiscard]] constexpr wifi_ap_config_t&
  native() noexcept;
 private:
  wifi::config cfg_{};
};

}  // namespace ap
}  // namespace wifi

#include "impl/ap.ipp"

#endif  // COMPONENTS_WIFI_AP_HPP_