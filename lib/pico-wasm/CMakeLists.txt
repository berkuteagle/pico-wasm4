if(NOT TARGET pico_stdlib)
    message(FATAL_ERROR "For pico-sdk projects only!!!")
endif()

add_library(pico_wasm_headers INTERFACE)
target_include_directories(pico_wasm_headers SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

pico_add_impl_library(pico_wasm)
target_sources(pico_wasm INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/wasm.c
    ${CMAKE_CURRENT_LIST_DIR}/wasm_c_stack.c
    ${CMAKE_CURRENT_LIST_DIR}/wasm_loader.c
    ${CMAKE_CURRENT_LIST_DIR}/wasm_loader_cursor.c
    ${CMAKE_CURRENT_LIST_DIR}/wasm_loader_parser.c
    ${CMAKE_CURRENT_LIST_DIR}/wasm_locals.c
    ${CMAKE_CURRENT_LIST_DIR}/wasm_utils.c
    ${CMAKE_CURRENT_LIST_DIR}/wasm_v_stack.c
)
target_include_directories(pico_wasm PRIVATE INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
