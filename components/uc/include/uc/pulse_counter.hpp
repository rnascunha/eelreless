/**
 * @file pulse_counter.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-01
 * 
 * @copyright Copyright (c) 2023
 * 
 * @see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/pcnt.html
 */

#ifndef COMPONENTS_UC_PULSE_COUNTER_HPP_
#define COMPONENTS_UC_PULSE_COUNTER_HPP_

#include <optional>

#include "driver/gpio.h"
#include "driver/pulse_cnt.h"

#include "sys/error.hpp"

namespace uc {

class pulse_counter {
 public:
  static constexpr const int low_limit = -32768;    // -0x7fff - 1
  static constexpr const int high_limit = 32767;      // 0x7fff
  
  static constexpr const int unused = GPIO_NUM_NC;

  class channel {
   public:
    channel(pulse_counter&, const pcnt_chan_config_t&) noexcept;
    ~channel() noexcept;

    [[nodiscard]] pcnt_channel_handle_t
    handler() noexcept;

    [[nodiscard]] pcnt_channel_handle_t
    release() noexcept;

    sys::error
    edge_action(pcnt_channel_edge_action_t positive,
                pcnt_channel_edge_action_t negative) noexcept;
    sys::error
    level_action(pcnt_channel_level_action_t high,
                 pcnt_channel_level_action_t low) noexcept;

    sys::error
    erase() noexcept;

   private:
    pcnt_channel_handle_t channel_ = nullptr;
  };

  pulse_counter(const pcnt_unit_config_t&) noexcept;
  ~pulse_counter() noexcept;

  [[nodiscard]] pcnt_unit_handle_t
  release() noexcept;

  [[nodiscard]] pcnt_unit_handle_t
  handler() noexcept;

  sys::error enable() noexcept;
  sys::error disable() noexcept;

  sys::error start() noexcept;
  sys::error stop() noexcept;

  sys::error erase() noexcept;

  sys::error
  glitch_filter(const pcnt_glitch_filter_config_t&) noexcept;

  [[nodiscard]] std::optional<int>
  count() const noexcept;
  sys::error clear() const noexcept;

  sys::error
  add_watch_point(int wpoint) noexcept;
  sys::error
  remove_watch_point(int wpoint) noexcept;
  sys::error
  register_callback(const pcnt_event_callbacks_t&, void* = nullptr) noexcept;
 private:
  pcnt_unit_handle_t unit_ = nullptr;
};

}  // namespace uc

#endif  // COMPONENTS_UC_PULSE_COUNTER_HPP_
