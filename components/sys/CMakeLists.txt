idf_component_register(SRCS "src/event.cpp" "src/sys.cpp" "src/nvs.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_event nvs_flash esp_netif)