set(LITTLEFS_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/littlefs")

add_library(pico_littlefs INTERFACE)
target_sources(pico_littlefs INTERFACE
    ${LITTLEFS_ROOT_DIR}/lfs.c
    ${LITTLEFS_ROOT_DIR}/lfs_util.c
)
target_include_directories(pico_littlefs INTERFACE ${LITTLEFS_ROOT_DIR})
target_compile_options(pico_littlefs INTERFACE -Wno-unused-function -Wno-null-dereference)
