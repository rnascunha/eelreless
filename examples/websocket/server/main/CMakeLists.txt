idf_component_register(SRCS "websocket_server_main.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES sys wifi http websocket)