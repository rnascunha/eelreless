#include <optional>

#include "control_flow.hpp"

control_flow::control_flow(gpio_num_t valve_port, gpio_num_t sensor_port, double k_sensor) noexcept
 : valve_(valve_port, GPIO_MODE_INPUT_OUTPUT), sensor_(sensor_port, k_sensor) {}
  
bool control_flow::is_open() noexcept {
  return valve_.read() != 0;
}

void control_flow::open() noexcept {
  sensor_.start();
  valve_.write(1);
}

void control_flow::close() noexcept {
  valve_.write(0);
  sensor_.stop();
}

sys::error control_flow::clear_count() noexcept {
  return sensor_.clear();
}

std::optional<int> control_flow::count() noexcept {
  return sensor_.count();
}

std::optional<double>
control_flow::volume() noexcept {
  return sensor_.volume<double>();
}

double
control_flow::volume(int count) noexcept {
  return sensor_.volume<double>(count);
}

int control_flow::k_ratio() const noexcept {
  return sensor_.k_ratio();
}
