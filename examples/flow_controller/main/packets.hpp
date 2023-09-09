/**
 * @file packtes.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MAIN_PACKETS_HPP_
#define MAIN_PACKETS_HPP_

#include <cstdint>

#include "version.hpp"
#include "sys/error.hpp"

#include "websocket/server.hpp"
#include "control_flow.hpp"

#include "info.hpp"

#define ATTR_PACKED __attribute__((packed, aligned(1)))

enum class command : std::uint8_t {
  config = 1,
  state,
  open_valve,
  close_valve,
  //
  error = 10
};

enum class error_description : std::uint8_t {
  not_defined = 0,
  size_packet_not_match,
  argument_error,
  error_reading,
  command_not_found,
  already_opened,
  safe_timer_idle,
};

enum class state : std::uint8_t {
  open = 0,
  close
};

struct error_response {
  command           cmd;
  command           response_cmd;
  error_description err;
};

struct config_response {
  command               cmd;
  firmware_version_type version;
  std::int32_t          k_converter;
} ATTR_PACKED;

struct state_response {
  command        cmd;
  state          st;
  std::int32_t   count;
  std::int32_t   limit;
  std::int32_t   freq;
} ATTR_PACKED;

struct open_valve_request {
  command       cmd;
  bool          clear_count;
  std::int32_t  freq;   // 0
  std::int32_t  limit;  // 0
} ATTR_PACKED;


struct close_valve_request {
  command       cmd;
} ATTR_PACKED;

#undef ATTR_PACKED

sys::error send_error(websocket::request&,
                      command,
                      error_description) noexcept;
sys::error send_error(websocket::client&,
                      command,
                      error_description) noexcept;
sys::error send_state(websocket::request&) noexcept;
sys::error send_state(control_valve&) noexcept;
sys::error send_config(websocket::request&) noexcept;
sys::error send_open_valve(websocket::request&,
                           const websocket::data&) noexcept;
sys::error send_close_valve(websocket::request&) noexcept;

#endif  // MAIN_PACKETS_HPP_
