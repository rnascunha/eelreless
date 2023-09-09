/**
 * @file timer.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "sys/timer.hpp"

#include <cassert>

#include <chrono>

#include "esp_timer.h"

#include "sys/error.hpp"

namespace sys {

timer::timer(const esp_timer_create_args_t& args) noexcept {
  if(esp_timer_create(&args, &handler_))
    handler_ = nullptr;
}

timer::~timer() noexcept {
  if (handler_ != nullptr) erase();
}

sys::error timer::stop() noexcept {
  assert(handler_ != nullptr && "Time handler cant be NULL");
  return esp_timer_stop(handler_);
}

sys::error timer::erase() noexcept {
  assert(handler_ != nullptr && "Time handler cant be NULL");
  return esp_timer_delete(handler_);
}

}  // namespace sys