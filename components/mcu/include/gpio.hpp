/**
 * @file gpio.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_MCU_GPIO_HPP_
#define COMPONENTS_MCU_GPIO_HPP_

#include "driver/gpio.h"

#include "sys/error.hpp"

namespace mcu {

class gpio {
 public:
  gpio(gpio_num_t) noexcept;
  gpio(gpio_num_t, gpio_mode_t) noexcept;
  gpio(gpio_num_t, gpio_mode_t, gpio_pull_mode_t) noexcept;

  sys::error mode(gpio_mode_t) noexcept;
  sys::error pull(gpio_pull_mode_t) noexcept;

  int read() noexcept;
  sys::error write(int) noexcept;

  sys::error reset() noexcept;
 private:
  gpio_num_t pin_;
};

}  // namespace mcu

#endif  // COMPONENTS_MCU_GPIO_HPP_