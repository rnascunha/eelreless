/**
 * @file func.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef MAIN_FUNC_HPP_
#define MAIN_FUNC_HPP_

#include <utility>

#include "sys/nvs.hpp"
#include "uc/gpio.hpp"

#define NVS_NAMESPACE   "nvs_login_wifi"
#define NVS_KEY_SSID    "ssid"
#define NVS_KEY_PASS    "pass"

#if CONFIG_ENABLE_MDNS == 1
void init_mdns() noexcept;
#endif  // CONFIG_ENABLE_MDNS == 1

void configure_wifi(sys::nvs& nvs) noexcept;

#endif  // MAIN_FUNC_HPP_