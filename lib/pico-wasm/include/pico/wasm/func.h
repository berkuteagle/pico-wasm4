#pragma once

#include <stdint.h>

typedef struct {
  uint8_t *code;
  uint16_t size;

  uint16_t locals;
  uint16_t params;
  uint16_t jump[128];
} wasm_func_t;
