/**
 * @file simple_wifi_retry.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"

#include "sys/event.hpp"
#include "wifi/simple_wifi_retry.hpp"

namespace wifi {
namespace station {

EventBits_t 
simple_wifi_retry::wait(TickType_t wait_time /* = portMAX_DELAY */) {
  return xEventGroupWaitBits(wifi_event_group,
                            connected | fail,
                            pdFALSE,
                            pdFALSE,
                            wait_time);
}

void
simple_wifi_retry::handler(void* arg,
            esp_event_base_t event_base,
            int32_t event_id,
            void* event_data) noexcept {
  simple_wifi_retry* self = (simple_wifi_retry*)arg;
  if (event_base == WIFI_EVENT)
    self->wifi_handler(arg, event_id, event_data);
  else if (event_base == IP_EVENT)
    self->ip_handler(arg, event_id, event_data);
}

void
simple_wifi_retry::wifi_handler(void* arg,
                                int32_t event_id,
                                void* event_data) noexcept {
  if (event_id == WIFI_EVENT_STA_START)
    esp_wifi_connect(); 
  else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (retry_++ < max_retry_)
      esp_wifi_connect();
    else
      xEventGroupSetBits(wifi_event_group, fail);
  }
}

void
simple_wifi_retry::ip_handler(void* arg,
                              int32_t event_id,
                              void* event_data) noexcept {
  retry_ = 0;
  xEventGroupSetBits(wifi_event_group, connected);
}

void register_handler(simple_wifi_retry& instance) noexcept {
  sys::event::register_handler(WIFI_EVENT, ESP_EVENT_ANY_ID, &simple_wifi_retry::handler, &instance);
  sys::event::register_handler(IP_EVENT, IP_EVENT_STA_GOT_IP, &simple_wifi_retry::handler, &instance);
}

}  // namespace station 
}  // namespace wifi


