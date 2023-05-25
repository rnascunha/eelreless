/**
 * @file format_types.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_LG_FORMAT_TYPES_HPP_
#define COMPONENTS_LG_FORMAT_TYPES_HPP_

#include "lg/core.hpp"

#include "esp_netif_types.h"

template <>
struct fmt::formatter<esp_ip4_addr_t> {
  constexpr auto
  parse(fmt::format_parse_context& ctx) -> fmt::format_parse_context::iterator {
    return ctx.end();
  }
 
  auto format(const esp_ip4_addr_t& ip,
              fmt::format_context& ctx) const -> fmt::format_context::iterator{
    return fmt::format_to(ctx.out(), "{}.{}.{}.{}", ip.addr & 0xFF,
                                                    ip.addr >> 8 & 0xFF,
                                                    ip.addr >> 16 & 0xFF,
                                                    ip.addr >> 24);
  }
};

#endif  // COMPONENTS_LG_FORMAT_TYPES_HPP_