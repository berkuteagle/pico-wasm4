if(NOT TARGET pico_stdlib)
    message(FATAL_ERROR "For pico-sdk projects only!!!")
endif()

add_library(pico_buttons_headers INTERFACE)
target_include_directories(pico_buttons_headers SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(pico_buttons_headers INTERFACE hardware_gpio_headers)

pico_add_impl_library(pico_buttons)
target_sources(pico_buttons INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/buttons.c
)
target_include_directories(pico_buttons PRIVATE INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src
)
target_link_libraries(pico_buttons INTERFACE hardware_gpio)
