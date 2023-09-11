/**
 * @file water_flow.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <cassert>

#include "peripherals/water_flow.hpp"


water_flow_sensor::water_flow_sensor(gpio_num_t pin, int k) noexcept
  : pc_({
    .low_limit = -1,
    .high_limit = uc::pulse_counter::high_limit,
    .flags = {
      .accum_count = 1
    }
  }), 
  channel_(pc_, {
    .edge_gpio_num = pin,
    .level_gpio_num = uc::pulse_counter::unused,
    .flags = {}
  }), 
  k_(k) {
  pc_.add_watch_point(uc::pulse_counter::high_limit);
  channel_.edge_action(PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD);
}

sys::error
water_flow_sensor::enable() noexcept {
  return pc_.enable();
}
sys::error
water_flow_sensor::disable() noexcept {
  return pc_.disable();
}

sys::error
water_flow_sensor::start() noexcept {
  return pc_.start();
}

sys::error
water_flow_sensor::stop() noexcept {
  return pc_.stop();
}

[[nodiscard]] std::optional<int>
water_flow_sensor::count() noexcept {
  return pc_.count();
}

sys::error water_flow_sensor::clear() noexcept {
  return pc_.clear();
}

int
water_flow_sensor::k_ratio() const noexcept {
  return k_;
}

void water_flow_sensor::k_ratio(int k) noexcept {
  assert(k > 0 && "K value must be graeter than 0");
  k_ = k;
}

sys::error
water_flow_sensor::remove_callback(int wpoint) noexcept {
  return pc_.remove_watch_point(wpoint);
}