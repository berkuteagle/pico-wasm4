#pragma once

#include <stdint.h>

typedef struct {
  uint32_t entries[256];
  uint16_t size;
} wasm_table_t;
