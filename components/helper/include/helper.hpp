/**
 * @file helper.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_HELPER_HPP_
#define COMPONENTS_HELPER_HPP_

namespace helper {

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

}  // namespace

#endif  // COMPONENTS_HELPER_HPP_