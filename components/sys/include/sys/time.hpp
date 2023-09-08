/**
 * @file time.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_SYS_TIME_HPP_
#define COMPONENTS_SYS_TIME_HPP_

#include <chrono>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"

#include "sys/type_traits.hpp"

namespace sys {
namespace time {

using ticks = TickType_t;

static constexpr const
ticks max = portMAX_DELAY;

template<typename Rep, typename Ratio>
[[nodiscard]] constexpr ticks
to_ticks(std::chrono::duration<Rep, Ratio> duration) noexcept {
  auto value = std::chrono::duration_cast<std::chrono::duration<ticks, std::milli>>(duration).count();
  return pdMS_TO_TICKS(value);
}

[[nodiscard]] constexpr ticks
to_ticks(ticks duration) noexcept {
  return duration;
}

[[nodiscard]] std::chrono::microseconds
uptime() noexcept;

template<typename T>
concept tick_time = is_duration_v<T> || std::is_same_v<T, sys::time::ticks>;

}  // namespace time
}  // namespace sys

#endif  // COMPONENTS_SYS_TIME_HPP_