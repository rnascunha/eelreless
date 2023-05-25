#include <algorithm>

#include "sys/error.hpp"
#include "lg/log.hpp"
#include "esp_http_server.h"

static constexpr const
lg::log lr{"Resources"};

/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *request)
{
  http::server::request req(request);

  req.allow_cors();
  {
    auto host = req.header_value("Host");
    if (host)
      lr.info("Found header => Host: {}", host.get());
  }

  req.send((const char*)req.context());

  return ESP_OK;
}

/* An HTTP POST handler */
static esp_err_t echo_post_handler(httpd_req_t *request)
{
  http::server::request req(request);
  req.allow_cors();

  char buf[100];
  std::size_t remaining = req.content_length();
  int ret;

  while (remaining > 0) {
    ret = req.receive(std::span(buf, std::min(remaining, sizeof(buf))));
    if (ret < 0) {
      if (ret == HTTPD_SOCK_ERR_TIMEOUT)
          continue;
      return ESP_FAIL;
    }

    /* Send back the same data */
    req.send_chunk(std::span(buf, ret));
    remaining -= ret;

    lr.info("=========== RECEIVED DATA ==========");
    lr.info("{}", std::string_view(buf, ret));
    lr.info("====================================");
  }

  // End response
  req.end_chunk();
  return ESP_OK;
}

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
  http::server::request(req).send_error(HTTPD_404_NOT_FOUND, "Some 404 error message");
  return ESP_FAIL;
}