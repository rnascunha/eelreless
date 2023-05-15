#ifndef COMPONENTS_WIFI_IMPL_HPP_
#define COMPONENTS_WIFI_IMPL_HPP_

#include <cstring>
#include <string_view>

#include "esp_wifi_types.h"

namespace wifi {
namespace station {

constexpr
build_config::build_config(std::string_view ssid,
              std::string_view password /* = "" */) noexcept {
  std::memcpy(cfg_.sta.ssid, ssid.data(), ssid.size());
  std::memcpy(cfg_.sta.password, password.data(), password.size());
}

constexpr build_config&
build_config::ssid(std::string_view ssid) noexcept {
  std::memcpy(cfg_.sta.ssid, ssid.data(), ssid.size());
  return *this;
}

constexpr build_config&
build_config::password(std::string_view password) noexcept {
  std::memcpy(cfg_.sta.password, password.data(), password.size());
  return *this;
}

constexpr
build_config::operator wifi::config() noexcept {
  return cfg_;
}

constexpr build_config&
build_config::scan(wifi_scan_method_t sc) noexcept {
  cfg_.sta.scan_method = sc;
  return *this;
}

constexpr build_config&
build_config::bssid(std::uint8_t* mac, bool set/* = true */) noexcept {
  std::memcpy(cfg_.sta.bssid, mac, 6);
  cfg_.sta.bssid_set = set;
  return *this;
}

constexpr build_config&
build_config::channel(std::uint8_t ch) noexcept {
  cfg_.sta.channel = ch;
  return *this;
}

constexpr build_config&
build_config::listen_interval(std::uint16_t li) noexcept {
  cfg_.sta.listen_interval = li;
  return *this;
}

constexpr build_config&
build_config::sort(wifi_sort_method_t val) noexcept {
  cfg_.sta.sort_method = val;
  return *this;
}

constexpr build_config&
build_config::authmode(wifi_auth_mode_t val) noexcept {
  cfg_.sta.threshold.authmode = val;
  return *this;
}

constexpr build_config&
build_config::threshold_rssi(std::int8_t rssi) noexcept {
  cfg_.sta.threshold.rssi = rssi;
  return *this;
}

constexpr build_config&
build_config::pmf_cfg(const wifi_pmf_config_t& val) noexcept {
  cfg_.sta.pmf_cfg = val;
  return *this;
}

constexpr build_config&
build_config::rm_enabled(std::uint32_t val) noexcept {
  cfg_.sta.rm_enabled = val;
  return *this;
}

constexpr build_config&
build_config::btm_enabled(std::uint32_t val) noexcept {
  cfg_.sta.btm_enabled = val;
  return *this;
}

constexpr build_config&
build_config::mbo_enabled(std::uint32_t val) noexcept {
  cfg_.sta.mbo_enabled = val;
  return *this;
}

constexpr build_config&
build_config::ft_enabled(std::uint32_t val) noexcept {
  cfg_.sta.ft_enabled = val;
  return *this;
}

constexpr build_config&
build_config::owe_enabled(std::uint32_t val) noexcept {
  cfg_.sta.owe_enabled = val;
  return *this;
}

constexpr build_config&
build_config::transition_disable(std::uint32_t val) noexcept {
  cfg_.sta.transition_disable = val;
  return *this;
}

constexpr build_config&
build_config::sae_pwe_h2e(wifi_sae_pwe_method_t val) noexcept {
  cfg_.sta.sae_pwe_h2e = val;
  return *this;
}

constexpr build_config&
build_config::sae_pk_mode(wifi_sae_pk_mode_t val) noexcept {
  cfg_.sta.sae_pk_mode = val;
  return *this;
}

constexpr build_config&
build_config::failure_retry_cnt(std::uint8_t val) noexcept {
  cfg_.sta.failure_retry_cnt = val;
  return *this;
}

constexpr build_config&
build_config::he_dcm_set(std::uint32_t val) noexcept {
  cfg_.sta.he_dcm_set = val;
  return *this;
}

constexpr build_config&
build_config::he_dcm_max_constellation_tx(std::uint32_t val) noexcept {
  cfg_.sta.he_dcm_max_constellation_tx = val;
  return *this;
}

constexpr build_config&
build_config::he_dcm_max_constellation_rx(std::uint32_t val) noexcept {
  cfg_.sta.he_dcm_max_constellation_rx = val;
  return *this;
}

constexpr build_config&
build_config::he_mcs9_enabled(std::uint32_t val) noexcept {
  cfg_.sta.he_mcs9_enabled = val;
  return *this;
}

constexpr build_config&
build_config::he_su_beamformee_disabled(std::uint32_t val) noexcept {
  cfg_.sta.he_su_beamformee_disabled =  val;
  return *this;
}

constexpr build_config&
build_config::he_trig_su_bmforming_feedback_disabled(std::uint32_t val) noexcept {
  cfg_.sta.he_trig_su_bmforming_feedback_disabled = val;
  return *this;
}

constexpr build_config&
build_config::he_trig_mu_bmforming_partial_feedback_disabled(std::uint32_t val) noexcept {
  cfg_.sta.he_trig_mu_bmforming_partial_feedback_disabled = val;
  return *this;
}

constexpr build_config&
build_config::he_trig_cqi_feedback_disabled(std::uint32_t val) noexcept {
  cfg_.sta.he_trig_cqi_feedback_disabled = val;
  return *this;
}

constexpr build_config&
build_config::sae_h2e_identifier(std::string_view val) noexcept {
  std::memcpy(cfg_.sta.sae_h2e_identifier, val.data(), val.size());
  return *this;
}

[[nodiscard]] constexpr wifi_sta_config_t&
build_config::native() noexcept {
  return cfg_.sta;
}

}  // namespace station 
}  // namespace wifi

#endif  // COMPONENTS_WIFI_IMPL_HPP_