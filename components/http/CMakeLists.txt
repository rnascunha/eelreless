idf_component_register(SRCS "src/server.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES sys esp_wifi esp_http_server esp_https_server)