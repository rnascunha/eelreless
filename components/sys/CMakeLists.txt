idf_component_register(SRCS "src/event.cpp"
                            "src/sys.cpp"
                            "src/nvs.cpp"
                            "src/task.cpp"
                            "src/time.cpp"
                            "src/timer.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_event esp_timer nvs_flash esp_netif lg)