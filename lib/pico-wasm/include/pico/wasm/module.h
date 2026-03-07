#pragma once

#include "pico/wasm/func.h"
#include "pico/wasm/table.h"
#include <stdint.h>

#ifndef WASM_MAX_FUNCS_COUNT
#define WASM_MAX_FUNCS_COUNT 128
#endif

typedef struct {
  wasm_type_t types[WASM_MAX_FUNCS_COUNT];
  uint16_t types_count;

  wasm_func_t funcs[WASM_MAX_FUNCS_COUNT];
  uint16_t funcs_count;

  wasm_table_t table;

  wasm_import_func_t import_funcs[WASM_MAX_FUNCS_COUNT];
  uint16_t import_funcs_count;

  wasm_export_func_t export_funcs[WASM_MAX_FUNCS_COUNT];
  uint16_t export_funcs_count;

  uint8_t *memory;
} wasm_module_t;

wasm_module_t *wasm_module_new(uint8_t *memory);
