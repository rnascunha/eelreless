/**
 * @file retry_handler.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_WIFI_SIMPLE_RETRY_HANDLER_HPP_
#define COMPONENTS_WIFI_SIMPLE_RETRY_HANDLER_HPP_

#include <limits>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

namespace wifi {
namespace station {

struct not_register{};

class simple_wifi_retry {
 public:
  static constexpr const unsigned connected = BIT0;
  static constexpr const unsigned fail = BIT1;

  simple_wifi_retry(int max_retry = std::numeric_limits<int>::max()) noexcept;
  simple_wifi_retry(not_register, int max_retry = std::numeric_limits<int>::max()) noexcept;

  EventBits_t wait(TickType_t wait_time = portMAX_DELAY) noexcept;

  static void handler(void* arg,
               esp_event_base_t event_base,
               int32_t event_id,
               void* event_data) noexcept;

  int retry() const noexcept { return retry_; }
  int max_retry() const noexcept { return max_retry_; }
  void reset() noexcept;

  bool is_connected() const noexcept;
  bool failed() const noexcept;

 private:
  void wifi_handler(void* arg,
                    int32_t event_id,
                    void* event_data) noexcept;
  void ip_handler(void* arg,
            int32_t event_id,
            void* event_data) noexcept;

  EventGroupHandle_t event_ = xEventGroupCreate();
  int max_retry_;
  int retry_ = 0;
};

void register_handler(simple_wifi_retry& instance) noexcept;

}  // namespace station 
}  // namespace wifi

#endif //  COMPONENTS_WIFI_SIMPLE_RETRY_HANDLER_HPP_
