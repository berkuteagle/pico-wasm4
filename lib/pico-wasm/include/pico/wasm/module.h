#pragma once

#include "pico/wasm/func.h"

#ifndef WASM_MAX_FUNCS_COUNT
#define WASM_MAX_FUNCS_COUNT 128
#endif

typedef struct {
  wasm_func_t funcs[WASM_MAX_FUNCS_COUNT];
  uint16_t funcs_count;

  wasm_func_t *start_func;
  wasm_func_t *update_func;
} wasm_module_t;
