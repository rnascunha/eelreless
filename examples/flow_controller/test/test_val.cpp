/**
 * @file main.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <chrono>

#include "uc/gpio.hpp"
#include "facility/debounce.hpp"

#define LED_GPIO            GPIO_NUM_2
#define VALVE_OUTPUT_GPIO   GPIO_NUM_21
#define BUTTON_GPIO         GPIO_NUM_0

extern "C" void app_main() {
  uc::gpio led(LED_GPIO, GPIO_MODE_OUTPUT);
  uc::gpio valve(VALVE_OUTPUT_GPIO, GPIO_MODE_OUTPUT);
  facility::debounce btn(uc::gpio(BUTTON_GPIO, GPIO_MODE_INPUT),
                         2);

  int level = 0;
  using namespace std::chrono_literals;
  while (true) {
    btn.wait(1s, [&] {
      valve.write(level = !level);
      led.write(level);
    });
  }
}