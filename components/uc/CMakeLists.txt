idf_component_register(SRCS src/adc_stream.cpp src/gpio.cpp src/serial.cpp
                    INCLUDE_DIRS "include"
                    # PRIV_INCLUDE_DIRS $ENV{IDF_PATH}/components/freertos/FreeRTOS-Kernel/include
                    REQUIRES driver sys esp_adc)