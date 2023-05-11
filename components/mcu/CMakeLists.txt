idf_component_register(SRCS src/adc_continuous.cpp src/gpio.cpp
                    INCLUDE_DIRS "include"
                    REQUIRES sys esp_adc driver)