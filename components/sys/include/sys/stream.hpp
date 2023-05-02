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

namespace sys {

struct out_stream {
  FILE* out;
};

out_stream out{stdout};

out_stream&
operator<<(out_stream& stream,
                       const char* string) noexcept {
  std::fputs(string, stream.out);
  return stream;
}

out_stream&
operator<<(out_stream& stream,
                       int number) noexcept {
  std::fprintf(stream.out, "%d", number);
  return stream;
}

out_stream&
operator<<(out_stream& stream,
                       unsigned number) noexcept {
  std::fprintf(stream.out, "%u", number);
  return stream;
}

out_stream&
operator<<(out_stream& stream,
                       float number) noexcept {
  std::fprintf(stream.out, "%f", number);
  return stream;
}

out_stream&
operator<<(out_stream& stream,
                       double number) noexcept {
  std::fprintf(stream.out, "%f", number);
  return stream;
}

}  // namespace sys

#endif  // COMPONENTS_SYS_STREAM_HPP_