/**
 * @file log.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_LOG_CORE_HPP_
#define COMPONENTS_LOG_CORE_HPP_

#include <cstdint>
#include <utility>
#include <string_view>

#include "fmt/core.h"

// #include "esp_log_internal.h"
#include "esp_log.h"

namespace lg {

struct timestamp {
  static
  std::uint32_t time() {
    return esp_log_timestamp();
  }
};

struct system_timestamp {
  static
  char* time() {
    return esp_log_system_timestamp();
  }
};

struct early_timestamp {
  static
  std::uint32_t time() {
    return esp_log_early_timestamp();
  }
};

static constexpr const char* end_color = "" LOG_RESET_COLOR;

template<bool BreakLine = true,
         bool UseColor = CONFIG_LOG_COLORS,
         typename TimeFunc = timestamp,
         bool Force    = false>
struct config {
  using time = TimeFunc;
  static constexpr const bool break_line  = BreakLine;
  static constexpr const bool color       = UseColor;
  static constexpr const bool force       = Force;
};

using default_config = config<>;

template<typename Level,
         typename Config = default_config,
         typename ...T>
constexpr void
print(std::string_view tag,
      fmt::format_string<T...> fmt,
      T&& ...args) {
  if constexpr (Config::force || Level::level <= LOG_LOCAL_LEVEL) {
    // if (!esp_log_impl_lock_timeout())
    //   return;
    // esp_log_level_t level_for_tag = s_log_level_get_and_unlock(tag);
    // if (!should_output(Level::level, level_for_tag))
    //   return;

    if constexpr (Config::color)
      fmt::print("{}", Level::color);
    fmt::print("{} ({}) {}:",
              Level::letter,
              Config::time::time(),
              tag);
    fmt::print(fmt,
              std::forward<T>(args)...);
    fmt::print("{}", end_color);
    if constexpr (Config::break_line)
      fmt::print("\n");
  }
}

#define LOG_FUNC_MAKE(name)                   \
template<typename Config = default_config,    \
         typename ...T>                       \
constexpr void                                \
name (std::string_view tag,                   \
      fmt::format_string<T...> fmt,           \
      T&& ...args) {                          \
  print< name ## _level, Config, T...>(       \
        tag, fmt, std::forward<T>(args)...);  \
}

#define LOG_FUNC_EMPTY(name)                  \
template<typename ...Ts>                      \
constexpr void __attribute__((always_inline)) \
name (Ts&&...) {}

}  // namespace lg

#endif // COMPONENTS_LOG_CORE_HPP_