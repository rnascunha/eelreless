#include <string_view>
#include <cstring>

#include "esp_netif.h"
#include "esp_wifi.h"

#include "sys/error.hpp"

#include "wifi/common.hpp"
#include "wifi/ap.hpp"

namespace wifi {
namespace ap {

esp_netif_t* config(wifi::config& cfg) noexcept {
  auto net = esp_netif_create_default_wifi_ap();

  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  sys::error err = esp_wifi_init(&config);
  if (err) {
    esp_netif_destroy(net);
    return nullptr;
  }

  err = esp_wifi_set_mode(WIFI_MODE_AP);
  if (err) {
    esp_netif_destroy(net);
    return nullptr;
  }

  err = esp_wifi_set_config(WIFI_IF_AP, &cfg);
  if (err) {
    esp_netif_destroy(net);
    return nullptr;
  }
  
  return net;
}

sys::error ip(esp_netif_t* driver,
              const esp_netif_ip_info_t& ip) noexcept {
  auto ret0 = esp_netif_dhcps_stop(driver);
	sys::error ret = esp_netif_set_ip_info(driver, &ip);
  if(ret0 != ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED)
	  esp_netif_dhcps_start(driver);
  return ret;
}

[[nodiscard]] wifi::config
default_init(std::string_view ssid,
             std::string_view password /* = "" */) noexcept {
  wifi::config config = {};
  std::strncpy((char*)config.ap.ssid, ssid.data(), ssid.size());
  std::strncpy((char*)config.ap.password, password.data(), password.size());

  if (password.size() == 0)
    config.ap.authmode = WIFI_AUTH_OPEN;

  return config;
}

}  // namespace ap
}  // namespace wifi
