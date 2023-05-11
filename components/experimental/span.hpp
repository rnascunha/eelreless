/**
 * @file span.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef SYS_SPAN_HPP_
#define SYS_SPAN_HPP_

#include <cstdlib>
#include <cstring>

namespace sys {

template<typename T>
class span {
 public:
  span() noexcept = default;
  span(const char* str) noexcept
   : data_(str), size_(std::strlen(str)) {}
  span(const T* data, std::size_t size) noexcept
   : data_(data), size_(size) {}
  template<std::size_t N>
  span(const T (&data)[N]) noexcept
   : data_(data), size_(N) {}
  template<typename C>
  span(const C& c) noexcept
   : data_(c.data()), size_(c.size()) {}

  const T* data() const noexcept {
    return data_;
  }
  std::size_t size() const noexcept {
    return size_;
  }

  std::size_t size_bytes() const noexcept {
    return size_ * sizeof(T);
  }

  bool empty() const noexcept {
    return size_ == 0;
  }

  const T* begin() const noexcept {
    return data_;
  }
  const T* end() const noexcept {
    return data_ + size_;
  }

  const T& operator[](std::size_t idx) {
    return data_[idx];
  }

 private:
  const T* data_;
  std::size_t size_;
};

}  // namespace sys

#endif  // SYS_SPAN_HPP_