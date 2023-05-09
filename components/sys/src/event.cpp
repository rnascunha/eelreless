/**
 * @file event.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <cstdint>

#include "esp_event.h"
#include "sys/event.hpp"

namespace sys{
namespace event {

sys::error register_handler(esp_event_base_t base,
              std::int32_t id,
              esp_event_handler_t handler,
              void* arg /* = nullptr */) noexcept {
  return esp_event_handler_register(base,
                                    id,
                                    handler,
                                    arg);
}

sys::error register_handler(esp_event_base_t base,
              std::int32_t id,
              esp_event_handler_instance_t& instance,
              esp_event_handler_t handler,
              void* arg /* = nullptr */) noexcept {
  return esp_event_handler_instance_register(base,
                                    id,
                                    handler,
                                    arg,
                                    &instance);
}

sys::error unregister_handler(esp_event_base_t base,
              std::int32_t id,
              esp_event_handler_t handler) noexcept {
  return esp_event_handler_unregister(base,
                                      id,
                                      handler);
}

sys::error unregister_handler(esp_event_base_t base,
              std::int32_t id,
              esp_event_handler_instance_t instance) noexcept {
  return esp_event_handler_instance_unregister(base,
                                              id,
                                              instance);
}

}  // namespace event
}  // namespace sys