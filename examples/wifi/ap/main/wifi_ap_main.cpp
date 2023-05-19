/**
 * @file wifi_ap_main.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "lg/log.hpp"

#include "sys/event.hpp"
#include "sys/sys.hpp"

#include "wifi/common.hpp"
#include "wifi/ap.hpp"

#include "facility/mac.hpp"
#if CONFIG_FIXE_AP_IP == 1
#include "lg/format_types.hpp"
#include "facility/ip4.hpp"
#endif  // CONFIG_FIXE_AP_IP == 1

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN       CONFIG_ESP_MAX_STA_CONN

static constexpr const
lg::log ll{"WiFi AP"};

static void
wifi_event_handler(void* arg,
                   esp_event_base_t event_base,
                   std::int32_t event_id,
                   void* event_data) noexcept {
  if (event_id == WIFI_EVENT_AP_STACONNECTED) {
    wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
    // auto mac = facility::mac{event->mac[0], event->mac[1], event->mac[2],
    //                          event->mac[3], event->mac[4], event->mac[5]};
    // ll.info("station {} join, AID={}", mac, event->aid);
    ll.info("station {:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X} join, AID={}",
            event->mac[0], event->mac[1], event->mac[2],
            event->mac[3], event->mac[4], event->mac[5], 
            event->aid);
  } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
    wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
    ll.info("station {:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X} leave, AID={}",
            event->mac[0], event->mac[1], event->mac[2],
            event->mac[3], event->mac[4], event->mac[5], 
            event->aid);
  }
}

extern "C" void app_main(void) {
  auto err = sys::default_net_init();
  if (err) {
    ll.error("Erro initializing chip [{:b}]", err);
    return;
  }

  wifi::config cfg = wifi::ap::build_config(EXAMPLE_ESP_WIFI_SSID,
                                            EXAMPLE_ESP_WIFI_PASS)
                      .channel(EXAMPLE_ESP_WIFI_CHANNEL)
                      .max_connection(EXAMPLE_MAX_STA_CONN)
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
                      .authmode(WIFI_AUTH_WPA3_PSK)
                      .sae_pwe_h2e(WPA3_SAE_PWE_BOTH);
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
                      .authmode(WIFI_AUTH_WPA2_PSK);
#endif
  
  if (cfg.ap.password[0] == '\0') {
    cfg.ap.authmode = WIFI_AUTH_OPEN;
  }

  auto* net = wifi::ap::config(cfg);
  if (net == nullptr) {
    ll.error("Erro configuring WiFi AP");
    return;
  }

#if CONFIG_FIXE_AP_IP == 1
  using namespace facility::literals;
  auto ip = esp_netif_ip_info_t{
    .ip = "192.168.2.1"_ip4,
    .netmask = "255.255.255.0"_ip4,
    .gw = "192.168.2.1"_ip4
  };
	wifi::ap::ip(net, ip);
  ll.info("Configured fixed IP:{}, netmask:{}, gw:{}",
              ip.ip, ip.netmask, ip.gw);
#endif  // CONFIG_FIXE_AP_IP == 1

  wifi::register_handler(ESP_EVENT_ANY_ID, &wifi_event_handler);
  err = wifi::start();
  if (err) {
    ll.error("Erro initiating WiFi AP [{:b}]", err);
    return;
  }

  ll.info("SSID:{} password:{} channel:{}",
                EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}