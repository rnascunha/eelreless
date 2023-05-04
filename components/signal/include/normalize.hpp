/**
 * @file normalize.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_SIGNAL_NORMALIZE_HPP_
#define COMPONENTS_SIGNAL_NORMALIZE_HPP_

#include <cmath>
#include <cassert>

#include <type_traits>
#include <algorithm>

namespace signal {

struct access {
  template<typename T>
  static constexpr auto
  get(const T& obj) noexcept {
    if constexpr (std::is_class_v<T>)
      return obj.value();
    else
      return obj;
  }
};

template<typename T>
T normalize(T value, T mini, T maxi) noexcept {
  static_assert(std::is_arithmetic_v<T>, 
                "Must be a arithmetic value");
  return value * mini / maxi;
}

template<typename Iter, typename Accessor = access>
auto mean(Iter begin, Iter end) noexcept {
  std::size_t size = 0;
  decltype(*begin) sum = 0;
  while (begin != end) {
    sum += Accessor::get(*begin);
    ++size;
    ++begin;
  }
  return sum / size;
}

template<typename Iter, typename T, typename Accessor = access>
void remove_constant(Iter begin, Iter end, T dc) noexcept {
  std::transform(begin, end, begin, [&dc](auto v) {
    return Accessor::get(v) - dc;
  });
}

template<typename Iter, typename Accessor = access>
void filter_first_order(Iter begin, Iter end, double weight) noexcept {
  assert(weight >= 0 && weight <= 1 && "Weight must be 0 <= weight <= 1");

  Iter next = begin++;
  while (next != end) {
    Accessor::get(*next) = weight * Accessor::get(*begin) + 
                           (1 - weight) * Accessor::get(*next);
    next = begin++;
  }
}

template<typename Iter, typename Accessor = access>
auto rms_sine_square(Iter begin, Iter end) noexcept {
  std::size_t size = 0;
  decltype(*begin) sum = 0;
  while (begin != end) {
    sum += Accessor::get(*begin) * Accessor(*begin);
    ++size;
    ++begin;
  }
  return sum / size;
}

template<typename Iter, typename Accessor = access>
auto rms_sine(Iter begin, Iter end) noexcept {
  return std::sqrt(rms_sine_square<Iter, Accessor>(begin, end));
}

}  // namespace signal

#endif  // COMPONENTS_SIGNAL_NORMALIZE_HPP_
