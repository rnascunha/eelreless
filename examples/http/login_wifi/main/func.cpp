/**
 * @file func.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <cstring>

#include "lwip/ip4_addr.h"

#include "resources.cpp"

#include "sys/error.hpp"
#include "sys/sys.hpp"

#include "wifi/common.hpp"
#include "wifi/ap.hpp"

#include "http/server.hpp"

static constexpr const char* TAG = "Main Func";

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_WIFI_AP_SSID
#define EXAMPLE_ESP_WIFI_CHANNEL   1
#define EXAMPLE_MAX_STA_CONN       4

#if CONFIG_ENABLE_MDNS == 1
#include "mdns.h"

void init_mdns() noexcept {
  if (mdns_init()) {
    ESP_LOGW(TAG, "MDNS Init failed");
    return;
  }
  mdns_hostname_set(CONFIG_MDNS_HOSTNAME);
  ESP_LOGI(TAG, "Hostname: " CONFIG_MDNS_HOSTNAME);
}
#endif  // CONFIG_ENABLE_MDNS == 1

void configure_wifi(sys::nvs& storage) noexcept {
  ESP_LOGI(TAG, "WiFi parameters not configured.");
  
  wifi::config cfg = {};
  std::strcpy((char*)cfg.ap.ssid, EXAMPLE_ESP_WIFI_SSID);
  cfg.ap.channel = EXAMPLE_ESP_WIFI_CHANNEL;
  cfg.ap.authmode = WIFI_AUTH_OPEN;
  cfg.ap.max_connection = EXAMPLE_MAX_STA_CONN,
  cfg.ap.pmf_cfg.required = true;

  auto* net = wifi::ap::config(cfg);
  if (net == nullptr) {
    ESP_LOGE(TAG, "Erro configuring WiFi AP");
    return;
  }

  esp_netif_ip_info_t ip;
  IP4_ADDR(&ip.ip, 192, 168, 2, 1);
	IP4_ADDR(&ip.gw, 192, 168, 2, 1);
	IP4_ADDR(&ip.netmask, 255, 255, 255, 0);
	wifi::ap::ip(net, ip);

  sys::error err = wifi::start();
  if (err) {
    ESP_LOGE(TAG, "Erro initiating WiFi AP [%d]", err.value());
    return;
  }

#if CONFIG_ENABLE_MDNS == 1
  init_mdns();
#endif  // CONFIG_ENABLE_MDNS == 1

  ESP_LOGI(TAG, "WiFi started. SSID:%s channel:%d",
                EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_CHANNEL);

  http::server server(80);
  if (!server.is_connected()) {
    ESP_LOGE(TAG, "HTTP server not initiated");
    return;
  }

  server.register_uri(http::server::uri{
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = web_get_handler,
    .user_ctx  = nullptr
  }, http::server::uri{
    .uri       = "/",
    .method    = HTTP_POST,
    .handler   = web_post_handler,
    .user_ctx  = &storage
  }, http::server::uri{
    .uri       = "/reboot",
    .method    = HTTP_GET,
    .handler   = reboot_get_handler,
    .user_ctx  = nullptr
  }, http::server::error{
    .code      = HTTPD_404_NOT_FOUND,
    .handler   = http_404_error_handler
  });

  while(true) {
    sys::delay(sys::time::max);
  }
}


