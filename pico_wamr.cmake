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

# ── User-configurable options ─────────────────────────────────────────────────
# Each option can be set to 0/1 by the parent project before add_subdirectory.
macro(_pwamr_default VAR DEFAULT)
  if(NOT DEFINED ${VAR})
    set(${VAR} ${DEFAULT})
  endif()
endmacro()

_pwamr_default(WAMR_BUILD_TARGET           "${_PWAMR_TARGET}")
_pwamr_default(WAMR_BUILD_INTERP           1)  # bytecode interpreter
_pwamr_default(WAMR_BUILD_FAST_INTERP      1)  # threaded-dispatch (faster, +~8 KB flash)
_pwamr_default(WAMR_BUILD_AOT              1)  # AOT blob loader; compile on host with wamrc
_pwamr_default(WAMR_BUILD_LIBC_BUILTIN     0)  # WAMR built-in libc (memcpy, printf, ...)
_pwamr_default(WAMR_BUILD_LIBC_WASI        0)  # WASI libc (requires os_* filesystem stubs)
_pwamr_default(WAMR_BUILD_MINI_LOADER      1)  # smaller loader, drops debug info
_pwamr_default(WAMR_BUILD_MULTI_MODULE     0)  # inter-module linking (needed for memory imports)
_pwamr_default(WAMR_BUILD_REF_TYPES        0)  # reference types proposal
# _pwamr_default(WAMR_BUILD_GLOBAL_HEAP_POOL 0)
# _pwamr_default(WAMR_BUILD_GLOBAL_HEAP_SIZE 131072)

# ── Features unavailable on bare-metal: forced off ────────────────────────────
foreach(_f
        WAMR_BUILD_JIT                   # Requires LLVM JIT infrastructure
        WAMR_BUILD_FAST_JIT              # Requires LLVM
        WAMR_BUILD_SIMD                  # No 128-bit SIMD on M0+/M33/Hazard3
        WAMR_BUILD_SHARED_MEMORY         # Requires OS threading primitives
        WAMR_BUILD_LIB_PTHREAD           # Requires pthreads
        WAMR_BUILD_LIB_PTHREAD_SEMAPHORE)
  if(${_f})
    message(WARNING "pico-wamr: ${_f} is not supported on Pico bare-metal, forcing off")
  endif()
  set(${_f} 0)
endforeach()

# Hardware bound check uses OS signal handlers — not available bare-metal
set(WAMR_DISABLE_HW_BOUND_CHECK 1)

# ── Wire up WAMR's build system ───────────────────────────────────────────────
set(WAMR_BUILD_PLATFORM "pico")

include("${WAMR_ROOT_DIR}/build-scripts/runtime_lib.cmake")

# ── Library target ─────────────────────────────────────────────────────────────
add_library(pico_wamr STATIC ${WAMR_RUNTIME_LIB_SOURCE})

target_compile_definitions(pico_wamr PUBLIC ${_PWAMR_CHIP_DEF})

target_link_libraries(pico_wamr PUBLIC pico_stdlib)
