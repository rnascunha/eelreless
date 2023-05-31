/**
 * @file serial.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_UC_SERIAL_HPP_
#define COMPONENTS_UC_SERIAL_HPP_

#include <cstdint>
#include <span>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "driver/uart.h"

#include "sys/error.hpp"
#include "sys/time.hpp"

namespace uc {

class serial {
 public:
  struct config {
    int                   baud_rate;
    uart_word_length_t    data_bits = UART_DATA_8_BITS;
    uart_parity_t         parity    = UART_PARITY_DISABLE;
    uart_stop_bits_t      stop_bits = UART_STOP_BITS_1;
    uart_hw_flowcontrol_t flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    std::uint8_t          rx_flow_ctrl_thresh = 122;
    uart_sclk_t           source_clk = UART_SCLK_DEFAULT;

    constexpr
    operator uart_config_t() const noexcept {
      return {
        .baud_rate = baud_rate,
        .data_bits = data_bits,
        .parity    = parity,
        .stop_bits = stop_bits,
        .flow_ctrl = flow_ctrl,
        .rx_flow_ctrl_thresh = rx_flow_ctrl_thresh,
        .source_clk = source_clk,
      };
    }
  };

  serial(uart_port_t) noexcept;
  serial(uart_port_t, const config&) noexcept;

  sys::error init(const config&) noexcept;
  sys::error install_driver(int rx_buffer_size,
                            int tx_buffer_size = 0,
                            int queue_size = 0,
                            // QueueHandle_t* = nullptr,
                            int intr_allic_flags = 0) noexcept;
  
  template<typename T>
  int read(std::span<T> buffer, sys::time::tick_time auto wait) noexcept {
    return uart_read_bytes(port_,
                           buffer.data(), buffer.size_bytes(),
                           sys::time::to_ticks(wait));
  }

  template<typename T>
  int write(std::span<T> buffer) noexcept {
    return uart_write_bytes(port_,
                           (const void*)buffer.data(),
                            buffer.size_bytes());
  }

 private:
  uart_port_t port_;
};

}  // namespace uc

#endif  // COMPONENTS_UC_SERIAL_HPP_
