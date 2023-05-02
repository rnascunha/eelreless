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
#ifndef COMPONENTS_HTTP_SERVER_CONNECT_CB_HPP_
#define COMPONENTS_HTTP_SERVER_CONNECT_CB_HPP_

#include <type_traits>
#include <array>

#include "esp_wifi.h"
#include "esp_http_server.h"

#include "sys/event.hpp"
#include "http/server.hpp"
#include "http/functions.hpp"

namespace http {

struct not_register{};

template<typename StartCallable>
struct server_connect_cb {
  server_connect_cb(StartCallable&& callable) noexcept;
  server_connect_cb(not_register, StartCallable&& callable) noexcept
   : call{callable} {}

  server svr;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  StartCallable call;
};


static void server_disconnect(void* arg,
                               esp_event_base_t,
                               int32_t, void*) {
  ((server*)arg)->stop();
}

template<typename StartCallable>
static void server_connect(void* arg,
                           esp_event_base_t,
                           int32_t, void*){
  auto* ssc = (server_connect_cb<StartCallable>*)arg;
  if (ssc->svr.is_connected())
    return;
  if (ssc->svr.start(ssc->config) == ESP_OK) {
    ssc->call(ssc->svr);
  }
}

template<typename Callable>
void register_handler(server_connect_cb<Callable>& server) noexcept {
  sys::event::register_handler(IP_EVENT, IP_EVENT_STA_GOT_IP, &server_connect<Callable>, &server);
  sys::event::register_handler(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &server_disconnect, &server.svr);
}

template<typename Callable>
server_connect_cb<Callable>::server_connect_cb(Callable&& callable) noexcept
: call{callable} {
  register_handler(*this);
}

}  // namespace http

#endif  // COMPONENTS_HTTP_SERVER_CONNECT_CB_HPP_
