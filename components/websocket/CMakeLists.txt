idf_component_register(SRCS "src/server.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_http_server sys http)