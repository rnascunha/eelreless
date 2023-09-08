/**
 * @file info.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "sys/task.hpp"

#include "info.hpp"
#include "packets.hpp"

#include "esp_log.h"

static bool
count_volume_cb(pcnt_unit_handle_t unit,
             const pcnt_watch_event_data_t *edata,
             void *user_ctx) noexcept;

#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
static void safe_timer_cb(void* arg) noexcept;
#endif // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1

control_valve::control_valve(gpio_num_t valve_port,
                            gpio_num_t sensor_port,
                            int k_sensor,
                            int step) noexcept 
  : control(valve_port, sensor_port, k_sensor, step),
    task(xTaskGetCurrentTaskHandle()) {

  pcnt_event_callbacks_t cbs = {
    .on_reach = count_volume_cb,
  };

  control.register_callback(std::initializer_list<int>{k_sensor}, cbs, this);
  control.enable();
  control.clear_count();
  
#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
  esp_timer_create_args_t safe_timer_args = {
    .callback = safe_timer_cb,
    .arg = this,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "",
    .skip_unhandled_events = false
  };
  esp_timer_create(&safe_timer_args, &safe_timer);
#endif  // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
}

void control_valve::start(websocket::request& req,
                          int freq, int limit,
                          bool clear) noexcept {
  if (clear && !control.is_open()) {
    control.clear_count();
    volume = 0;
  }

  this->freq = freq;
  client = websocket::client(req);

  this->limit = limit;
  control.open();

  restart_timer();
}

void control_valve::close(bool clear /* = false */) noexcept {
  if (clear) {
    control.clear_count();
    volume = 0;
    limit = 0;
  }

  freq = 0;
  control.close();

#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
  esp_timer_stop(safe_timer);
#endif  // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
}

#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
void control_valve::restart_timer() noexcept {
  esp_timer_stop(safe_timer);
  esp_timer_start_once(safe_timer, CONFIG_SAFE_TIMER_IDLE_MILISECONDS * 1000);
}

bool control_valve::check_update_timer_args() noexcept {
  auto count = control.count();
  if (!count) return false;
  if (*count != last_count || volume != last_volume) {
    last_count = *count;
    last_volume = volume;
    restart_timer();
    return false;
  }
  return true;
}

#endif  // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1

void control_valve::check() noexcept {
  if (sys::notify_wait(freq == 0 ? 
                       sys::time::max :
                       sys::time::to_ticks(std::chrono::milliseconds(freq)))) {
#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
    if (try_abort && check_update_timer_args()) {
      try_abort = false;
      close();
      send_state(*this);
      send_error(client, command::state, error_description::safe_timer_idle);
      return;
    } else
#endif // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
    if (limit != 0 && volume >= limit) {
      close();
      send_state(*this);
      return;
    }
  } else if (freq > 0)
    send_state(*this);
#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
  check_update_timer_args();
#endif  // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
}

/**
 * callback function
 */

static bool
count_volume_cb(pcnt_unit_handle_t unit,
             const pcnt_watch_event_data_t *edata,
             void *user_ctx) noexcept {
  BaseType_t high_task_wakeup;
  control_valve& info = *(control_valve*)user_ctx;
  // send event data to queue, from this interrupt callback
  info.volume += info.control.step();
  info.control.clear_count();

  sys::notify_from_ISR(info.task, &high_task_wakeup);

  return (high_task_wakeup == pdTRUE);
}

#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
static void safe_timer_cb(void* arg) noexcept {
  control_valve& info = *((control_valve*)(arg));
  info.try_abort = true;
  sys::notify_from_ISR(info.task);
}
#endif  // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1