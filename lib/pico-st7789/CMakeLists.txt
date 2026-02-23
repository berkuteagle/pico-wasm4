if(NOT TARGET pico_stdlib)
    message(FATAL_ERROR "For pico-sdk projects only!!!")
endif()

add_library(pico_st7789_headers INTERFACE)
target_include_directories(pico_st7789_headers SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(pico_st7789_headers INTERFACE hardware_spi_headers hardware_gpio_headers)

pico_add_impl_library(pico_st7789)
target_sources(pico_st7789 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/st7789.c
)
target_include_directories(pico_st7789 PRIVATE INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(pico_st7789 INTERFACE hardware_spi hardware_gpio)
