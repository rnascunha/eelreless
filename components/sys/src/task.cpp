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

}  // namespace sys
