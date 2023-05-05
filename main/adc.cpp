#include <optional>

#include "adc/continuous.hpp"

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
#define EXAMPLE_READ_LEN                    (EXAMPLE_READ_LEN_BYTES / sizeof(uC::ADC_continuous::data))

std::optional<uC::ADC_continuous> initiate_adc() {
  auto adc = std::make_optional<uC::ADC_continuous>(uC::ADC_continuous::config{
    .max_store_buf_size = EXAMPLE_ADC_BUFFER_SIZE,
    .conv_frame_size = EXAMPLE_READ_LEN_BYTES,
  });
  
  if (!adc->is_initiated()) {
    return std::nullopt;
  }

  uC::ADC_continuous::pattern ptt[]{
    {
      .atten      = EXAMPLE_ADC_ATTEN,
      .channel    = ADC_CHANNEL_0 & 0x7,
      .unit       = EXAMPLE_ADC_UNIT,
      .bit_width  = EXAMPLE_ADC_BIT_WIDTH
    }
  };

  if (adc->configure({
    .pattern_num    = sizeof(ptt) / sizeof(ptt[0]),
    .adc_pattern    = ptt,
    .sample_freq_hz = 26 * 1000,
    .conv_mode      = EXAMPLE_ADC_CONV_MODE,
    .format         = EXAMPLE_ADC_OUTPUT_TYPE,
  })) {
    return std::nullopt;
  }

  return adc;
}