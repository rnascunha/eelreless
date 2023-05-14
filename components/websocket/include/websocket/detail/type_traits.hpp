/**
 * @file type_traits.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_WEBSOCKET_DETAIL_TYPE_TRAITS_HPP_
#define COMPONENTS_WEBSOCKET_DETAIL_TYPE_TRAITS_HPP_

#include <type_traits>

#include "sys/error.hpp"

namespace websocket {

class request;

namespace detail {

template<class Func, class = void>
struct has_on_open : std::false_type{};

template<class Func>
struct has_on_open<Func,
                   std::enable_if_t<
                    std::is_invocable_r_v<
                      sys::error,
                      decltype(Func::on_open),
                      request>
                    >
                  > : std::true_type{};

template<class Func>
static constexpr const bool
has_on_open_v = has_on_open<Func>::value;

template<class Func, class = void>
struct has_on_data : std::false_type{};

template<class Func>
struct has_on_data<Func,
                   std::enable_if_t<
                    std::is_invocable_r_v<
                      sys::error,
                      decltype(Func::on_data),
                      request>
                    >
                  > : std::true_type{};

template<class Func>
static constexpr const bool
has_on_data_v = has_on_data<Func>::value;

template<class Func, class = void>
struct has_on_close : std::false_type{};

template<class Func>
struct has_on_close<Func,
                   std::enable_if_t<
                    std::is_invocable_v<
                      decltype(Func::on_close),
                      int, void*>
                    >
                  > : std::true_type{};

template<class Func>
static constexpr const bool
has_on_close_v = has_on_close<Func>::value;

}  // namespace detail
}  // namespace websocket

#endif  // COMPONENTS_WEBSOCKET_DETAIL_TYPE_TRAITS_HPP_
