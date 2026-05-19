if(NOT TARGET pico_stdlib)
    message(FATAL_ERROR "For pico-sdk projects only!!!")
endif()

option(M3_STATIC_ENV "Static environment" ON)

add_library(pico_m3_headers INTERFACE)
target_include_directories(pico_m3_headers SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

pico_add_impl_library(pico_m3)
target_sources(pico_m3 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/m3_bind.c
    ${CMAKE_CURRENT_LIST_DIR}/src/m3_code.c
    ${CMAKE_CURRENT_LIST_DIR}/src/m3_compile.c
    ${CMAKE_CURRENT_LIST_DIR}/src/m3_core.c
    ${CMAKE_CURRENT_LIST_DIR}/src/m3_env.c
    ${CMAKE_CURRENT_LIST_DIR}/src/m3_function.c
    ${CMAKE_CURRENT_LIST_DIR}/src/m3_info.c
    ${CMAKE_CURRENT_LIST_DIR}/src/m3_module.c
    ${CMAKE_CURRENT_LIST_DIR}/src/m3_parse.c
)

if(M3_STATIC_ENV)
    target_compile_definitions(pico_m3 INTERFACE
        M3_STATIC_ENV=true
    )
endif()

# target_compile_features(m3 PRIVATE c_std_99)

# FIXME: comparison of integers of different signs: 'u32' and 'i32'
set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/m3_env.c PROPERTIES COMPILE_FLAGS -Wno-sign-compare)
