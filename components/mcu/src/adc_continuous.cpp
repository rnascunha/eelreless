#include <cassert>

#include "esp_adc/adc_continuous.h"

#include "sys/error.hpp"
#include "sys/time.hpp"
#include "adc/continuous.hpp"

namespace uC {

ADC_continuous::ADC_continuous(const config& cfg) noexcept {
  init(cfg);
}

bool
ADC_continuous::is_initiated() const noexcept {
  return handler_ != nullptr;
}

sys::error
ADC_continuous::init(const config& cfg) noexcept {
  assert(handler_ == nullptr && "ADC already initated");

  auto ret = adc_continuous_new_handle(&cfg, &handler_);
  if (ret)
    handler_ = nullptr;
  return ret;
}

sys::error ADC_continuous::deinit() noexcept {
  if (handler_ == nullptr)
    return ESP_OK;
  auto ret = adc_continuous_deinit(handler_);
  if (!ret)
    handler_ = nullptr;
  return ret;
}

sys::error
ADC_continuous::start() noexcept {
  assert(handler_ != nullptr && "ADC NOT initated");
  return adc_continuous_start(handler_);
}

sys::error
ADC_continuous::stop() noexcept {
  if (handler_ == nullptr)
    return ESP_OK;

  return adc_continuous_stop(handler_);
}

sys::error
ADC_continuous::configure(const continuous_config& cfg) noexcept {
  assert(handler_ != nullptr && "ADC NOT initated");
  return adc_continuous_config(handler_, &cfg);
}

sys::error
ADC_continuous::register_handler(const callback& cb,
                                 void* data /* = nullptr */) noexcept {
  assert(handler_ != nullptr && "ADC NOT initated");
  return adc_continuous_register_event_callbacks(handler_, &cb, data);
}

ADC_continuous::result
ADC_continuous::read(data* dt, std::size_t size, sys::time::ticks ticks) noexcept {
  assert(handler_ != nullptr && "ADC NOT initated");
  std::uint32_t num = 0;
  sys::error err = adc_continuous_read(handler_,
                                       (std::uint8_t*)&(dt->raw_data()),
                                       sizeof(data) * size,
                                       &num,
                                       ticks);
  return {num / sizeof(adc_digi_output_data_t), err};
}

ADC_continuous::handler
ADC_continuous::initiate(const config& cfg) noexcept {
  handler h = nullptr;
  if (adc_continuous_new_handle(&cfg, &h)) {
    h = nullptr;
  }
  return h;
}

}  // namespace uC
