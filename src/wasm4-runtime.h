#pragma once

#include <stdint.h>
#include "pico/m3.h"

#define M3_RUNTIME_STACK_SIZE 16384

uint8_t *w4_runtime_init(void);
void w4_runtime_load_wasm(const uint8_t *const wasm_data, uint32_t wasm_data_size);
void w4_runtime_update(uint8_t *memory);
