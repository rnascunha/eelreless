idf_component_register(SRCS "https_main.cpp"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "certs/mydomain.com.crt"
                                   "certs/mydomain.com.key")