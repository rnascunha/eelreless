/**
 * @file serial.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "driver/uart.h"

#include "sys/error.hpp"
#include "uc/serial.hpp"

namespace uc {

serial::serial(uart_port_t port) noexcept 
 : port_(port) {}
serial::serial(uart_port_t port,
               const config& cfg) noexcept 
 : port_(port) {
  init(cfg);
 }

sys::error
serial::init(const config& cfg) noexcept {
  auto c = (uart_config_t)cfg;
  return uart_param_config(port_, &c);
}

sys::error
serial::install_driver(int rx_buffer_size,
                      int tx_buffer_size /* = 0 */,
                      int queue_size /* = 0 */,
                      // QueueHandler_t* queue /* = nullptr */,
                      int intr_allic_flags /* = 0 */) noexcept {
  return uart_driver_install(port_,
                             rx_buffer_size,
                             tx_buffer_size,
                             queue_size,
                             nullptr, //queue,
                             intr_allic_flags);
}

}  // namespace uc
