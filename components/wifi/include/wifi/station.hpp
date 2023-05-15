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

#include <string_view>
#include <cstdint>

#include "esp_netif.h"

#include "wifi/common.hpp"

namespace wifi {
namespace station {

esp_netif_t* config(wifi::config&) noexcept;

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
  scan(wifi_scan_method_t) noexcept;
  constexpr build_config&
  bssid(std::uint8_t*, bool = true) noexcept;
  constexpr build_config&
  channel(std::uint8_t) noexcept;
  constexpr build_config&
  listen_interval(std::uint16_t) noexcept;
  constexpr build_config&
  sort(wifi_sort_method_t) noexcept;
  constexpr build_config&
  authmode(wifi_auth_mode_t) noexcept;
  constexpr build_config&
  threshold_rssi(std::int8_t) noexcept;
  constexpr build_config&
  pmf_cfg(const wifi_pmf_config_t&) noexcept;
  constexpr build_config&
  rm_enabled(std::uint32_t) noexcept;
  constexpr build_config&
  btm_enabled(std::uint32_t) noexcept;
  constexpr build_config&
  mbo_enabled(std::uint32_t) noexcept;
  constexpr build_config&
  ft_enabled(std::uint32_t) noexcept;
  constexpr build_config&
  owe_enabled(std::uint32_t) noexcept;
  constexpr build_config&
  transition_disable(std::uint32_t) noexcept;
  constexpr build_config&
  sae_pwe_h2e(wifi_sae_pwe_method_t) noexcept;
  constexpr build_config&
  sae_pk_mode(wifi_sae_pk_mode_t) noexcept;
  constexpr build_config&
  failure_retry_cnt(std::uint8_t) noexcept;
  constexpr build_config&
  he_dcm_set(std::uint32_t) noexcept;
  constexpr build_config&
  he_dcm_max_constellation_tx(std::uint32_t) noexcept;
  constexpr build_config&
  he_dcm_max_constellation_rx(std::uint32_t) noexcept;
  constexpr build_config&
  he_mcs9_enabled(std::uint32_t) noexcept;
  constexpr build_config&
  he_su_beamformee_disabled(std::uint32_t) noexcept;
  constexpr build_config&
  he_trig_su_bmforming_feedback_disabled(std::uint32_t) noexcept;
  constexpr build_config&
  he_trig_mu_bmforming_partial_feedback_disabled(std::uint32_t) noexcept;
  constexpr build_config&
  he_trig_cqi_feedback_disabled(std::uint32_t) noexcept;
  constexpr build_config&
  sae_h2e_identifier(std::string_view) noexcept;

  [[nodiscard]] constexpr wifi_sta_config_t&
  native() noexcept;
 private:
  wifi::config cfg_{};
};

}   // namespace station
}   // namespace wifi

#include "impl/station.ipp"

#endif   // COMPONENTS_WIFI_STATION_HPP_
