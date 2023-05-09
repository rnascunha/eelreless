/**
 * @file resources.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <cstdio>

#include "esp_err.h"
#include "esp_http_server.h"

#include "http/server.hpp"
#include "adc/continuous.hpp"
#include "adc.hpp"

#include "sys/span.hpp"

static esp_err_t
current_get_handler(httpd_req_t *request) {
  uC::ADC_continuous* adc = (uC::ADC_continuous*) request->user_ctx;
  http::server::request req(request);

  req.allow_cors();

  if (adc->start()) {
    req.send_error(HTTPD_500_INTERNAL_SERVER_ERROR, "Error starting adc");
    return ESP_OK;
  }

  uC::ADC_continuous::data data[EXAMPLE_READ_LEN]{};
  using namespace std::chrono_literals;
  auto result = adc->read(data, EXAMPLE_READ_LEN, sys::time::to_ticks(500ms));
  if (result) {
    if (!validate_data(data, result.readed)) {
      req.send_error(HTTPD_500_INTERNAL_SERVER_ERROR, "Error validaint data");
    } else {
      char str[10]{};
      auto size = std::snprintf(str, 10, "%f", process_adc_data(data, result.readed));
      req.send(sys::span{str, (std::size_t)size});
    }
  } else if (result.error == ESP_ERR_TIMEOUT) {
    req.send_error(HTTPD_500_INTERNAL_SERVER_ERROR, "Error timeout reading");
  } else {
    req.send_error(HTTPD_500_INTERNAL_SERVER_ERROR, "Error reading data");
  }

  adc->stop();
  return ESP_OK;
}

static esp_err_t
http_404_error_handler(httpd_req_t *req, httpd_err_code_t err){
  httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
  return ESP_FAIL;
}
