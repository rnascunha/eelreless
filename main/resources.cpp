#include <cstdio>

#include "esp_err.h"
#include "esp_http_server.h"

#include "adc/continuous.hpp"
#include "wave.hpp"

bool validate_data(uC::ADC_continuous::data* begin,
                    std::size_t size) noexcept {
  auto end = begin + size;
  while (begin != end) {
    if (!begin->is_valid(EXAMPLE_ADC_UNIT))
      return false;
    ++begin;
  }
  return true;
}

// Need to incresase size of task to put array inside
static
double bbout[EXAMPLE_READ_LEN]{};

double process_adc_data(uC::ADC_continuous::data* data,
                        std::size_t size) noexcept {
  using value_type = uC::ADC_continuous::data::value_type;
  value_type* begin = &data->raw_data().val;
  value_type* end = begin;
  for (std::size_t i = 0; i < size; ++i)
    *end++ = data[i].value();

  double* bout = bbout;
  double* eout = bout + size;

  wave::filter_first_order(begin, end, 0.8);
  wave::convert(begin, end, bout);
  wave::remove_constant(bout, eout,
                          wave::mean(bout, eout));

  return wave::rms_sine(bout, eout);
}

static esp_err_t
current_get_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "Current handler");
  uC::ADC_continuous* adc = (uC::ADC_continuous*) req->user_ctx;

  if (adc->start()) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error starting adc");
    return ESP_OK;
  }

  uC::ADC_continuous::data data[EXAMPLE_READ_LEN]{};
  using namespace std::chrono_literals;
  auto result = adc->read(data, EXAMPLE_READ_LEN, sys::time::to_ticks(500ms));
  if (result) {
    if (!validate_data(data, result.readed)) {
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error validaint data");
    } else {
      char str[10]{};
      auto size = std::snprintf(str, 10, "%f", process_adc_data(data, result.readed));
      httpd_resp_send(req, str, size);
    }
  } else if (result.error == ESP_ERR_TIMEOUT) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error timeout reading");
  } else {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error reading data");
  }

  adc->stop();
  return ESP_OK;
}

static esp_err_t
http_404_error_handler(httpd_req_t *req, httpd_err_code_t err){
  httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
  return ESP_FAIL;
}
