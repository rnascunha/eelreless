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

#define ATTR_PACKED __attribute__((packed, aligned(1)))

enum class command : std::uint8_t {
  config = 1,
  state,
  open_valve,
  close_valve,
  start_control_volume,
  data_control_volume,
  end_control_volume,
  //
    error = 10
};

enum class error_description : std::uint8_t {
  not_defined = 0,
  size_packet_not_match,
  argument_error,
  error_reading,
  command_not_found
};

enum class state : std::uint8_t {
  open = 0,
  close,
  error
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
  command cmd;
  state   st;
  int     count;
  double  volume;
} ATTR_PACKED;

struct open_valve_request {
  command cmd;
  bool    clear_count;
} ATTR_PACKED;

using close_valve_request = open_valve_request;

// struct start_control_volume_packet {
//   command       cmd;
//   bool          clear_count;
//   std::uint32_t volume_ml;
// } ATTR_PACKED;

// struct data_control_volume_packet {
//   command       cmd;
//   std::uint32_t volume_ml;
// } ATTR_PACKED;

// struct end_control_volume_packet {
//   command       cmd;
//   state         st;
//   std::uint32_t volume_ml;
// } ATTR_PACKED;

#endif  // MAIN_PACKETS_HPP_
