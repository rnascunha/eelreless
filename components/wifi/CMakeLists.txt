idf_component_register(SRCS "src/station.cpp" "src/simple_wifi_retry.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES sys esp_system esp_netif esp_wifi)