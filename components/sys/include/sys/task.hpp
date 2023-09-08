/**
 * @file task.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_SYS_TASK_HPP__
#define COMPONENTS_SYS_TASK_HPP__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <cstdint>

#include "sys/time.hpp"

namespace sys {

using task_handle = TaskHandle_t;

/**
 * @see https://www.freertos.org/ulTaskNotifyTake.html
 */
std::uint32_t
notify_wait(time::tick_time auto duration, BaseType_t clear_on_exit = pdTRUE) noexcept {
  return ulTaskNotifyTake(clear_on_exit, time::to_ticks(duration));
}

/**
 * @see https://www.freertos.org/xTaskNotifyGive.html
 */
BaseType_t
notify(task_handle handle) noexcept;
BaseType_t
notify(task_handle handle, UBaseType_t index) noexcept;

/**
 * @see https://www.freertos.org/vTaskNotifyGiveFromISR.html 
*/
void
notify_from_ISR(task_handle handle,
                BaseType_t *pxHigherPriorityTaskWoken = nullptr) noexcept;
void
notify_from_ISR(task_handle handle,
                UBaseType_t index,
                BaseType_t *pxHigherPriorityTaskWoken /* = nullptr */) noexcept;


}  // namespace sys

#endif  // COMPONENTS_SYS_TASK_HPP__
