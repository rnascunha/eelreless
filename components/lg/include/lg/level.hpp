/**
 * @file level.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_LOG_LEVEL_HPP_
#define COMPONENTS_LOG_LEVEL_HPP_

#include "esp_log.h"

#include "lg/core.hpp"

#include "fmt/color.h"

namespace lg {

#define LOG_LEVEL_MAKE(name, LETTER, COLOR, STYLE, LEVEL)   \
struct name ## _level {                                     \
  static constexpr const char letter = LETTER;              \
  static constexpr const char* color = "" COLOR;            \
  /* static constexpr const fmt::text_style style = STYLE;  */\
  static constexpr const int level = LEVEL;                 \
};

LOG_LEVEL_MAKE(verbose, 'V', LOG_COLOR_V, fmt::fg(fmt::terminal_color::white),  5)
LOG_LEVEL_MAKE(debug,   'D', LOG_COLOR_D, fmt::fg(fmt::terminal_color::white),  4)
LOG_LEVEL_MAKE(info,    'I', LOG_COLOR_I, fmt::fg(fmt::terminal_color::green),  3)
LOG_LEVEL_MAKE(warn,    'W', LOG_COLOR_W, fmt::fg(fmt::terminal_color::yellow), 2)
LOG_LEVEL_MAKE(error,   'E', LOG_COLOR_E, fmt::fg(fmt::terminal_color::red),    1)

LOG_FUNC_MAKE(verbose)
LOG_FUNC_MAKE(debug)
LOG_FUNC_MAKE(info)
LOG_FUNC_MAKE(warn)
LOG_FUNC_MAKE(error)

}  // namespace lg

#endif  // COMPONENTS_LOG_LEVEL_HPP_
