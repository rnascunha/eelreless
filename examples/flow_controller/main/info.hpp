/**
 * @file info.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef MAIN_INFO_HPP_
#define MAIN_INFO_HPP_

#include "sys/timer.hpp"
#include "sys/task.hpp"

#include "control_flow.hpp"
#include "websocket/server.hpp"

struct control_valve {
  static constexpr sys::event_group::bits bit_init = BIT0;
  static constexpr sys::event_group::bits bit_step = BIT1;
  static constexpr sys::event_group::bits bit_timer = BIT2;

  control_flow        control;
#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
  sys::timer          safe_timer;
  int                 last_pulses = 0;
  int                 last_count = 0;
#endif  // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
  sys::event_group    events{};
  websocket::client   client{};
  int freq            = 0;      // milieconds
  int pulses          = 0;      // hold pulses when stop/start
  int limit           = 0;      // mililiters

  control_valve(gpio_num_t valve_port,
                gpio_num_t sensor_port,
                int k_sensor) noexcept;

  void start(websocket::request&, int freq, int limit, bool clear) noexcept;
  void close() noexcept;

  void check() noexcept;

#if CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
  bool check_update_timer_args() noexcept;
#endif  // CONFIG_ENABLE_SAFE_TIMER_IDLE == 1
};

#endif // MAIN_INFO_HPP_
