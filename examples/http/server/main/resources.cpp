#include <algorithm>

#include "sys/error.hpp"
#include "lg/log.hpp"
#include "esp_http_server.h"

static constexpr const
lg::log lr{"Resources"};

/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
  char*  buf;
  size_t buf_len;

  /* Get header value string length and allocate memory for length + 1,
    * extra byte for null termination */
  buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    /* Copy null terminated value string into buffer */
    if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
      lr.info("Found header => Host: {}", buf);
    }
    free(buf);
  }

  /* Send response with custom headers and body set as the
    * string passed in user context*/
  const char* resp_str = (const char*) req->user_ctx;
  httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

  return ESP_OK;
}

/* An HTTP POST handler */
static esp_err_t echo_post_handler(httpd_req_t *req)
{
  lr.info("Echo handler");
  char buf[100];
  int ret, remaining = req->content_len;

  while (remaining > 0) {
    /* Read the data for the request */
    if ((ret = httpd_req_recv(req, buf,
                    std::min<int>(remaining, sizeof(buf)))) <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            /* Retry receiving if timeout occurred */
            continue;
        }
        return ESP_FAIL;
    }

    /* Send back the same data */
    httpd_resp_send_chunk(req, buf, ret);
    remaining -= ret;

    lr.info("=========== RECEIVED DATA ==========");
    lr.info("{}", std::string_view(buf, ret));
    lr.info("====================================");
  }

  // End response
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}
