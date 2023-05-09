/**
 * @file task_send.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <cstdio>
#include <chrono>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_netif_types.h"

#include "sys/sys.hpp"
#include "adc/continuous.hpp"

#include "adc.hpp"

static constexpr const char*
ADC_TAG = "ADC task";

struct data_adc_send {
  esp_netif_ip_info_t ip;
  uC::ADC_continuous* adc;
  int interval;
  bool& stop;
};

void task_send(void* arg) {
  data_adc_send* adc_ip = (data_adc_send*)arg;

  if(adc_ip->adc->start()) {
    ESP_LOGW(ADC_TAG, "Error starting ADC");
    return;
  }

  while (!adc_ip->stop) {
    uC::ADC_continuous::data data[EXAMPLE_READ_LEN]{};
    using namespace std::chrono_literals;
    auto result = adc_ip->adc->read(data, EXAMPLE_READ_LEN, sys::time::to_ticks(500ms));
    if (result) {
      if (!validate_data(data, result.readed)) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error validaint data");
      } else {
        char str[10]{};
        auto size = std::snprintf(str, 10, "%f", process_adc_data(data, result.readed));
        httpd_resp_send(req, str, size);
      }
    }
    sys::delay(std::chrono::seconds(adc_ip->interval));
  }
  adc_ip->adc->stop();
  vTaskDelete(NULL);
}