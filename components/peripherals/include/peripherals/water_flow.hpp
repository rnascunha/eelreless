/**
 * @file water_flow.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_PERIPHERALS_WATER_FLOW_HPP
#define COMPONENTS_PERIPHERALS_WATER_FLOW_HPP

#include <optional>

#include "uc/pulse_counter.hpp"

template<typename Ktype = double>
class water_flow_sensor {
 public:
  water_flow_sensor(gpio_num_t pin, Ktype k) noexcept
    : pc_({
      .low_limit = -1,
      .high_limit = uc::pulse_counter::high_limit,
      .flags = {
        .accum_count = 0
      }
    }), 
    channel_(pc_, {
      .edge_gpio_num = pin,
      .level_gpio_num = uc::pulse_counter::unused,
      .flags = {}
    }), 
    k_(k) {
    channel_.edge_action(PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD);
    pc_.enable();
    pc_.clear();
  }

  sys::error start() noexcept {
    return pc_.start();
  }

  sys::error stop() noexcept {
    return pc_.stop();
  }

  [[nodiscard]] std::optional<int>
  count() noexcept {
    return pc_.count();
  }
  sys::error clear() noexcept {
    return pc_.clear();
  }

  [[nodiscard]] std::optional<Ktype>
  volume() noexcept {
    auto value = pc_.count();
    if (value) return *value / k_;
    return std::nullopt;
  }

  [[nodiscard]] Ktype
  volume(int count) {
    return count / k_;
  }

 private:
  uc::pulse_counter pc_;
  uc::pulse_counter::channel channel_;
  Ktype k_;
};

#endif  // COMPONENTS_PERIPHERALS_WATER_FLOW_HPP
