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

#include <array>
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

using mac = std::array<std::uint8_t, 6>;

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

#include "lg/core.hpp"

template <>
struct fmt::formatter<facility::mac> {
  bool uppercase = false;

  constexpr auto
  parse(fmt::format_parse_context& ctx) -> fmt::format_parse_context::iterator {
    auto it = ctx.begin();
    if (it == ctx.end()) {
      return it;
    }

    if (*it == 'X')
      uppercase = true;
    return ++it;
  }
  
  auto format(const facility::mac& mac,
              fmt::format_context& ctx) const -> fmt::format_context::iterator 
              {
    return uppercase ? 
              fmt::format_to(ctx.out(), "{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}",
                                          mac[0], mac[1], mac[2],
                                          mac[3], mac[4], mac[5]) :
              fmt::format_to(ctx.out(), "{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
                                          mac[0], mac[1], mac[2],
                                          mac[3], mac[4], mac[5]);
  }
};

#endif  // COMPONENTS_FACILITY_MAC_HPP_