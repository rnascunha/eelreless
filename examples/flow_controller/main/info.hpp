/**
 * @file info.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef MAIN_INFO_HPP_
#define MAIN_INFO_HPP_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "control_flow.hpp"
#include "websocket/server.hpp"

struct control_valve {
  control_flow        control;
  QueueHandle_t       queue;
  websocket::client   client{};
  int freq            = 0;      // Seconds
  int volume          = 0;      // volume_ml
  int limit           = 0;      // miliiters
};

#endif // MAIN_INFO_HPP_
