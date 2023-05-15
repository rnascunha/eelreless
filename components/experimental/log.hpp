/**
 * @file log.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_SYS_LOG_HPP_
#define COMPONENTS_SYS_LOG_HPP_

#include <cstdio>
#include <inttypes.h>

#include "esp_log.h"

#include "sys/stream.hpp"

namespace sys {

struct log{};

struct info : log {
  static constexpr const char* letter = "I";
  static constexpr const char* color = "" LOG_COLOR_I;
  static constexpr const int level = 3;
  const char* tag = "";
};

struct LogHelper : ostream {
  ~LogHelper() {
    std::fputs("\n" LOG_RESET_COLOR, stream);
  }
};

template<typename Log, typename T>
std::enable_if_t<std::is_base_of_v<log, Log>, LogHelper>
operator<<(Log&& stream, T&& arg) noexcept {
  std::fputs(Log::color, stdout);
  std::fputs(Log::letter, stdout);
  std::printf(" (%" PRIu32 ") %s:", esp_log_timestamp(), stream.tag);
  return operator<<(LogHelper{}, std::forward<T>(arg));
}

}  // namespace sys

#endif  // COMPONENTS_SYS_LOG_HPP_
