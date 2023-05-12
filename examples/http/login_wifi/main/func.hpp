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
#include "gpio.hpp"

#define NVS_NAMESPACE   "nvs_login_wifi"
#define NVS_KEY_SSID    "ssid"
#define NVS_KEY_PASS    "pass"

#if CONFIG_ENABLE_MDNS == 1
void init_mdns() noexcept;
#endif  // CONFIG_ENABLE_MDNS == 1

struct debouce {
  debouce(mcu::gpio&& btn, int lmt) noexcept
   : button(btn), limit(lmt) {}

  void reset() noexcept {
    count = 0;
  }

  template<typename Duration,
           typename Callable,
           typename ...Args>
  void wait(Duration&& dt, Callable&& call, Args&&... args) noexcept {
    while(true) {
      if (button.read() == 0) {
        if (++count == limit) {
          call(std::forward<Args>(args)...);
          break;
        }
      } else {
        count = 0;
      }
      sys::delay(dt);
    }
  }

  mcu::gpio button;
  int limit;
 private:
  int count = 0;
};

void configure_wifi(sys::nvs& nvs) noexcept;

#endif  // MAIN_FUNC_HPP_