#pragma once

#include <stdint.h>

#ifndef WASM_MAX_LOCALS_COUNT
#define WASM_MAX_LOCALS_COUNT 64
#endif

typedef struct {
  uint64_t locals[WASM_MAX_LOCALS_COUNT];
  uint16_t lp;
} wasm_locals_t;
