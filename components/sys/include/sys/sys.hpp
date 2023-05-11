/**
 * @file sys.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_SYS_SYS_HPP_
#define COMPONENTS_SYS_SYS_HPP_

#include <chrono>

#include "esp_err.h"

#include "sys/error.hpp"
#include "sys/time.hpp"

namespace sys {

error default_net_init() noexcept;

void delay(sys::time::ticks) noexcept;
template<typename Rep, typename Ratio>
void delay(std::chrono::duration<Rep, Ratio> duration) noexcept {
  return delay(time::to_ticks(duration));
}

void reboot() noexcept;

}  // namespace sys

#endif  // COMPONENTS_SYS_SYS_HPP_