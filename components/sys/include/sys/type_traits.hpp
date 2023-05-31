/**
 * @file type_traits.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_SYS_TYPE_TRAITS_HPP_
#define COMPONENTS_SYS_TYPE_TRAITS_HPP_

#include <type_traits>
#include <chrono>

namespace sys {

template<class T>
struct is_duration : std::false_type{};

template<class Rep, class Period>
struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type{};

template<typename T>
static constexpr bool
is_duration_v = is_duration<T>::value;

}  // namespace sys

#endif  // COMPONENTS_SYS_TYPE_TRAITS_HPP_
