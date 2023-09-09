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
                            int k_sensor) noexcept 
  : control(valve_port, sensor_port, k_sensor) 
#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
    , safe_timer(esp_timer_create_args_t{
    .callback = safe_timer_cb,
    .arg = this,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "",
    .skip_unhandled_events = false
  })
#endif  // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
    {}

void control_valve::start(websocket::request& req,
                          int freq, int limit,
                          bool clear) noexcept {
  if (clear) {
    control.clear_count();
    pulses = 0;
  } else pulses += *control.count();

  this->freq = freq;
  client = websocket::client(req);

  if (control.is_open()) {
    control.close();
    control.disable();
    if (this->limit > 0)
      control.remove_callback(this->limit);
  }
  if (limit > 0) {
    int temp = limit * control.k_ratio();
    this->limit = temp / 1000;
    this->limit += temp % 1000;
    control.register_callback(std::initializer_list<int>{this->limit},
                              pcnt_event_callbacks_t{.on_reach = count_volume_cb,},
                              this);
  }

  control.enable();
  control.clear_count();
  control.open();

#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
  // Dont use restart because maybe is not started yet
  safe_timer.stop();
  safe_timer.start_once(std::chrono::milliseconds(CONFIG_SAFE_TIMER_IDLE_MILISECONDS));
#endif  // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
}

void control_valve::close() noexcept {
  freq = 0;
  control.close();
  control.disable();
  if (this->limit > 0)
    control.remove_callback(this->limit);

#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
  safe_timer.stop();
#endif  // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
}

#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
bool control_valve::check_update_timer_args() noexcept {
  auto count = control.count();
  if (!count) return false;
  if (*count != last_count || pulses != last_pulses) {
    last_count = *count;
    last_pulses = pulses;
    safe_timer.restart(std::chrono::milliseconds(CONFIG_SAFE_TIMER_IDLE_MILISECONDS));
    return false;
  }
  return true;
}

#endif  // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1

void control_valve::check() noexcept {
  auto b = events.wait(bit_init | bit_step | bit_timer,
    freq == 0 ? sys::time::max : sys::time::to_ticks(std::chrono::milliseconds(freq)));
  
  if (sys::event_group::is_set(b, bit_init)) return;
  if (sys::event_group::is_set(b, bit_step)) {
    close();
    send_state(*this);
    return;
  }
#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1  
  if ((b & bit_timer) && check_update_timer_args()) {
    close();
    send_state(*this);
    send_error(client, command::state, error_description::safe_timer_idle);
    return;
  }
#endif  // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
  if (freq > 0)
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

  // must test if is needed
  // if (edata->watch_point_value == uc::pulse_counter::high_limit) return pdFALSe;
  info.events.set_from_ISR(control_valve::bit_step, &high_task_wakeup);

  return (high_task_wakeup == pdTRUE);
}

#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
static void safe_timer_cb(void* arg) noexcept {
  control_valve& info = *((control_valve*)(arg));
  info.events.set_from_ISR(control_valve::bit_timer);
}
#endif  // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1