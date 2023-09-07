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
#include <inttypes.h>

#include <cstdint>
#include <chrono>
#include <type_traits>
#include <string_view>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#include "sys/sys.hpp"

#include "wifi/common.hpp"
#include "wifi/station.hpp"
#include "wifi/simple_wifi_retry.hpp"
#include "http/server_connect_cb.hpp"
#include "websocket/server.hpp"

#include "control_flow.hpp"

#include "packets.hpp"
#include "resources.cpp"

#include "wifi_args.hpp"

#include "info.hpp"

#if CONFIG_ENABLE_MDNS
#include "mdns.h"
#endif  // CONFIG_ENABLE_MDNS

#if CONFIG_ENABLE_MDNS
void init_mdns() noexcept {
  if (mdns_init()) {
    ESP_LOGW(TAG, "MDNS Init failed");
    return;
  }
  mdns_hostname_set(CONFIG_MDNS_HOSTNAME);
  ESP_LOGI(TAG, "Hostname: " CONFIG_MDNS_HOSTNAME);
}
#endif

#define STEP_100      0

#define VALVE_GPIO      GPIO_NUM_21
#define SENSOR_GPIO     GPIO_NUM_17

#if STEP_100 == 1
# define K_SENSOR        438     // 100
# define STEP            100
#else
# define K_SENSOR        44     // 10
# define STEP            10
#endif

static bool count_volume(pcnt_unit_handle_t unit,
                         const pcnt_watch_event_data_t *edata,
                         void *user_ctx) {
  BaseType_t high_task_wakeup;
  control_valve& info = *(control_valve*)user_ctx;
  // send event data to queue, from this interrupt callback
  info.volume += info.control.step();
  info.control.clear_count();
  xQueueSendFromISR(info.queue, &(edata->watch_point_value), &high_task_wakeup);

  return (high_task_wakeup == pdTRUE);
}

extern "C" void app_main() {
  /**
   * Initiate chip
   */
  if (sys::default_net_init()) {
    ESP_LOGE(TAG, "Error initiating chip");
    return;
  }

  /**
   * WiFi configuration
   */
  wifi::config config = wifi::station::build_config(EXAMPLE_ESP_WIFI_SSID,
                                                    EXAMPLE_ESP_WIFI_PASS)
                          .sae_h2e_identifier(EXAMPLE_H2E_IDENTIFIER)
                          .authmode(ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD)
                          .sae_pwe_h2e(ESP_WIFI_SAE_MODE);

  auto* net_handler = wifi::station::config(config);
  if (net_handler == nullptr) {
    ESP_LOGE(TAG, "Configure WiFi error");
    return;
  }

  control_valve info = {
    .control = control_flow(VALVE_GPIO, SENSOR_GPIO, K_SENSOR, STEP),
    .queue = xQueueCreate(10, sizeof(int))
  };

  /**
   * Starting WiFi/Websocket server
   */
  wifi::station::simple_wifi_retry wifi{};
  http::server_connect_cb([&info](auto& server) {
    server.register_uri(
      websocket::uri<ws_cb>{
        .uri            = "/ws",
        .user_ctx       = &info,
        .control_frames = false,
        .supported_subprotocol = nullptr
      }(),
      http::server::error{
        .code      = HTTPD_404_NOT_FOUND,
        .handler   = http_404_error_handler
      }
    );
  });

  if (wifi::start()) {
    ESP_LOGE(TAG, "Error connecting to network");
    return;
  }

  wifi.wait();

  if (wifi.is_connected()) {
    auto ip = wifi::ip(net_handler).ip;
    ESP_LOGI(TAG, "Connected! IP: %" PRIu32 ".%" PRIu32 ".%" PRIu32 ".%" PRIu32, ip.addr & 0xFF,
                                                ip.addr >> 8 & 0xFF,
                                                ip.addr >> 16 & 0xFF,
                                                ip.addr >> 24);
  } else {
    ESP_LOGI(TAG, "Failed");
    return;
  }

#if CONFIG_ENABLE_MDNS
  init_mdns();
#endif

  pcnt_event_callbacks_t cbs = {
    .on_reach = count_volume,
  };

  info.control.register_callback(std::initializer_list<int>{K_SENSOR}, cbs, &info);
  info.control.enable();
  info.control.clear_count();

  int event_count = 0;
  while (true) {
    if (xQueueReceive(info.queue,
                      &event_count,
                      info.freq == 0 ? sys::time::max : pdMS_TO_TICKS(info.freq * 1000))) {
      if (info.limit != 0 && info.volume >= info.limit) {
        info.control.close();
        info.limit = 0;
        info.freq = 0;
        send_state(info);
      }
    }
    if (info.freq > 0)
      send_state(info);
  }
}
