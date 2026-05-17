# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")
set(PICO_DEOPTIMIZED_DEBUG 1)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-wasm4 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(pico-wasm4
    src/main.c
    src/wasm4-runtime.c
    src/wasm4-display.c
    src/wasm4-framebuffer.c
)

target_compile_definitions(pico-wasm4 PRIVATE
    PICO_STACK_SIZE=0x1000
)

add_subdirectory(lib/pico-m3)
add_subdirectory(lib/pico-st7789)
add_subdirectory(lib/pico-wasm)
add_subdirectory(lib/pico-buttons)

target_compile_definitions(pico_m3 INTERFACE
    d_m3FixedHeap=204800
    d_m3SkipMemoryBoundsCheck=1
    d_m3SkipStackCheck=0
    d_m3VerboseErrorMessages=0
)

pico_set_program_name(pico-wasm4 "pico-wasm4")
pico_set_program_version(pico-wasm4 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(pico-wasm4 TRUE)
pico_enable_stdio_usb(pico-wasm4 FALSE)
pico_enable_stdio_rtt(pico-wasm4 FALSE)

# Add the standard library to the build
target_link_libraries(pico-wasm4
    pico_stdlib
    pico_stdio
    pico_multicore
    pico_time
    pico_m3
    pico_st7789
    pico_wasm
    pico_buttons
    hardware_timer
)

# Add the standard include files to the build
target_include_directories(pico-wasm4 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

# Add any user requested libraries
# target_link_libraries(pico-wasm4
#     hardware_spi
# )

pico_add_extra_outputs(pico-wasm4)
