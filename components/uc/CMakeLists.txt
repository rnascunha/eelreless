idf_component_register(SRCS src/adc_stream.cpp src/gpio.cpp src/serial.cpp src/pulse_counter.cpp
                    INCLUDE_DIRS "include"
                    REQUIRES driver sys esp_adc)