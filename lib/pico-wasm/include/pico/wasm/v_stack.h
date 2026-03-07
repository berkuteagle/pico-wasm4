#pragma once

#include <stdint.h>

#ifndef WASM_V_STACK_SIZE
#define WASM_V_STACK_SIZE 128
#endif

typedef struct {
  uint64_t stack[WASM_V_STACK_SIZE];
  uint16_t sp;
} wasm_v_stack_t;
