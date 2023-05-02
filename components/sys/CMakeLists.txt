idf_component_register(SRCS "src/event.cpp" "src/sys.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_event nvs_flash esp_netif)