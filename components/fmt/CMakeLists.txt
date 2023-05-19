idf_component_register()
option(FMT_INSTALL "" OFF)
add_subdirectory(fmt)
target_compile_definitions(fmt PUBLIC FMT_STATIC_THOUSANDS_SEPARATOR
                                      FMT_USE_LONG_DOUBLE=0)
target_link_libraries(${COMPONENT_LIB} INTERFACE fmt::fmt)
