#include "wasm_locals.h"
#include "wasm_utils.h"
#include <stdint.h>
#include <string.h>

void wasm_locals_init(wasm_locals_t *pool) { pool->lp = 0; }

uint16_t wasm_locals_alloc(wasm_locals_t *pool, uint16_t count) {
  uint16_t base = pool->lp;
  memset(pool->locals + pool->lp, 0, count * sizeof(uint64_t));
  pool->lp += count;
  return base;
}

void wasm_locals_free(wasm_locals_t *pool, uint16_t count) {
  pool->lp -= count;
}

int32_t wasm_locals_get_i32(wasm_locals_t *pool, uint16_t base,
                            uint16_t index) {
  return (int32_t)pool->locals[base + index];
}

int64_t wasm_locals_get_i64(wasm_locals_t *pool, uint16_t base,
                            uint16_t index) {
  return (int64_t)pool->locals[base + index];
}

float wasm_locals_get_f32(wasm_locals_t *pool, uint16_t base,
                          uint16_t index) {
  return wasm_u64_to_f32(pool->locals[base + index]);
}

double wasm_locals_get_f64(wasm_locals_t *pool, uint16_t base,
                           uint16_t index) {
  return wasm_u64_to_f64(pool->locals[base + index]);
}

void wasm_locals_set_i32(wasm_locals_t *pool, uint16_t base,
                         uint16_t index, int32_t value) {
  pool->locals[base + index] = (uint64_t)value;
}

void wasm_locals_set_i64(wasm_locals_t *pool, uint16_t base,
                         uint16_t index, int64_t value) {
  pool->locals[base + index] = (uint64_t)value;
}

void wasm_locals_set_f32(wasm_locals_t *pool, uint16_t base,
                         uint16_t index, float value) {
  pool->locals[base + index] = wasm_f32_to_u64(value);
}

void wasm_locals_set_f64(wasm_locals_t *pool, uint16_t base,
                         uint16_t index, double value) {
  pool->locals[base + index] = wasm_f64_to_u64(value);
}
