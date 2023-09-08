#include <chrono>

#include "esp_timer.h"

#include "sys/time.hpp"

[[nodiscard]] std::chrono::microseconds
uptime() noexcept {
  return std::chrono::microseconds(esp_timer_get_time());
}