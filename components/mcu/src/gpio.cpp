#include "driver/gpio.h"

#include "sys/error.hpp"
#include "gpio.hpp"

namespace mcu {

gpio::gpio(gpio_num_t pin) noexcept
 : pin_{pin} {}

gpio::gpio(gpio_num_t pin, gpio_mode_t md) noexcept
 : pin_{pin}  {
  mode(md);
}

gpio::gpio(gpio_num_t pin, gpio_mode_t md, gpio_pull_mode_t pl) noexcept
 : pin_{pin}  {
  mode(md);
  pull(pl);
}

sys::error gpio::mode(gpio_mode_t md) noexcept {
  return gpio_set_direction(pin_, md);
}

sys::error gpio::pull(gpio_pull_mode_t pl) noexcept {
  return gpio_set_pull_mode(pin_, pl);
}

int gpio::read() noexcept {
  return gpio_get_level(pin_);
}

sys::error gpio::write(int level) noexcept {
  return gpio_set_level(pin_, level);
}

sys::error gpio::reset() noexcept {
  return gpio_reset_pin(pin_);
}

}  // namespace mcu
