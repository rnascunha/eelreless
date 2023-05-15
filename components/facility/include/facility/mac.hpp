/**
 * @file mac.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_FACILITY_MAC_HPP_
#define COMPONENTS_FACILITY_MAC_HPP_

#include <cstdint>
#include <cctype>
#include <cstdlib>
#include <cassert>

#include <string_view>
#include <optional>

namespace facility {

namespace detail {
  int hex_value(char c) {
    if (std::isdigit(c))
      return c - '0';
    if (c >= 'a' && c <= 'f')
      return 10 + (c - 'a');
    return 10 + (c - 'A');
  }
}  // namespace detail

struct mac {
  std::uint8_t f[6]{};

  constexpr
  std::uint8_t& operator[](int i) noexcept {
      assert(i >= 0 && i <= 5 && "Out of bound");
      return f[i];
  }

  constexpr
  std::uint8_t operator[](int i) const noexcept {
      assert(i >= 0 && i <= 5 && "Out of bound");
      return f[i];
  }
};

[[nodiscard]] constexpr std::optional<mac>
to_mac(std::string_view addr) {
  mac ip;
  int idx = 0;
  int curr_idx = 0;
  for (char c : addr) {
    if (curr_idx == 0) {
      if (!std::isxdigit(c))
        return std::nullopt;
      ip[idx] = detail::hex_value(c);
      curr_idx = 1;
      continue;
    }
    if (!std::isxdigit(c)) {
      if (c != ':')
        return std::nullopt;
      if (curr_idx == 0)
        return std::nullopt;
      curr_idx = 0;
      if (++idx > 5)
        return std::nullopt;
      continue;
    }
    if (++curr_idx == 3)
      return std::nullopt;
    int val = 16 * ip[idx] + detail::hex_value(c);
    if (val > 255)
      return std::nullopt;
    ip[idx] = val;
  }
  if (idx != 5 || curr_idx == 0)
   return std::nullopt;
  return ip;
}

namespace literals {

constexpr mac
operator ""_mac(const char* addr, std::size_t size) noexcept {
    auto mac = to_mac({addr, size});
    if (!mac)
        std::abort();
    return *mac;
}

}  // namespace literals

}  // namespace facility

#endif  // COMPONENTS_FACILITY_MAC_HPP_