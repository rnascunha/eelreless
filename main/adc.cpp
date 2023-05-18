/**
 * @file adc.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <optional>

#include "uc/adc/stream.hpp"
#include "wave.hpp"

#include "adc.hpp"

#define EXAMPLE_ADC_CONV_MODE               ADC_CONV_SINGLE_UNIT_1
#define EXAMPLE_ADC_ATTEN                   ADC_ATTEN_DB_11
#define EXAMPLE_ADC_BIT_WIDTH               SOC_ADC_DIGI_MAX_BITWIDTH

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
#define EXAMPLE_ADC_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE1
#else
#define EXAMPLE_ADC_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE2
#endif

[[nodiscard]] std::optional<uc::adc::stream>
initiate_adc() noexcept {
  auto adc = std::make_optional<uc::adc::stream>(uc::adc::stream::config{
    .max_store_buf_size = EXAMPLE_ADC_BUFFER_SIZE,
    .conv_frame_size = EXAMPLE_READ_LEN_BYTES,
  });
  
  if (!adc->is_initiated()) {
    return std::nullopt;
  }

  uc::adc::stream::pattern ptt[]{
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

[[nodiscard]] 
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

[[nodiscard]] 
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
