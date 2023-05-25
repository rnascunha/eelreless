/**
 * @file ip4.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_FACILITY_IP4_HPP_
#define COMPONENTS_FACILITY_IP4_HPP_

#include <cstdint>
#include <cctype>
#include <cstdlib>
#include <cassert>

#include <string_view>
#include <optional>

#include "lwip/ip4_addr.h"

namespace facility {

struct ip4 {
  std::uint8_t f[4]{};

  constexpr
  std::uint8_t& operator[](int i) noexcept {
      assert(i >= 0 && i <= 3 && "Out of bound");
      return f[i];
  }

  constexpr
  std::uint8_t operator[](int i) const noexcept {
      assert(i >= 0 && i <= 3 && "Out of bound");
      return f[i];
  }

  constexpr
  operator std::uint32_t () noexcept {
    return PP_HTONL(LWIP_MAKEU32(f[0], f[1], f[2], f[3]));
  }

  constexpr
  operator ip4_addr () noexcept {
    ip4_addr ip;
    IP4_ADDR(&ip, f[0], f[1], f[2], f[3]);
    return ip;
  }
};

[[nodiscard]] constexpr std::optional<ip4>
to_ip4(std::string_view addr) {
  ip4 ip;
  int idx = 0;
  int curr_idx = 0;
  for (char c : addr) {
    if (curr_idx == 0) {
      if (!std::isdigit(c))
        return std::nullopt;
      ip[idx] = c - '0';
      curr_idx = 1;
      continue;
    }
    if (!std::isdigit(c)) {
      if (c != '.')
        return std::nullopt;
      if (curr_idx == 0)
        return std::nullopt;
      curr_idx = 0;
      if (++idx > 3)
        return std::nullopt;
      continue;
    }
    if (++curr_idx == 4)
      return std::nullopt;
    int val = 10 * ip[idx] + (c - '0');
    if (val > 255)
      return std::nullopt;
    ip[idx] = val;
  }
  if (idx != 3 || curr_idx == 0)
   return std::nullopt;
  return ip;
}

namespace literals {

constexpr ip4
operator ""_ip4(const char* addr, std::size_t size) noexcept {
    auto ip = to_ip4({addr, size});
    if (!ip)
        std::abort();
    return *ip;
}

}  // namespace literals

}  // namespace facility

#include "lg/core.hpp"

template <>
struct fmt::formatter<facility::ip4> {
  constexpr auto
  parse(fmt::format_parse_context& ctx) -> fmt::format_parse_context::iterator {
    return ctx.begin();
  }
 
  auto format(const facility::ip4& ip,
              fmt::format_context& ctx) const -> fmt::format_context::iterator{
    return fmt::format_to(ctx.out(), "{}.{}.{}.{}", ip[0], ip[1], ip[2], ip[3]);
  }
};

#endif  // COMPONENTS_FACILITY_IP4_HPP_