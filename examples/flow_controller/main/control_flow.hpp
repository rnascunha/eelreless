/**
 * @file control_flow.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MAIN_CONTROL_FLOW_HPP_
#define MAIN_CONTROL_FLOW_HPP_

#include <optional>

#include "sys/error.hpp"

#include "uc/gpio.hpp"
#include "peripherals/water_flow.hpp"

class control_flow {
 public:
  control_flow(gpio_num_t valve_port, gpio_num_t sensor_port, double k_sensor) noexcept;
  
  bool is_open() noexcept;

  void open() noexcept;
  void close() noexcept;

  sys::error clear_count() noexcept;
  std::optional<int> count() noexcept;
  std::optional<double> volume() noexcept;
  double volume(int count) noexcept;

  int k_ratio() const noexcept;

 private:
  uc::gpio valve_;
  water_flow_sensor sensor_;
};

#endif  // MAIN_CONTROL_FLOW_HPP_
