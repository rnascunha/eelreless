/**
 * @file task.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sys/task.hpp"

namespace sys {

BaseType_t
notify(task_handle handle) noexcept {
  return xTaskNotifyGive(handle);
}

BaseType_t
notify(task_handle handle, UBaseType_t index) noexcept {
  return xTaskNotifyGiveIndexed(handle, index);
}

void
notify_from_ISR(task_handle handle,
                BaseType_t *pxHigherPriorityTaskWoken /* = nullptr */) noexcept {
  vTaskNotifyGiveFromISR(handle, pxHigherPriorityTaskWoken);
}

void
notify_from_ISR(task_handle handle,
                UBaseType_t index,
                BaseType_t *pxHigherPriorityTaskWoken /* = nullptr */) noexcept {
  vTaskNotifyGiveIndexedFromISR(handle, index, pxHigherPriorityTaskWoken);
}

/**
 * Event group
 * 
 */
event_group::event_group() noexcept
 : handler_(xEventGroupCreate()) {}

event_group::event_group(StaticEventGroup_t* buffer) noexcept
 : handler_(xEventGroupCreateStatic(buffer)) {}

event_group::~event_group() noexcept {
  if (handler_ != nullptr) erase();
}

void event_group::erase() noexcept {
  assert(handler_ != nullptr && "EventGroup handler cant be NULL");
  vEventGroupDelete(handler_);
  handler_ = nullptr;
}

auto event_group::set(const bits bits_to_set) noexcept -> bits {
  assert(handler_ != nullptr && "EventGroup handler cant be NULL");
  return xEventGroupSetBits(handler_, bits_to_set);
}

BaseType_t
event_group::set_from_ISR(const bits bits_to_set,
                          BaseType_t *pxHigherPriorityTaskWoken /* nullptr */) noexcept {
  assert(handler_ != nullptr && "EventGroup handler cant be NULL");
  return xEventGroupSetBitsFromISR(handler_, bits_to_set, pxHigherPriorityTaskWoken);
}

auto event_group::get() noexcept -> bits {
  assert(handler_ != nullptr && "EventGroup handler cant be NULL");
  return xEventGroupGetBits(handler_);
}

auto event_group::get_from_ISR() noexcept -> bits {
  assert(handler_ != nullptr && "EventGroup handler cant be NULL");
  return xEventGroupGetBitsFromISR(handler_);
}

auto event_group::clear(const bits bits_to_clear) noexcept -> bits {
  assert(handler_ != nullptr && "EventGroup handler cant be NULL");
  return xEventGroupClearBits(handler_, bits_to_clear);
}

BaseType_t event_group::clear_from_ISR(const bits bits_to_clear) noexcept {
  assert(handler_ != nullptr && "EventGroup handler cant be NULL");
  return xEventGroupClearBitsFromISR(handler_, bits_to_clear);
}

bool
event_group::is_set(const bits checked, const bits check) noexcept {
  return (checked & check) != 0;
}

}  // namespace sys
