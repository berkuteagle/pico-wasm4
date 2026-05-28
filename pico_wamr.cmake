set(WAMR_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/wamr")

# ── Platform detection ────────────────────────────────────────────────────────
if(PICO_PLATFORM STREQUAL "rp2040")
  # Cortex-M0+, ARMv6-M, no FPU, no hardware cache on SRAM
  set(_PWAMR_TARGET   "THUMBV6M")
  set(_PWAMR_CHIP_DEF "PICO_WAMR_RP2040=1")
  set(_PWAMR_WAMRC    "--target=thumbv6m.main --target-abi=eabi --cpu=cortex-m0plus")
elseif(PICO_PLATFORM STREQUAL "rp2350-arm-s")
  # Cortex-M33, ARMv8-M.main, single-precision FPU (fpv5-sp-d16)
  # Pico SDK uses -mfloat-abi=hard, so wamrc must use eabihf to match.
  # THUMBV8M_VFP: wamrc emits VFP instructions for f32/f64 ops.
  set(_PWAMR_TARGET   "THUMBV8M_VFP")
  set(_PWAMR_CHIP_DEF "PICO_WAMR_RP2350=1")
  set(_PWAMR_WAMRC    "--target=thumbv8m.main --target-abi=eabihf --cpu=cortex-m33")
elseif(PICO_PLATFORM STREQUAL "rp2350-riscv")
  # Hazard3, RV32IMAC
  set(_PWAMR_TARGET   "RISCV32")
  set(_PWAMR_CHIP_DEF "PICO_WAMR_RP2350=1;PICO_WAMR_RISCV=1")
  set(_PWAMR_WAMRC    "--target=riscv32 --target-abi=ilp32 --cpu=generic-rv32")
else()
  message(FATAL_ERROR "WAMR: Unsupported PICO_PLATFORM '${PICO_PLATFORM}'")
endif()

message(STATUS "pico-wamr: Platform: ${PICO_PLATFORM}  WAMR target: ${_PWAMR_TARGET}")
message(STATUS "pico-wamr: wamrc AOT:  wamrc ${_PWAMR_WAMRC} -o out.aot in.wasm")

set(WAMR_BUILD_TARGET       "${_PWAMR_TARGET}")
set(WAMR_BUILD_INTERP       1)  # bytecode interpreter
set(WAMR_BUILD_FAST_INTERP  1)  # threaded-dispatch (faster, +~8 KB flash)
set(WAMR_BUILD_AOT          0)  # AOT blob loader; compile on host with wamrc
set(WAMR_BUILD_LIBC_BUILTIN 0)  # WAMR built-in libc (memcpy, printf, ...)
set(WAMR_BUILD_LIBC_WASI    0)  # WASI libc (requires os_* filesystem stubs)
set(WAMR_BUILD_MINI_LOADER  1)  # smaller loader, drops debug info
set(WAMR_BUILD_MULTI_MODULE 0)  # inter-module linking (needed for memory imports)
set(WAMR_BUILD_REF_TYPES    0)  # reference types proposal

set(WAMR_BUILD_GLOBAL_HEAP_POOL 0)
set(WAMR_BUILD_GLOBAL_HEAP_SIZE 65536) # 131072

set(WAMR_BUILD_JIT                   0)
set(WAMR_BUILD_FAST_JIT              0)
set(WAMR_BUILD_SIMD                  0)
set(WAMR_BUILD_SHARED_MEMORY         0)
set(WAMR_BUILD_LIB_PTHREAD           0)
set(WAMR_BUILD_LIB_PTHREAD_SEMAPHORE 0)
set(WAMR_DISABLE_HW_BOUND_CHECK      1)

set(WAMR_BUILD_PLATFORM "pico")

include("${WAMR_ROOT_DIR}/build-scripts/runtime_lib.cmake")

# ── Library target ─────────────────────────────────────────────────────────────
add_library(pico_wamr STATIC ${WAMR_RUNTIME_LIB_SOURCE})

target_compile_definitions(pico_wamr PUBLIC ${_PWAMR_CHIP_DEF})

target_link_libraries(pico_wamr PUBLIC pico_stdlib)
