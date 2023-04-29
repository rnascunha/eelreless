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
#include "esp_event.h"
#include "sys/event.hpp"

namespace sys{
namespace event {

esp_err_t register_handler(esp_event_base_t base,
              int32_t id,
              esp_event_handler_t handler,
              void* arg /* = nullptr */) noexcept {
  return esp_event_handler_register(base,
                                    id,
                                    handler,
                                    arg);
}

esp_err_t register_handler(esp_event_base_t base,
              int32_t id,
              esp_event_handler_instance_t& instance,
              esp_event_handler_t handler,
              void* arg /* = nullptr */) noexcept {
  return esp_event_handler_instance_register(base,
                                    id,
                                    handler,
                                    arg,
                                    &instance);
}

}  // namespace event
}  // namespace sys