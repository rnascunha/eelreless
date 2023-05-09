/**
 * @file event.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef SYSTEM_EVENT_HPP_
#define SYSTEM_EVENT_HPP_

#include <cstdint>

#include "esp_event.h"

#include "sys/error.hpp"

namespace sys {
namespace event {

sys::error register_handler(esp_event_base_t base,
              std::int32_t id,
              esp_event_handler_t handler,
              void* arg = nullptr) noexcept;
sys::error register_handler(esp_event_base_t base,
              std::int32_t id,
              esp_event_handler_instance_t& instance,
              esp_event_handler_t handler,
              void* arg = nullptr) noexcept;

sys::error unregister_handler(esp_event_base_t base,
              std::int32_t id,
              esp_event_handler_t handler) noexcept;
sys::error unregister_handler(esp_event_base_t base,
              std::int32_t id,
              esp_event_handler_instance_t handler) noexcept;

}  // namespace event
}  // namespace sys

#endif  // SYSTEM_EVENT__HPP_
