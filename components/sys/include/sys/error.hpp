/**
 * @file error.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_SYS_ERROR_HPP_
#define COMPONENTS_SYS_ERROR_HPP_

#include <esp_err.h>

namespace sys {

class error {
 public:
  constexpr
  error() noexcept = default;
  constexpr
  error(esp_err_t err) noexcept
   : err_(err){}

  [[nodiscard]] constexpr esp_err_t
  value() const noexcept {
    return err_;
  }

  [[nodiscard]] constexpr
  operator bool() const noexcept {
    return err_ != ESP_OK;
  }

  [[nodiscard]] constexpr
  operator esp_err_t() const noexcept {
    return err_;
  }

  [[nodiscard]] const char*
  message() const noexcept {
    return esp_err_to_name(err_);
  }
 private:
    esp_err_t err_ = ESP_OK;
};

constexpr bool
operator==(const error& err0, esp_err_t err1) noexcept {
  return err0.value() == err1;
}

constexpr bool
operator==(esp_err_t err1, const error& err0) noexcept {
  return err0 == err1;
}

constexpr bool
operator!=(const error& err0, esp_err_t err1) noexcept {
  return err0.value() != err1;
}

}  // namespace sys

#include "lg/core.hpp"

template <>
struct fmt::formatter<sys::error> {
  bool value = true;
  bool message = false;

  constexpr auto
  parse(fmt::format_parse_context& ctx) -> fmt::format_parse_context::iterator {
    auto it = ctx.begin(), end = ctx.end();
    if (it == end) {
      return it;
    }

    value = false;
    while (it != end) {
      switch(*it) {
        case 'v':
          value = true;
          break;
        case 'm':
          message = true;
          break;
        case 'b':
          value = true;
          message = true;
          break;
        default:
          // ctx.on_error("invalid format");
          return it;  
      }
      ++it;
    }  
    return it;
  }
 
  auto format(const sys::error& err,
              fmt::format_context& ctx) const -> fmt::format_context::iterator{
    if (value && message)
      return fmt::format_to(ctx.out(), "{}:{}", err.value(), err.message());
    else if (value)
      return fmt::format_to(ctx.out(), "{}", err.value());
    return fmt::format_to(ctx.out(), "{}", err.message());
  }
};

#endif  // COMPONENTS_SYS_ERROR_HPP_
