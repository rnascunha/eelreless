#ifndef COMPONENTS_WIFI_IMPL_AP_HPP_
#define COMPONENTS_WIFI_IMPL_AP_HPP_

#include <cstdint>
#include <cstring>
#include <string_view>

#include "esp_wifi_types.h"

namespace wifi {
namespace ap {

constexpr
build_config::build_config(std::string_view ssid_,
                           std::string_view pass /* = "" */) noexcept {
  ssid(ssid_);
  password(pass);
}

constexpr
build_config::operator wifi::config() noexcept {
  return cfg_;
}

constexpr build_config&
build_config::ssid(std::string_view ssid) noexcept {
  std::memcpy(cfg_.ap.ssid, ssid.data(), ssid.size());
  cfg_.ap.ssid_len = ssid.size();
  return *this;
}

constexpr build_config&
build_config::password(std::string_view pass) noexcept {
  std::memcpy(cfg_.ap.password, pass.data(), pass.size());
  if (pass.size() == 0)
    authmode(WIFI_AUTH_OPEN);
  return *this;
}

constexpr build_config&
build_config::channel(std::uint8_t val) noexcept {
  cfg_.ap.channel = val;
  return *this;
}

constexpr build_config&
build_config::authmode(wifi_auth_mode_t val) noexcept {
  cfg_.ap.authmode = val;
  return *this;
}

constexpr build_config&
build_config::hidden(std::uint8_t val) noexcept {
  cfg_.ap.ssid_hidden = val;
  return *this;
}

constexpr build_config&
build_config::max_connection(std::uint8_t val) noexcept {
  cfg_.ap.max_connection = val;
  return *this;
}

constexpr build_config&
build_config::beacon_interval(std::uint16_t val) noexcept {
  cfg_.ap.beacon_interval = val;
  return *this;
}

constexpr build_config&
build_config::pairwise_cipher(wifi_cipher_type_t val) noexcept {
  cfg_.ap.pairwise_cipher = val;
  return *this;
}

constexpr build_config&
build_config::ftm_responder(bool val) noexcept {
  cfg_.ap.ftm_responder = val;
  return *this;
}

constexpr build_config&
build_config::pmf_cfg(const wifi_pmf_config_t& val) noexcept {
  cfg_.ap.pmf_cfg = val;
  return *this;
}

constexpr build_config&
build_config::sae_pwe_h2e(wifi_sae_pwe_method_t val) noexcept {
  cfg_.ap.sae_pwe_h2e = val;
  return *this;
}

[[nodiscard]] constexpr wifi_ap_config_t&
build_config::native() noexcept {
  return cfg_.ap;
}

}  // namespace ap
}  // namespace wifi

#endif  // COMPONENTS_WIFI_IMPL_AP_HPP_
