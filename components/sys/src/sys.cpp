/**
 * @file sys.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <chrono>

#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"

#include "freertos/task.h"

#include "sys/sys.hpp"

namespace sys {

error default_net_init() noexcept {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ret = nvs_flash_erase();
    if (ret)
      return ret;
    ret = nvs_flash_init();
  }
  if (ret)
    return ret;

  ret = esp_netif_init();
  if (ret)
    return ret;

  return esp_event_loop_create_default();
}

void delay(sys::time::ticks ticks) noexcept {
  return vTaskDelay(ticks);
}

void reboot() noexcept {
  esp_restart();
}

}  // namespace sys
