#pragma once

#include "pico/wasm/c_stack.h"
#include "pico/wasm/func.h"
#include "pico/wasm/locals.h"
#include "pico/wasm/v_stack.h"
#include <stdint.h>

typedef struct {
  const uint8_t *pc;

  wasm_v_stack_t v_stack;
  wasm_c_stack_t c_stack;
  wasm_locals_t locals;

  // wasm_module_t module;
  // control_frame_t control_stack[32];
  // int csp;
} wasm_vm_t;

void wasm_vm_func_call(wasm_vm_t *vm, wasm_func_t *func);
void wasm_vm_func_return(wasm_vm_t *vm);

typedef int32_t (*host_func_t)(void *vm);

typedef struct {
  uint16_t param_count;
  uint16_t result_count;
} wasm_type_t;

typedef struct {
  uint8_t kind; // 0=wasm, 1=host

  union {
    wasm_func_t *wasm;
    host_func_t host;
  };
} wasm_func_ref_t;

typedef struct {
  uint8_t *ptr;
  uint8_t *end;
} wasm_reader_t;

typedef enum {
  IMPORT_FUNC = 0,
  IMPORT_TABLE = 1,
  IMPORT_MEMORY = 2,
  IMPORT_GLOBAL = 3
} wasm_import_kind_t;

typedef struct {
  const char *module;
  const char *name;

  uint16_t type_index;
} wasm_import_func_t;

typedef struct {
  const char *name;
  uint8_t kind;

  uint16_t index;
} wasm_export_t;

typedef struct {
  uint32_t entries[256];
  uint16_t size;
} wasm_table_t;

typedef struct {

  wasm_type_t types[32];
  uint16_t type_count;

  wasm_func_t funcs[128];
  uint16_t func_count;

  uint16_t func_types[128];

  wasm_table_t table;
  wasm_import_func_t imports[32];
  uint16_t import_count;

  wasm_export_t exports[32];
  uint16_t export_count;

  uint16_t start_func;

  wasm_func_ref_t func_table[160];
  uint16_t total_func_count;

  uint8_t *memory;
  uint32_t memory_size;
} wasm_module_t;

typedef struct {
  const char *ptr;
  uint32_t len;
} wasm_str_t;

typedef struct {
  const char *name;
  host_func_t fn;
} host_func_entry_t;

typedef union {
  uint32_t u32;
  int32_t i32;
  uint64_t u64;
  int64_t i64;
  float f32;
  double f64;
} wasm_val_t;

typedef struct {

  uint8_t *target;

  uint8_t type;

} control_frame_t;

#define CONTROL_STACK_SIZE 32
