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

class water_flow_sensor {
 public:
  water_flow_sensor(gpio_num_t pin, int k) noexcept;

  sys::error start() noexcept;
  sys::error stop() noexcept;

  [[nodiscard]] std::optional<int>
  count() noexcept;
  sys::error clear() noexcept;

  template<typename Ktype = double>
  [[nodiscard]] std::optional<Ktype>
  volume() noexcept {
    auto value = pc_.count();
    if (value) return static_cast<Ktype>(*value) / k_;
    return std::nullopt;
  }

  template<typename Ktype = double>
  [[nodiscard]] Ktype
  volume(int count) {
    return static_cast<Ktype>(count) / k_;
  }

  int k_ratio() const noexcept;

 private:
  uc::pulse_counter pc_;
  uc::pulse_counter::channel channel_;
  int k_;
};

#endif  // COMPONENTS_PERIPHERALS_WATER_FLOW_HPP
