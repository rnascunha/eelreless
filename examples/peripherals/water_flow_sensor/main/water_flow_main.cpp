/**
 * @file water_flow_main.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "freertos/queue.h"
#include "esp_log.h"

#include <chrono>

#include "sys/sys.hpp"
#include "peripherals/water_flow.hpp"

static const char *TAG = "WATER FLOW";

#define WATER_FLOW_GPIO GPIO_NUM_17

// YF-S201
static constexpr const double K_water_flow = 450.0; // 450 pulse for 1L
// static constexpr const double K_water_flow = 4380.0; // 4380 pulse for 1L

extern "C" void app_main(void)
{
  water_flow_sensor wf(WATER_FLOW_GPIO, K_water_flow);
  wf.start();

  while (true) {
    auto value = wf.count();
    if (value)
      ESP_LOGI(TAG, "Pulse count: %d / Volume: %fL", *value, wf.volume(*value));
    using namespace std::chrono_literals;
    sys::delay(1s);
  }
}
