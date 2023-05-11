/**
 * @file expected.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_SYS_EXPECETED_HPP_
#define COMPONENTS_SYS_EXPECETED_HPP_

#include <variant>

#include "sys/error.hpp"

namespace sys {

template<typename E>
struct unexpected {
  template<typename Err = E>
  unexpected(Err e) : error(e){}
  E error;
};

template<typename E>
unexpected(E e) -> unexpected<E>;

template<typename T,
         typename E = error>
class expected {
 public:
  constexpr
  expected(T&& val)
   : val_(val) {}

  constexpr
  expected(unexpected<E>&& err)
   : val_(err.error) {}

  constexpr
  expected& operator=(T&& t) noexcept {
    val_ = t;
    return *this;
  }

  constexpr
  expected& operator=(unexpected<E>&& e) noexcept {
    val_ = e;
    return *this;
  }

  constexpr
  T operator*() const {
    return value();
  }

  constexpr
  bool has_value() const {
    return std::holds_alternative<T>(val_);
  }

  constexpr
  const T& value() const {
    return std::get<T>(val_);
  }

  constexpr
   T& value() {
    return std::get<T>(val_);
  }

  constexpr
  E error() const {
    return std::get<E>(val_);
  }

  operator bool() const noexcept {
    return has_value();
  }

 private:
  std::variant<T, E> val_;
};

}  // namespace sys

#endif  // COMPONENTS_SYS_EXPECETED_HPP_