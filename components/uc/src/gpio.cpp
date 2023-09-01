#include "driver/gpio.h"

#include "sys/error.hpp"
#include "uc/gpio.hpp"

namespace uc {

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

// Interrupt functions
sys::error gpio::add_isr(gpio_int_type_t type, gpio_isr_t cb, void* arg /* = nullptr */) noexcept {
  auto err = gpio_set_intr_type(pin_, type);
  if (err != ESP_OK) return err;
  return gpio_isr_handler_add(pin_, cb, arg);
}

sys::error gpio::remove_isr() noexcept {
  return gpio_isr_handler_remove(pin_);
}

sys::error gpio::install_isr(int intr_alloc_flags /* = 0 */) noexcept {
  return gpio_install_isr_service(intr_alloc_flags);
}

void gpio::unistall_isr() noexcept {
  gpio_uninstall_isr_service();
}

}  // namespace uc
