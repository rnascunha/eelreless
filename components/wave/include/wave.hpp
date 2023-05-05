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
#ifndef COMPONENTS_WAVE_HPP_
#define COMPONENTS_WAVE_HPP_

#include <cmath>
#include <cassert>
#include <cstdint>

#include <iterator>
#include <type_traits>
#include <algorithm>

namespace wave {

template< class T >
struct remove_cvref {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template< class T >
using remove_cvref_t = typename remove_cvref<T>::type;

struct access {
  template<typename T>
  static constexpr auto&
  get(const T& obj) noexcept {
    if constexpr (std::is_class_v<T>)
      return obj.value();
    else
      return obj;
  }

  template<typename T>
  static constexpr auto&
  get(T& obj) noexcept {
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
  using value_type = remove_cvref_t<decltype(*begin)>;
  value_type sum = 0;
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

  Iter before = begin++;
  while (begin != end) {
    Accessor::get(*begin) = weight * Accessor::get(*before) + 
                           (1 - weight) * Accessor::get(*begin);
    before = begin++;
  }
}

template<typename Iter, typename Accessor = access>
auto rms_sine_square(Iter begin, Iter end) noexcept {
  std::size_t size = 0;
  using value_type = remove_cvref_t<decltype(*begin)>;
  value_type sum = 0.0;
  while (begin != end) {
    sum += Accessor::get(*begin) * Accessor::get(*begin);
    ++size;
    ++begin;
  }
  return sum / size;
}

template<typename Iter, typename Accessor = access>
auto rms_sine(Iter begin, Iter end) noexcept {
  return std::sqrt(rms_sine_square<Iter, Accessor>(begin, end));
}

template<typename IterIn, typename IterOut, typename Accessor = access>
void convert(IterIn begin, IterIn end, IterOut out) {
    // Circuit polarization parameters for currente sensor
    double Vmax = 2.450;							// Maximum voltage ADC can read [V];
    double Vmin = 0.120;							// Minimum voltage ADC can read [V];                     
    int d_max = 2895;                              // Max decimal value correlated with Vmax on 12 bit range;
    // double GND  = 0.0;								// gnd for ADC converter [V];
    double Vdc = 3.3;								// Voltage divisor supply [V];
    double R1 = 180.0*1000;							// Voltage divisor top resistor [Ohms];
    double R2 = 120.0*1000;							// Voltage divisor bottom resistor [Ohms];
    double V_R2 = R2/(R1+R2)*Vdc;					// Voltage over R2 [V] or Vref for ADC converter [V];

    // double Rb1 = 0.0;								// Burden resistor (bias) [Ohms];
    double Rb2 = 120.0;								// Burden resistor (bias) [Ohms];
    double N1 = 1;									// Current transformer sensor ration parameters
    double N2 = 2000;								// Current transformer sensor ration parameters

    std::transform(begin, end, out, [&](auto v) {
        return (v * (Vmax - Vmin) / (d_max) + Vmin - V_R2) * (1 / Rb2) * (N2 / N1);
    });
}

}  // namespace wave

#endif  // COMPONENTS_WAVE_HPP_
