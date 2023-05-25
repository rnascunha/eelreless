/**
 * @file log.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_LOG_LOG_HPP_
#define COMPONENTS_LOG_LOG_HPP_

#include <utility>
#include <string_view>

#include "lg/level.hpp"

namespace lg {

#define LOG_METHOD_MAKE(name)                     \
template<typename Config = config_type,           \
         typename ...Ts>                          \
constexpr void                                    \
name(fmt::format_string<Ts...> fmt,               \
      Ts&&... args) const {                       \
  lg::name<Config, Ts...>(tag_,                   \
                          fmt,                    \
                          std::forward<Ts>(args)...);  \
} 

template<typename ClassConfig = default_config>
class log {
 public:
  using config_type = ClassConfig;

  constexpr
  log(std::string_view tag)
   : tag_(tag) {}
  
  LOG_METHOD_MAKE(verbose)
  LOG_METHOD_MAKE(debug)
  LOG_METHOD_MAKE(info)
  LOG_METHOD_MAKE(warn)
  LOG_METHOD_MAKE(error)

  [[nodiscard]] constexpr std::string_view
  tag() const noexcept {
    return tag_;
  }
 private:
  std::string_view tag_;
};

}  // namespace lg

#endif  // COMPONENTS_LOG_LOG_HPP_