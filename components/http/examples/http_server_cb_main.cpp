/**
 * @file main.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <cstring>
#include <unistd.h>

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_http_server.h"

#include "sys/sys.hpp"

#include "wifi/station.hpp"
#include "wifi/simple_wifi_retry.hpp"

#include "http/server_connect_cb.hpp"

#include "resources.cpp"

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

static constexpr const
char *TAG = "HTTP Server";

extern "C" void app_main() {
  /**
   * Chip Modules initialization
   */
  sys::default_net_init();
  
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
    ESP_LOGE(TAG,  "Configure WiFi error");
    return;
  }

  wifi::station::simple_wifi_retry retry{EXAMPLE_ESP_MAXIMUM_RETRY};
  http::server_connect_cb http_server{[](http::server& server) {
      server.register_uri(httpd_uri_t{
        .uri       = "/echo",
        .method    = HTTP_POST,
        .handler   = echo_post_handler,
        .user_ctx  = NULL
      });
      server.register_uri(httpd_uri{
        .uri       = "/hello",
        .method    = HTTP_GET,
        .handler   = hello_get_handler,
        .user_ctx  = (void* )"Hello World!"
      });
      server.register_err(http::http_error{HTTPD_404_NOT_FOUND, http_404_error_handler});
    }
  };
  http_server.config.server_port = 80;
  
  esp_err_t ret = wifi::station::connect();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG,  "Connect WiFi error %d", ret);
    return;
  }

  ESP_LOGI(TAG, "WiFi connecting to SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

  retry.wait();

  if (retry.is_connected()) {
    auto ip_info = wifi::station::ip(net_handler);
    ESP_LOGI(TAG, "Connected! IP:" IPSTR, IP2STR(&ip_info.ip));
  } else {
    ESP_LOGI(TAG, "Failed");
    return;
  }

  while (true) {
    sleep(5);
  }
}
