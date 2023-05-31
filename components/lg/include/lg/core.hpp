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

#include <cstdio>
#include <cstdint>
#include <utility>
#include <string_view>

#include "esp_system.h"

#define FMT_THROW(x)  esp_system_abort(x.what())

#include "fmt/format.h"
#include "fmt/color.h"

#include "esp_log.h"

namespace lg {

/**
 * https://github.com/fmtlib/fmt/issues/1617
 */
template <typename T>
struct basic_print_output_iterator {
  using value_type = T;
  using iterator_category = std::output_iterator_tag;
  using difference_type = void;
  using pointer = void;
  using reference = void;
  using _Unchecked_type = basic_print_output_iterator; // Mark iterator as checked.

  basic_print_output_iterator& operator*() { return *this; }
  basic_print_output_iterator& operator++() { return *this; }
  basic_print_output_iterator& operator++(int) { return *this; }
  basic_print_output_iterator& operator=(const T& value) {
    std::fputc(value, stdout);
    return *this;
  }
};

using print_output_iterator = basic_print_output_iterator<char>;

template <typename... Args>
constexpr auto
print(fmt::format_string<Args...> fmt, Args&&... args) {
  return fmt::format_to(
            print_output_iterator{},
            fmt, std::forward<Args>(args)...);
}

template <typename S, typename... Args>
constexpr auto
print(const fmt::text_style& ts,  S& fmt, Args&&... args) {
  return fmt::format_to(
            print_output_iterator{},
            ts, fmt, std::forward<Args>(args)...);
}

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

static constexpr const
char* end_color = "" LOG_RESET_COLOR;

#ifdef CONFIG_LOG_COLORS
#define CONFIG_LG_USE_COLOR    true
#else
#define CONFIG_LG_USE_COLOR    false
#endif 

template<bool BreakLine = true,
         bool UseColor = CONFIG_LG_USE_COLOR,
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
out(std::string_view tag,
      fmt::format_string<T...> fmt,
      T&& ...args) {
  if constexpr (Config::force || Level::level <= LOG_LOCAL_LEVEL) {
    if constexpr (Config::color)
      lg::print("{}", Level::color);
    lg::print("{} ({}) {}:",
              Level::letter,
              Config::time::time(),
              tag);
    lg::print(fmt,
              std::forward<T>(args)...);
    lg::print("{}", end_color);
    if constexpr (Config::break_line)
      lg::print("\n");
  }
}

#define LOG_FUNC_MAKE(name)                   \
template<typename Config = default_config,    \
         typename ...T>                       \
constexpr void                                \
name (std::string_view tag,                   \
      fmt::format_string<T...> fmt,           \
      T&& ...args) {                          \
  out< name ## _level, Config, T...>(         \
        tag, fmt, std::forward<T>(args)...);  \
}

#define LOG_FUNC_EMPTY(name)                  \
template<typename ...Ts>                      \
constexpr void __attribute__((always_inline)) \
name (Ts&&...) {}

}  // namespace lg

#endif // COMPONENTS_LOG_CORE_HPP_
