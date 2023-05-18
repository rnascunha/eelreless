/**
 * @file main.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <cstring>
#include <chrono>
#include <algorithm>

#include "lg/log.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "sys/error.hpp"
#include "sys/sys.hpp"
#include "sys/time.hpp"

#include "uc/adc/stream.hpp"
#include "wave.hpp"

#define EXAMPLE_ADC_UNIT                    ADC_UNIT_1
#define _EXAMPLE_ADC_UNIT_STR(unit)         #unit
#define EXAMPLE_ADC_UNIT_STR(unit)          _EXAMPLE_ADC_UNIT_STR(unit)
#define EXAMPLE_ADC_CONV_MODE               ADC_CONV_SINGLE_UNIT_1
#define EXAMPLE_ADC_ATTEN                   ADC_ATTEN_DB_11
#define EXAMPLE_ADC_BIT_WIDTH               SOC_ADC_DIGI_MAX_BITWIDTH

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
#define EXAMPLE_ADC_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE1
#else
#define EXAMPLE_ADC_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE2
#endif

#define EXAMPLE_ADC_BUFFER_SIZE             4092
#define EXAMPLE_READ_LEN_BYTES              1400
#define EXAMPLE_READ_LEN                    (EXAMPLE_READ_LEN_BYTES / sizeof(uc::adc::stream::data))

static constexpr const
lg::log ll{"wave"};

static bool IRAM_ATTR
conversion_done(adc_continuous_handle_t handle,
               const adc_continuous_evt_data_t *edata,
               void *user_data) {
  BaseType_t mustYield = pdFALSE;
  vTaskNotifyGiveFromISR((TaskHandle_t)user_data, &mustYield);

  return (mustYield == pdTRUE);
}

bool validate_data(uc::adc::stream::data* begin,
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

double process_adc_data(uc::adc::stream::data* data,
                        std::size_t size) noexcept {
  using value_type = uc::adc::stream::data::value_type;
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

extern "C" void app_main() {
  uc::adc::stream adc({
    .max_store_buf_size = EXAMPLE_ADC_BUFFER_SIZE,
    .conv_frame_size = EXAMPLE_READ_LEN_BYTES,
  });
  
  if (!adc.is_initiated()) {
    ll.error("Error intiating ADC continuous");
    return;
  }

  uc::adc::stream::pattern ptt[]{
    {
      .atten      = EXAMPLE_ADC_ATTEN,
      .channel    = ADC_CHANNEL_0 & 0x7,
      .unit       = EXAMPLE_ADC_UNIT,
      .bit_width  = EXAMPLE_ADC_BIT_WIDTH
    }
  };

  adc.configure({
    .pattern_num    = sizeof(ptt) / sizeof(ptt[0]),
    .adc_pattern    = ptt,
    .sample_freq_hz = 26 * 1000,
    .conv_mode      = EXAMPLE_ADC_CONV_MODE,
    .format         = EXAMPLE_ADC_OUTPUT_TYPE,
  });

  adc.register_handler({
    .on_conv_done = conversion_done,
    .on_pool_ovf = nullptr
  }, xTaskGetCurrentTaskHandle());
  
  if (adc.start()) {
    ll.error("Error starting ADC continuous");
    return;
  }

  uc::adc::stream::data data[EXAMPLE_READ_LEN]{};

  while(1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    while (1) {
      auto result = adc.read(data, EXAMPLE_READ_LEN, 0);
      if (result) {
        ll.info("{} new samples collected", result.readed);
        if (!validate_data(data, result.readed)) {
          ll.warn("Invalid data received");
        } else {
          ll.info("Irms = {}",
                   process_adc_data(data, result.readed));
        }
        using namespace std::chrono_literals;
        sys::delay(1s); // Need for watchdog
      } else if (result.error == ESP_ERR_TIMEOUT) {
        break;
      }
    }
  }
}
