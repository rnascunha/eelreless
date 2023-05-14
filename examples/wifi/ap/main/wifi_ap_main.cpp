// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
#include "esp_mac.h"

#include <cstring>

#include "esp_log.h"

#include "sys/event.hpp"
#include "sys/sys.hpp"

#include "wifi/common.hpp"
#include "wifi/ap.hpp"

#if CONFIG_FIXE_AP_IP == 1
#include "lwip/ip4_addr.h"
#endif  // CONFIG_FIXE_AP_IP == 1

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN       CONFIG_ESP_MAX_STA_CONN

static constexpr const char *TAG = "WiFi AP";

static void
wifi_event_handler(void* arg,
                   esp_event_base_t event_base,
                   std::int32_t event_id,
                   void* event_data) noexcept {
  if (event_id == WIFI_EVENT_AP_STACONNECTED) {
    wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
    ESP_LOGI(TAG, "station " MACSTR " join, AID=%d",
            MAC2STR(event->mac), event->aid);
  } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
    wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
    ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
            MAC2STR(event->mac), event->aid);
  }
}

extern "C" void app_main(void) {
  auto err = sys::default_net_init();
  if (err) {
    ESP_LOGE(TAG, "Erro initializing chip [%d]", err.value());
    return;
  }

  wifi::config cfg = wifi::ap::default_init(EXAMPLE_ESP_WIFI_SSID,
                                            EXAMPLE_ESP_WIFI_PASS);
  cfg.ap.channel = EXAMPLE_ESP_WIFI_CHANNEL;
  cfg.ap.max_connection = EXAMPLE_MAX_STA_CONN,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
  cfg.ap.authmode = WIFI_AUTH_WPA3_PSK,
  cfg.ap.sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
  cfg.ap.authmode = WIFI_AUTH_WPA2_PSK,
#endif
  cfg.ap.pmf_cfg.required = true;
  
  if (cfg.ap.size() == 0) {
    cfg.ap.authmode = WIFI_AUTH_OPEN;
  }

  auto* net = wifi::ap::config(cfg);
  if (net == nullptr) {
    ESP_LOGE(TAG, "Erro configuring WiFi AP");
    return;
  }

#if CONFIG_FIXE_AP_IP == 1
  esp_netif_ip_info_t ip;
  IP4_ADDR(&ip.ip, 192, 168, 2, 1);
	IP4_ADDR(&ip.gw, 192, 168, 2, 1);
	IP4_ADDR(&ip.netmask, 255, 255, 255, 0);
	wifi::ap::ip(net, ip);
#endif  // CONFIG_FIXE_AP_IP == 1

  wifi::register_handler(ESP_EVENT_ANY_ID, &wifi_event_handler);
  err = wifi::start();
  if (err) {
    ESP_LOGE(TAG, "Erro initiating WiFi AP [%d]", err.value());
    return;
  }

  ESP_LOGI(TAG, "WiFi started. SSID:%s password:%s channel:%d",
                EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}