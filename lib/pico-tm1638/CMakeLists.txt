if(NOT TARGET pico_stdlib)
    message(FATAL_ERROR "For pico-sdk projects only!!!")
endif()

add_library(pico_tm1638_headers INTERFACE)
target_include_directories(pico_tm1638_headers SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(pico_tm1638_headers INTERFACE hardware_timer_headers hardware_gpio_headers)

pico_add_impl_library(pico_tm1638)
target_sources(pico_tm1638 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/tm1638.c
)
target_include_directories(pico_tm1638 PRIVATE INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(pico_tm1638 INTERFACE hardware_timer hardware_gpio)
