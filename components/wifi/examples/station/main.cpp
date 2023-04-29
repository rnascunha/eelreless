#include <cstring>

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include "wifi/station.hpp"
#include "wifi/simple_wifi_retry.hpp"

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

#if CONFIG_ESP_WPA3_SAE_PWE_HUNT_AND_PECK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK
#define EXAMPLE_H2E_IDENTIFIER ""
#elif CONFIG_ESP_WPA3_SAE_PWE_HASH_TO_ELEMENT
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HASH_TO_ELEMENT
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#elif CONFIG_ESP_WPA3_SAE_PWE_BOTH
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#endif
#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

static constexpr const char *TAG = "wifi station";

extern "C" void app_main() {
  /**
   * Chip Modules initialization
   */
  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());


  /**
   * WiFi configuration/connection
   */
  wifi_config_t config = {};
  std::strcpy((char*)config.sta.ssid, EXAMPLE_ESP_WIFI_SSID);
  std::strcpy((char*)config.sta.password, EXAMPLE_ESP_WIFI_PASS);
  std::strcpy((char*)config.sta.sae_h2e_identifier, EXAMPLE_H2E_IDENTIFIER);
  config.sta.threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD;
  config.sta.sae_pwe_h2e = ESP_WIFI_SAE_MODE;

  auto* net_handler = wifi::station::config(config);
  if (net_handler == nullptr) {
    ESP_LOGE(TAG,  "Configure WiFi error %d", ret);
    return;
  }

  wifi::station::simple_wifi_retry retry{EXAMPLE_ESP_MAXIMUM_RETRY};
  wifi::station::register_handler(retry);
  
  ret = wifi::station::connect();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG,  "Connect WiFi error %d", ret);
    return;
  }

  ESP_LOGI(TAG, "WiFi connecting!");

  auto bits = retry.wait();

  if (bits & wifi::station::simple_wifi_retry::connected) {
      ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
      auto ip_info = wifi::station::ip(net_handler);
      ESP_LOGI(TAG, "IP:" IPSTR, IP2STR(&ip_info.ip));
  } else if (bits & wifi::station::simple_wifi_retry::fail) {
      ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
  } else {
      ESP_LOGE(TAG, "UNEXPECTED EVENT");
  }
}