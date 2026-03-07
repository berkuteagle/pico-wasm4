#pragma once

#include "pico/wasm/locals.h"
#include <stdint.h>

void wasm_locals_init(wasm_locals_t *pool);
uint16_t wasm_locals_alloc(wasm_locals_t *pool, uint16_t count);
void wasm_locals_free(wasm_locals_t *pool, uint16_t count);

int32_t wasm_locals_get_i32(wasm_locals_t *pool, uint16_t base,
                            uint16_t index);
int64_t wasm_locals_get_i64(wasm_locals_t *pool, uint16_t base,
                            uint16_t index);
float wasm_locals_get_f32(wasm_locals_t *pool, uint16_t base,
                          uint16_t index);
double wasm_locals_get_f64(wasm_locals_t *pool, uint16_t base,
                           uint16_t index);

void wasm_locals_set_i32(wasm_locals_t *pool, uint16_t base,
                         uint16_t index, int32_t value);
void wasm_locals_set_i64(wasm_locals_t *pool, uint16_t base,
                         uint16_t index, int64_t value);
void wasm_locals_set_f32(wasm_locals_t *pool, uint16_t base,
                         uint16_t index, float value);
void wasm_locals_set_f64(wasm_locals_t *pool, uint16_t base,
                         uint16_t index, double value);
