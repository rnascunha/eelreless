/**
 * @file nvs.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_SYS_NVS_HPP_
#define COMPONENTS_SYS_NVS_HPP_

#include <cstdint>

#include "nvs.h"

#include "sys/error.hpp"

namespace sys {

class nvs {
 public:
  nvs() noexcept = default;
  nvs(const char*, nvs_open_mode_t = NVS_READWRITE) noexcept;

  error
  open(const char*, nvs_open_mode_t = NVS_READWRITE) noexcept;
  void
  close() noexcept;

  //Sets
  error
  set(const char*, int) noexcept;
  error
  set(const char*, std::int8_t) noexcept;
  error
  set(const char*, std::uint8_t) noexcept;
  error
  set(const char*, std::int16_t) noexcept;
  error
  set(const char*, std::uint16_t) noexcept;
  error
  set(const char*, std::int32_t) noexcept;
  error
  set(const char*, std::uint32_t) noexcept;
  error
  set(const char*, std::int64_t) noexcept;
  error
  set(const char*, std::uint64_t) noexcept;
  error
  set(const char*, const char*) noexcept;
  error
  set(const char*, const void*, std::size_t) noexcept;

  // Gets
  error
  get(const char*, std::int8_t&) noexcept;
  error
  get(const char*, std::uint8_t&) noexcept;
  error
  get(const char*, std::int16_t&) noexcept;
  error
  get(const char*, std::uint16_t&) noexcept;
  error
  get(const char*, std::int32_t&) noexcept;
  error
  get(const char*, std::uint32_t&) noexcept;
  error
  get(const char*, std::int64_t&) noexcept;
  error
  get(const char*, std::uint64_t&) noexcept;
  error
  get(const char*, char*, std::size_t&) noexcept;
  error
  get(const char*, void*, std::size_t&) noexcept;

  //
  error
  erase(const char*) noexcept;
  error
  erase_all() noexcept;
  error
  commit() noexcept;
  [[nodiscard]] nvs_handle_t
  native() noexcept;

 private:
  nvs_handle_t handler_ = 0;
};

}  // namespace sys

#endif  // COMPONENTS_SYS_NVS_HPP_