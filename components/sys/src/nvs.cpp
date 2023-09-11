#include <cstdint>

#include "nvs.h"

#include "sys/error.hpp"
#include "sys/nvs.hpp"

namespace sys {

nvs::nvs(const char* ns,
         nvs_open_mode_t mode/* = NVS_READWRITE */) noexcept {
  open(ns, mode);
}

error
nvs::open(const char* ns, nvs_open_mode_t mode /* = NVS_READWRITE */) noexcept {
  return nvs_open(ns, mode, &handler_);
}

void
nvs::close() noexcept {
  nvs_close(handler_);
}

//Sets
error
nvs::set(const char* key, int value) noexcept {
  return nvs_set_i32(handler_, key, value);
}

error
nvs::set(const char* key, std::int8_t value) noexcept {
  return nvs_set_i8(handler_, key, value);
}

error
nvs::set(const char* key, std::uint8_t value) noexcept {
  return nvs_set_u8(handler_, key, value);
}

error
nvs::set(const char* key, std::int16_t value) noexcept {
  return nvs_set_i16(handler_, key, value);
}

error
nvs::set(const char* key, std::uint16_t value) noexcept {
  return nvs_set_u16(handler_, key, value);
}

error
nvs::set(const char* key, std::int32_t value) noexcept{
  return nvs_set_i32(handler_, key, value);
}

error
nvs::set(const char* key, std::uint32_t value) noexcept{
  return nvs_set_u32(handler_, key, value);
}

error
nvs::set(const char* key, std::int64_t value) noexcept{
  return nvs_set_i64(handler_, key, value);
}

error
nvs::set(const char* key, std::uint64_t value) noexcept{
  return nvs_set_u64(handler_, key, value);
}

error
nvs::set(const char* key, const char* value) noexcept{
  return nvs_set_str(handler_, key, value);
}

error
nvs::set(const char* key, const void* value, std::size_t size) noexcept{
  return nvs_set_blob(handler_, key, value, size);
}

// Gets
error
nvs::get(const char* key, std::int8_t& value) noexcept {
  return nvs_get_i8(handler_, key, &value);
}

error
nvs::get(const char* key, std::uint8_t& value) noexcept {
  return nvs_get_u8(handler_, key, &value);
}

error
nvs::get(const char* key, std::int16_t& value) noexcept {
  return nvs_get_i16(handler_, key, &value);
}

error
nvs::get(const char* key, std::uint16_t& value) noexcept {
  return nvs_get_u16(handler_, key, &value);
}

error
nvs::get(const char* key, std::int32_t& value) noexcept {
  return nvs_get_i32(handler_, key, &value);
}

error
nvs::get(const char* key, std::uint32_t& value) noexcept {
  return nvs_get_u32(handler_, key, &value);
}

error
nvs::get(const char* key, std::int64_t& value) noexcept {
  return nvs_get_i64(handler_, key, &value);
}

error
nvs::get(const char* key, std::uint64_t& value) noexcept {
  return nvs_get_u64(handler_, key, &value);
}

error
nvs::get(const char* key, char* value, std::size_t& size) noexcept {
  return nvs_get_str(handler_, key, value, &size);
}

error
nvs::get(const char* key, void* value, std::size_t& size) noexcept {
  return nvs_get_blob(handler_, key, value, &size);
}

//
error
nvs::erase(const char* key) noexcept {
  return nvs_erase_key(handler_, key);
}

error
nvs::erase_all() noexcept {
  return nvs_erase_all(handler_);
}

error
nvs::commit() noexcept {
  return nvs_commit(handler_);
}

[[nodiscard]] nvs_handle_t
nvs::native() noexcept {
  return handler_;
}

}  // namespace sys