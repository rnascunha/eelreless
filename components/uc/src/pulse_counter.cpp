/**
 * @file pulse_counter.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <cassert>
#include <optional>

#include "driver/pulse_cnt.h"

#include "sys/error.hpp"
#include "uc/pulse_counter.hpp"

namespace uc {

pulse_counter::pulse_counter(const pcnt_unit_config_t& config) noexcept {
  auto err = pcnt_new_unit(&config, &unit_);
  if (err)
    unit_ = nullptr;
}

pulse_counter::~pulse_counter() noexcept {
  if (unit_ != nullptr) {
    stop();
    disable();
    erase();
  }
}

[[nodiscard]] pcnt_unit_handle_t
pulse_counter::handler() noexcept {
  return unit_;
}

pcnt_unit_handle_t
pulse_counter::release() noexcept {
  auto unit = unit_;
  unit_ = nullptr;
  return unit;
}

sys::error
pulse_counter::enable() noexcept {
  assert(unit_ != nullptr && "pulse_counter is null");
  return pcnt_unit_enable(unit_);
}

sys::error
pulse_counter::disable() noexcept {
  assert(unit_ != nullptr && "pulse_counter is null");
  return pcnt_unit_disable(unit_);
}

sys::error
pulse_counter::start() noexcept {
  assert(unit_ != nullptr && "pulse_counter is null");
 return pcnt_unit_start(unit_);
}

sys::error
pulse_counter::stop() noexcept {
  assert(unit_ != nullptr && "pulse_counter is null");
  return pcnt_unit_stop(unit_);
}

sys::error
pulse_counter::erase() noexcept {
  assert(unit_ != nullptr && "pulse_counter is null");
  auto err = pcnt_del_unit(unit_);
  if (!err) unit_ = nullptr;
  return err;
}

sys::error
pulse_counter::glitch_filter(const pcnt_glitch_filter_config_t& filter) noexcept {
  assert(unit_ != nullptr && "pulse_counter is null");
  return pcnt_unit_set_glitch_filter(unit_, &filter);
}

std::optional<int>
pulse_counter::count() const noexcept {
  assert(unit_ != nullptr && "pulse_counter is null");
  int val;
  auto err = pcnt_unit_get_count(unit_, &val);
  if (err != ESP_OK)
    return std::nullopt;
  return val;
}

sys::error
pulse_counter::clear() const noexcept {
  assert(unit_ != nullptr && "pulse_counter is null");
  return pcnt_unit_clear_count(unit_);
}

sys::error
pulse_counter::add_watch_point(int wpoint) noexcept {
  assert(unit_ != nullptr && "pulse_counter is null");
  return pcnt_unit_add_watch_point(unit_, wpoint);
}

sys::error
pulse_counter::remove_watch_point(int wpoint) noexcept {
  assert(unit_ != nullptr && "pulse_counter is null");
  return pcnt_unit_remove_watch_point(unit_, wpoint);
}

sys::error
pulse_counter::register_callback(const pcnt_event_callbacks_t& cb,
                                 void* arg /* = nullptr */) noexcept {
  assert(unit_ != nullptr && "pulse_counter is null");
  return pcnt_unit_register_event_callbacks(unit_, &cb, arg);
}

/**
 * Channel
 */
pulse_counter::channel::channel(pulse_counter& cnt, const pcnt_chan_config_t& config) noexcept {
  auto err = pcnt_new_channel(cnt.handler(), &config, &channel_);
  if (err)
    channel_ = nullptr;
}

pulse_counter::channel::~channel() noexcept {
  if (channel_ != nullptr) erase();
}

pcnt_channel_handle_t
pulse_counter::channel::handler() noexcept {
  return channel_;
}

pcnt_channel_handle_t
pulse_counter::channel::release() noexcept {
  auto channel = channel_;
  channel_ = nullptr;
  return channel;
}

sys::error
pulse_counter::channel::edge_action(pcnt_channel_edge_action_t positive,
                                    pcnt_channel_edge_action_t negative) noexcept {
  assert(channel_ != nullptr && "channel is null");
  return pcnt_channel_set_edge_action(channel_, positive, negative);
}

sys::error
pulse_counter::channel::level_action(pcnt_channel_level_action_t high,
                                     pcnt_channel_level_action_t low) noexcept {
  assert(channel_ != nullptr && "channel is null");
  return pcnt_channel_set_level_action(channel_, high, low);
}

sys::error
pulse_counter::channel::erase() noexcept {
  assert(channel_ != nullptr && "channel is null");
  auto err = pcnt_del_channel(channel_);
  if (!err) channel_ = nullptr;
  return err;
}


}  // namespace uc
