#include <span>
#include <string_view>

#include <esp_log.h>

#include "sys/sys.hpp"
#include "sys/nvs.hpp"
#include "sys/error.hpp"
#include "http/server.hpp"

#include "func.hpp"

static constexpr const char* TAGG = "Resources";

extern const std::uint8_t mini_start[] asm("_binary_mini_html_start");
extern const std::uint8_t mini_end[]   asm("_binary_mini_html_end");

// static void 
// print_array(const char* arr, std::size_t size) noexcept {
//   for (int i = 0; i < size; ++i) {
//     printf("%c:%d ", *arr, *arr);
//     ++arr;
//   }
//   printf("\n");
// }

static bool validate_ssid(const std::string_view& ssid) noexcept {
  return ssid.size() >= 2 && ssid.size() <= 32;
}

static bool validate_password(const std::string_view& ssid) noexcept {
  return ssid.size() >= 8 && ssid.size() <= 64;
}

/* An HTTP GET handler */
static esp_err_t
web_get_handler(httpd_req_t *req) {
  return http::server::request{req}
          .content_type("text/html")
          .send(std::span(mini_start, mini_end - mini_start));
}

static esp_err_t
web_post_handler(httpd_req_t *request) {
  ESP_LOGI(TAGG, "Post web");
  char buffer[100];
  http::server::request req{request};
  int size = req.receive(std::span{buffer});
  if (size < 0) {
    ESP_LOGW(TAGG, "Erro received data %d", size);
    req.send_error(HTTPD_500_INTERNAL_SERVER_ERROR, "Rcv Error");
    return ESP_FAIL;
  }
  // ESP_LOGI(TAGG, "Received[%d]: %.*s", size, size, buffer);
  // print_array(buffer, size);

  // Processing data
  auto* sep = (const char*)std::memchr(buffer, '?', size);
  if (sep == nullptr) {
    req.send_error(HTTPD_400_BAD_REQUEST, "Rcv Error");
    return ESP_FAIL;
  }

  auto ssid_size = sep - buffer;
  std::string_view ssid(buffer, ssid_size);
  if (!validate_ssid(ssid)) {
    ESP_LOGW(TAGG, "Invalid SSID");
    req.send("Invalid SSID");
    return ESP_OK;
  }

  std::string_view pass(sep + 1, size - ssid_size - 1);
  if (!validate_password(ssid)) {
    ESP_LOGW(TAGG, "Invalid password");
    req.send("Invalid password");
    return ESP_OK;
  }

  ESP_LOGI(TAGG, "SSID:%.*s, PASS:%.*s",
           ssid.size(), ssid.data(), pass.size(), pass.data());

  buffer[ssid_size] = '\0';
  buffer[size] = '\0';
  auto* storage = (sys::nvs*)req.context();
  storage->set(NVS_KEY_SSID, ssid.data());
  storage->set(NVS_KEY_PASS, pass.data());
  storage->commit();

  req.send(std::span(std::string_view{"Net configured"}));
  ESP_LOGI(TAGG, "Configured WiFi");

  return ESP_OK;
}

static esp_err_t
reboot_get_handler(httpd_req_t *request) {
  sys::reboot();
  return ESP_OK;
}

static
esp_err_t
http_404_error_handler(httpd_req_t *req, httpd_err_code_t) {
  http::server::request{req}.send_error(HTTPD_404_NOT_FOUND, "Some 404 error message");
  return ESP_FAIL;
}