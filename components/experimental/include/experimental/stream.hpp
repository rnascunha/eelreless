/**
 * @file stream.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_SYS_STREAM_HPP_
#define COMPONENTS_SYS_STREAM_HPP_

#include <cstdio>
#include <utility>
#include <type_traits>

#include <string_view>
#include <span>

#include "sys/error.hpp"

namespace sys {

struct ostream {
  FILE* stream = stdout;
};

template<typename Stream>
static constexpr const bool
is_stream_v = std::is_base_of_v<ostream, Stream>;

/**
 * Strings
 */
template<typename Stream>
std::enable_if_t<is_stream_v<Stream>, Stream&&>
operator<<(Stream&& stream,
           const char* string) noexcept {
  std::fputs(string, stream.stream);
  return std::forward<Stream>(stream);
}

template<typename Stream>
std::enable_if_t<is_stream_v<Stream>, Stream&&>
operator<<(Stream&& stream,
           char c) noexcept {
  std::fputc(c, stream.stream);
  return std::forward<Stream>(stream);
}

template<typename Stream>
std::enable_if_t<is_stream_v<Stream>, Stream&&>
operator<<(Stream&& stream,
           const std::string_view& sv) noexcept {
  std::fprintf(stream.stream, "%.*s", sv.size(), sv.data());
  return std::forward<Stream>(stream);
}

/**
 * Numbers[integers]
 */
template<typename Stream>
std::enable_if_t<is_stream_v<Stream>, Stream&&>
operator<<(Stream&& stream,
           int number) noexcept {
  std::fprintf(stream.stream, "%d", number);
  return std::forward<Stream>(stream);
}

template<typename Stream>
std::enable_if_t<is_stream_v<Stream>, Stream&&>
operator<<(Stream&& stream,
           unsigned number) noexcept {
  std::fprintf(stream.stream, "%u", number);
  return std::forward<Stream>(stream);
}

/**
 * Numbers[float]
 */
template<typename Stream>
std::enable_if_t<is_stream_v<Stream>, Stream&&>
operator<<(Stream&& stream,
           float number) noexcept {
  std::fprintf(stream.stream, "%f", number);
  return std::forward<Stream>(stream);
}

template<typename Stream>
std::enable_if_t<is_stream_v<Stream>, Stream&&>
operator<<(Stream&& stream,
           double number) noexcept {
  std::fprintf(stream.stream, "%f", number);
  return std::forward<Stream>(stream);
}

/**
 * Span
 */
template<typename Stream, typename T>
std::enable_if_t<is_stream_v<Stream>, Stream&&>
operator<<(Stream&& stream,
           const std::span<T>& sp) noexcept {
  for (const auto& s : sp)
    stream << s << ' ';
  return std::forward<Stream>(stream);
}

/**
 * Others
 */
template<typename Stream>
std::enable_if_t<is_stream_v<Stream>, Stream&&>
operator<<(Stream&& stream, const error& err) noexcept {
  return stream << err.value() << ":" << err.message();
}

}  // namespace sys

#endif  // COMPONENTS_SYS_STREAM_HPP_
