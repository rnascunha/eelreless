/**
 * @file timer.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_SYS_TIMER_HPP_
#define COMPONENTS_SYS_TIMER_HPP_

#include <cassert>

#include <chrono>

#include "esp_timer.h"

#include "sys/error.hpp"

namespace sys {

class timer {
 public:
  using handle = esp_timer_handle_t;

  timer(const esp_timer_create_args_t&) noexcept;
  ~timer() noexcept;

  template<typename Rep, typename Ratio>
  sys::error start_once(std::chrono::duration<Rep, Ratio> time) noexcept {
    assert(handler_ != nullptr && "Time handler cant be NULL");
    return esp_timer_start_once(handler_, std::chrono::microseconds(time).count());
  }

  template<typename Rep, typename Ratio>
  sys::error start_periodic(std::chrono::duration<Rep, Ratio> time) noexcept {
    assert(handler_ != nullptr && "Time handler cant be NULL");
    return esp_timer_start_periodic(handler_, std::chrono::microseconds(time).count());
  }

  template<typename Rep, typename Ratio>
  sys::error restart(std::chrono::duration<Rep, Ratio> time) noexcept {
    assert(handler_ != nullptr && "Time handler cant be NULL");
    return esp_timer_restart(handler_, std::chrono::microseconds(time).count());
  }

  sys::error stop() noexcept;
  sys::error erase() noexcept;

 private:
  handle handler_;
};

}  // namespace sys

#endif  // COMPONENTS_SYS_TIMER_HPP_
