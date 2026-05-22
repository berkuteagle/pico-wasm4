if(NOT TARGET pico_stdlib)
    message(FATAL_ERROR "For pico-sdk projects only!!!")
endif()

add_library(pico_w25qxx_headers INTERFACE)
target_include_directories(pico_w25qxx_headers SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(pico_w25qxx_headers INTERFACE hardware_spi_headers)

pico_add_impl_library(pico_w25qxx)
target_sources(pico_w25qxx INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/w25qxx.c
)
# target_include_directories(pico_w25qxx PRIVATE INTERFACE
#     ${CMAKE_CURRENT_LIST_DIR}/src
# )
target_link_libraries(pico_w25qxx INTERFACE hardware_spi)
