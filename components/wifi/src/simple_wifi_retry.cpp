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
#include <chrono>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"

#include "sys/time.hpp"
#include "sys/event.hpp"

#include "wifi/common.hpp"
#include "wifi/simple_wifi_retry.hpp"

namespace wifi {
namespace station {

simple_wifi_retry::simple_wifi_retry(
  int max_retry /* = std::numeric_limits<int>::max() */) noexcept
  : max_retry_{max_retry} {
  register_handler(*this);
}

simple_wifi_retry::simple_wifi_retry(not_register,
                                     int max_retry /* = std::numeric_limits<int>::max() */) noexcept
 : max_retry_{max_retry} {}

EventBits_t 
simple_wifi_retry::wait(sys::time::ticks wait_time /* = sys::time::max */) {
  return xEventGroupWaitBits(event_,
                            connected | fail,
                            pdFALSE,
                            pdFALSE,
                            wait_time);
}

void
simple_wifi_retry::handler(void* arg,
            esp_event_base_t event_base,
            std::int32_t event_id,
            void* event_data) noexcept {
  simple_wifi_retry* self = (simple_wifi_retry*)arg;
  if (event_base == WIFI_EVENT)
    self->wifi_handler(arg, event_id, event_data);
  else if (event_base == IP_EVENT)
    self->ip_handler(arg, event_id, event_data);
}

void simple_wifi_retry::reset() noexcept {
  retry_ = 0;
  xEventGroupClearBits(event_, connected | fail);
}

void
simple_wifi_retry::wifi_handler(void* arg,
                                std::int32_t event_id,
                                void* event_data) noexcept {
  if (event_id == WIFI_EVENT_STA_START)
    esp_wifi_connect(); 
  else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (retry_++ < max_retry_)
      esp_wifi_connect();
    else
      xEventGroupSetBits(event_, fail);
  }
}

void
simple_wifi_retry::ip_handler(void* arg,
                              std::int32_t event_id,
                              void* event_data) noexcept {
  retry_ = 0;
  xEventGroupSetBits(event_, connected);
}

bool
simple_wifi_retry::is_connected() const noexcept {
  return (connected & xEventGroupGetBits(event_)) != 0;
}

bool
simple_wifi_retry::failed() const noexcept {
  return (fail & xEventGroupGetBits(event_)) != 0;
}

void register_handler(simple_wifi_retry& instance) noexcept {
  wifi::register_handler(ESP_EVENT_ANY_ID, &simple_wifi_retry::handler, &instance);
  sys::event::register_handler(IP_EVENT, IP_EVENT_STA_GOT_IP, &simple_wifi_retry::handler, &instance);
}

}  // namespace station 
}  // namespace wifi


