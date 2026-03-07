#pragma once

#include <stddef.h>
#include <stdint.h>

#ifndef WASM_C_STACK_SIZE
#define WASM_C_STACK_SIZE 128
#endif

typedef struct {
  const uint8_t *return_pc;
  const uint8_t *code_end;

  uint16_t l_base;
  uint16_t l_count;
} wasm_c_frame_t;

typedef struct {
  wasm_c_frame_t frames[WASM_C_STACK_SIZE];
  uint16_t fp;
} wasm_c_stack_t;
