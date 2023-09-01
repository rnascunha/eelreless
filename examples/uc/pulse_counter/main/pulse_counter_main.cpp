/**
 * @file pulse_counter_main.cpp
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
#include "freertos/queue.h"
#include "esp_log.h"

#include <chrono>

#include "sys/sys.hpp"
#include "uc/pulse_counter.hpp"

static const char *TAG = "PULSE";

#define EXAMPLE_INPUT_GPIO_A GPIO_NUM_17

static bool
example_pcnt_on_reach(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx) {
  BaseType_t high_task_wakeup;
  QueueHandle_t queue = (QueueHandle_t)user_ctx;
  // send event data to queue, from this interrupt callback
  xQueueSendFromISR(queue, &(edata->watch_point_value), &high_task_wakeup);
  return (high_task_wakeup == pdTRUE);
}

extern "C" void app_main(void)
{
  ESP_LOGI(TAG, "install pcnt unit");
  uc::pulse_counter counter({
    .low_limit = uc::pulse_counter::low_limit,
    .high_limit = uc::pulse_counter::high_limit,
    .flags = {
      .accum_count = 0
    }
  });
  if (counter.handler() == nullptr) {
    ESP_LOGE(TAG, "Error initating counter");
    return;
  }

  ESP_LOGI(TAG, "set glitch filter");
  counter.glitch_filter({.max_glitch_ns = 1000, });

  ESP_LOGI(TAG, "install pcnt channels");
  uc::pulse_counter::channel channel(counter, {
      .edge_gpio_num = EXAMPLE_INPUT_GPIO_A,
      .level_gpio_num = uc::pulse_counter::unused,
      .flags = {}
    }
  );
  if (channel.handler() == nullptr) {
    ESP_LOGE(TAG, "Error initating channel");
    return;
  }
  
  ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
  ESP_ERROR_CHECK(channel.edge_action(PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));

  for (int wpoint : {50, 100})
    ESP_ERROR_CHECK(counter.add_watch_point(wpoint));
  
  QueueHandle_t queue = xQueueCreate(10, sizeof(int));
  ESP_ERROR_CHECK(counter.register_callback({ .on_reach = example_pcnt_on_reach }, queue));

  ESP_LOGI(TAG, "enable pcnt unit");
  ESP_ERROR_CHECK(counter.enable());
  ESP_LOGI(TAG, "clear pcnt unit");
  ESP_ERROR_CHECK(counter.clear());
  ESP_LOGI(TAG, "start pcnt unit");
  ESP_ERROR_CHECK(counter.start());

  // Report counter value
  int event_count = 0;
  while (true) {
    if (xQueueReceive(queue, &event_count, pdMS_TO_TICKS(1000))) {
      ESP_LOGI(TAG, "Watch point event, count: %d", event_count);
    } else {
      auto value = counter.count();
      if (value)
        ESP_LOGI(TAG, "Pulse count: %d", *value);
    }
    using namespace std::chrono_literals;
    sys::delay(1s);
  }
}
