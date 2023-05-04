/**
 * @file mcu.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <cstdint>


namespace uC {

enum class ADC_continuous_format {
  type1 = 0,
  type2,
  type1_2
};

template<ADC_continuous_format Type>
struct mcu {
  struct ADC {
    struct continuous {
      static constexpr const
      ADC_continuous_format format = Type;
    };
  };
};

using esp32 = mcu<ADC_continuous_format::type1>;
using esp32s2 = mcu<ADC_continuous_format::type1_2>;
using esp32c3 = mcu<ADC_continuous_format::type2>;
using esp32h4 = mcu<ADC_continuous_format::type2>;
using esp32c2 = mcu<ADC_continuous_format::type2>;
using esp32s3 = mcu<ADC_continuous_format::type2>;
using esp32c6 = mcu<ADC_continuous_format::type2>;
using esp32h2 = mcu<ADC_continuous_format::type2>;

#if CONFIG_IDF_TARGET_ESP32
using compiled_mcu = esp32;
#elif CONFIG_IDF_TARGET_ESP32S2
using compiled_mcu = esp32s2;
#elif CONFIG_IDF_TARGET_ESP32C3
using compiled_mcu = esp32c3;
#elif CONFIG_IDF_TARGET_ESP32H4
using compiled_mcu = esp32h4;
#elif CONFIG_IDF_TARGET_ESP32C2
using compiled_mcu = esp32c2;
#elif CONFIG_IDF_TARGET_ESP32S3
using compiled_mcu = esp32s3;
#elif CONFIG_IDF_TARGET_ESP32C6
using compiled_mcu = esp32c6;
#elif CONFIG_IDF_TARGET_ESP32H2
#else
  #error "Unknown MCU"
#endif

}  // namespace uC