/**
 * @file log.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <inttypes.h>
#include <cstdint>
#include <ctime>
#include "../stream.hpp"

std::uint32_t esp_log_timestamp() {
  return std::time(NULL);
}

namespace sys {

struct log{};

#define LOG_COLOR_BLACK   "30"
#define LOG_COLOR_RED     "31"
#define LOG_COLOR_GREEN   "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR_BLUE    "34"
#define LOG_COLOR_PURPLE  "35"
#define LOG_COLOR_CYAN    "36"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I       LOG_COLOR(LOG_COLOR_GREEN)
#define LOG_COLOR_D
#define LOG_COLOR_V

struct info : log {
  constexpr
  info(const char* tag = "") noexcept
   : tag(tag){}

  const char* tag;
  static constexpr const char* letter = "I";
  static constexpr const char* color = "" LOG_COLOR_I;
  static constexpr const int level = 3;
};

struct erro : log {
  constexpr
  erro(const char* tag = "") noexcept
   : tag(tag){}

  const char* tag;
  static constexpr const char* letter = "E";
  static constexpr const char* color = "" LOG_COLOR_E;
  static constexpr const int level = 0;
};

struct LogHelper : ostream {
  constexpr
  LogHelper() = default;
  constexpr
  LogHelper(LogHelper&& h)
   : f(true) {}

  bool f = false;
  ~LogHelper() {
    if (f)
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


int main() {
  sys::info("MyTag") << "test" << " " << "Teste" << "New teste" << 5 << " " << 1.4 <<  " " << 'c';
  sys::info{} << "test2";
  sys::erro{"ERROR"} << "First error";
}