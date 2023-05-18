/**
 * @file https_main.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "lg/log.hpp"
#include "lg/format_types.hpp"

#include "sys/error.hpp"
#include "sys/sys.hpp"

#include "wifi/common.hpp"
#include "wifi/station.hpp"
#include "wifi/simple_wifi_retry.hpp"

#include "http/server_connect_cb.hpp"

#include "wifi_args.hpp"

static constexpr const
lg::log ll{"HTTP Server"};

/* An HTTP GET handler */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, "<h1>Hello Secure World!</h1>", HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

extern "C" void app_main() {
  auto err = sys::default_net_init();
  if (err) {
    ll.error("Erro initializing chip [{:b}]", err);
    return;
  }

  /**
   * WiFi configuration/connection
   */
  wifi::config config = wifi::station::build_config(EXAMPLE_ESP_WIFI_SSID,
                                                    EXAMPLE_ESP_WIFI_PASS)
                          .authmode(ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD)
                          .sae_h2e_identifier(EXAMPLE_H2E_IDENTIFIER)
                          .sae_pwe_h2e(ESP_WIFI_SAE_MODE);

  auto* net_handler = wifi::station::config(config);
  if (net_handler == nullptr) {
    ll.error("Configure WiFi error");
    return;
  }

  wifi::station::simple_wifi_retry retry{EXAMPLE_ESP_MAXIMUM_RETRY};
  http::server_connect_cb<true> http_server{[](http::server& server) {
    server.register_uri(
      http::server::uri{
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_get_handler,
        .user_ctx = nullptr
      });
    }
  };
  
  extern const unsigned char servercert_start[] asm("_binary_mydomain_com_crt_start");
  extern const unsigned char servercert_end[]   asm("_binary_mydomain_com_crt_end");
  http_server.config.servercert = servercert_start;
  http_server.config.servercert_len = servercert_end - servercert_start;

  extern const unsigned char prvtkey_pem_start[] asm("_binary_mydomain_com_key_start");
  extern const unsigned char prvtkey_pem_end[]   asm("_binary_mydomain_com_key_end");
  http_server.config.prvtkey_pem = prvtkey_pem_start;
  http_server.config.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;
  
  err = wifi::start();
  if (err) {
    ll.error("Connect WiFi error {:b}", err);
    return;
  }

  ll.info("WiFi connecting to SSID:{} password:{}",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

  retry.wait();

  if (retry.is_connected()) {
    ll.info("Connected! IP: {}", wifi::ip(net_handler).ip);
  } else {
    ll.info("Failed");
    return;
  }

  while (true) {
    using namespace std::chrono_literals;
    sys::delay(5s);
  }
}