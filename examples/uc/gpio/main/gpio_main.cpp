#include <chrono>

#include "esp_log.h"

#include "sys/error.hpp"
#include "sys/sys.hpp"

#include "uc/gpio.hpp"

#define TAG "GPIO"

#define GPIO_LED    GPIO_NUM_2    // ON board led at ESP32 DevKitC
#define GPIO_BUTTON GPIO_NUM_0    // BOOT button at ESP32 DevKitC

extern "C" void app_main() {
  uc::gpio led(GPIO_LED, GPIO_MODE_OUTPUT);
  uc::gpio btn(GPIO_BUTTON, GPIO_MODE_INPUT /* , GPIO_PULLUP_ONLY */);

  // Must call it if want to use install
  uc::gpio::install_isr();

  int cnt = 0;
  btn.add_isr(GPIO_INTR_ANYEDGE, [](void* args){
    ++(*static_cast<int*>(args));
  }, &cnt);

  int level = 0;
  while (true) {
    ESP_LOGI(TAG, "read: %d / Counter: %d", btn.read(), cnt);

    // Blinking led
    led.write(level = !level);

    using namespace std::chrono_literals;
    sys::delay(1s);
  }
}