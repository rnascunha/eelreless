/**
 * @file sys.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_SYS_SYS_HPP_
#define COMPONENTS_SYS_SYS_HPP_

#include "esp_err.h"

namespace sys {

esp_err_t default_net_init() noexcept;

}  // namespace sys

#endif  // COMPONENTS_SYS_SYS_HPP_