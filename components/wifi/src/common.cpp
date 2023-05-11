#include "esp_netif.h"
#include "esp_wifi.h"

#include "sys/error.hpp"
#include "sys/event.hpp"

namespace wifi {

sys::error start() noexcept {
  return esp_wifi_start();
}

esp_netif_ip_info_t ip(esp_netif_t* handler) noexcept {
  esp_netif_ip_info_t ip_info{};
  esp_netif_get_ip_info(handler, &ip_info);
  return ip_info;
}

sys::error register_handler(std::int32_t id,
                            esp_event_handler_t handler,
                            void* arg /* = nullptr */) noexcept {
  return sys::event::register_handler(WIFI_EVENT, id, handler, arg);
}

}  // namespace wifi
