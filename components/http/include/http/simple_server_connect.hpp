/**
 * @file simple_connect.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMPONENTS_SIMPLE_HTTP_SERVER_CONNECT_HPP_
#define COMPONENTS_SIMPLE_HTTP_SERVER_CONNECT_HPP_

#include <type_traits>
#include <array>

#include "esp_wifi.h"
#include "esp_http_server.h"

#include "sys/event.hpp"
#include "http/server.hpp"
#include "http/functions.hpp"

namespace http {

struct not_register{};

template<typename ...Args>
struct simple_server_connect {
  simple_server_connect(Args&&... args) noexcept;
  simple_server_connect(not_register, Args&&... args) noexcept
   : uris{args...} {}

  server svr;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  std::array<uri_error, sizeof...(Args)> uris;
};


static void server_disconnect(void* arg,
                               esp_event_base_t,
                               int32_t, void*) {
  ((server*)arg)->stop();
}

template<typename ...Args>
static void server_connect(void* arg,
                           esp_event_base_t,
                           int32_t, void*){
  auto* ssc = (simple_server_connect<Args...>*)arg;
  if (ssc->svr.is_connected())
    return;
  if (ssc->svr.start(ssc->config) == ESP_OK) {
    register_uris(ssc->svr, ssc->uris);
  }
}

template<typename ...Args>
void register_handler(simple_server_connect<Args...>& server) noexcept {
  sys::event::register_handler(IP_EVENT, IP_EVENT_STA_GOT_IP, &server_connect<Args...>, &server);
  sys::event::register_handler(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &server_disconnect, &server.svr);
}

template<typename ...Args>
simple_server_connect<Args...>::simple_server_connect(Args&&... args) noexcept
: uris{args...} {
  register_handler(*this);
}

}  // namespace http

#endif  // COMPONENTS_SIMPLE_HTTP_SERVER_CONNECT_HPP_