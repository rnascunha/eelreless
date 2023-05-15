/**
 * @file stream.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_UC_ADC_STREAM_HPP_
#define COMPONENTS_UC_ADC_STREAM_HPP_

#include <cstdint>
#include <utility>

#include "esp_adc/adc_continuous.h"

#include "sys/error.hpp"
#include "sys/time.hpp"

namespace uc {
namespace adc {

class stream {
 public:
  using handler = adc_continuous_handle_t;
  using config = adc_continuous_handle_cfg_t;
  using continuous_config = adc_continuous_config_t;
  using callback = adc_continuous_evt_cbs_t;
  using pattern = adc_digi_pattern_config_t;

  class data {
   public:
    using value_type = decltype(std::declval<adc_digi_output_data_t>().val);

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
    value_type channel() const noexcept {
      return data_.type1.channel;
    }

    value_type value() const noexcept {
      return data_.type1.data;
    }
#else
    value_type channel() const noexcept {
      return data_.type2.channel;
    }

    value_type value() const noexcept {
      return data_.type2.data;
    }

    value_type unit() const noexcept {
      return data_.type2.unit();
    }
#endif
    bool is_valid(std::uint32_t unit) const noexcept {
      return channel() < SOC_ADC_CHANNEL_NUM(unit);
    }

    adc_digi_output_data_t&
    raw_data() noexcept {
      return data_;
    }

    const adc_digi_output_data_t&
    raw_data() const noexcept {
      return data_;
    }

   private:
    adc_digi_output_data_t data_{};
  };

  struct result {
    std::uint32_t readed;
    sys::error    error;

    constexpr
    operator bool() const noexcept {
      return !(bool)error;
    }
  };

  stream() noexcept = default;
  stream(const config& cfg) noexcept;
  stream(const stream&) noexcept = delete;
  stream(stream&& adc) noexcept {
    handler_ = adc.handler_;
    adc.handler_ = nullptr;
  }

  bool is_initiated() const noexcept;

  sys::error init(const config&) noexcept;
  sys::error deinit() noexcept;

  sys::error start() noexcept;
  sys::error stop() noexcept;

  sys::error configure(const continuous_config&) noexcept;

  sys::error register_handler(const callback&, void* data = nullptr) noexcept;

  result read(data*, std::size_t, sys::time::ticks) noexcept;
  template<typename Rep, typename Ratio>
  result read(data* dt, std::size_t size,
              std::chrono::duration<Rep, Ratio> duration) noexcept {
    return read(dt, size, sys::time::to_ticks(duration));
  }

  static handler
  initiate(const config& cfg) noexcept;

  ~stream() noexcept {
    stop();
    deinit();
  }
 private:
  handler handler_ = nullptr;
};

}  // namespace adc
}  // namespace uc

#endif  // COMPONENTS_UC_ADC_STREAM_HPP_
