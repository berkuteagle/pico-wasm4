#include "wasm_v_stack.h"
#include "wasm_utils.h"
#include <stdint.h>

static inline void wasm_v_push_raw(wasm_v_stack_t *s, uint64_t v) {
  s->stack[s->sp++] = v;
}

static inline uint64_t wasm_v_pop_raw(wasm_v_stack_t *s) {
  return s->stack[--s->sp];
}

static inline uint64_t wasm_v_peek_raw(wasm_v_stack_t *s) {
  return s->stack[s->sp - 1];
}

void wasm_v_stack_push_i32(wasm_v_stack_t *stack, int32_t value) {
  wasm_v_push_raw(stack, (uint64_t)(uint32_t)value);
}

void wasm_v_stack_push_i64(wasm_v_stack_t *stack, int64_t value) {
  wasm_v_push_raw(stack, (uint64_t)value);
}

void wasm_v_stack_push_f32(wasm_v_stack_t *stack, float value) {
  wasm_v_push_raw(stack, wasm_f32_to_u64(value));
}
void wasm_v_stack_push_f64(wasm_v_stack_t *stack, double value) {
  wasm_v_push_raw(stack, wasm_f64_to_u64(value));
}

int32_t wasm_v_stack_pop_i32(wasm_v_stack_t *stack) {
  return (int32_t)wasm_v_pop_raw(stack);
}

int64_t wasm_v_stack_pop_i64(wasm_v_stack_t *stack) {

  return (int64_t)wasm_v_pop_raw(stack);
}

float wasm_v_stack_pop_f32(wasm_v_stack_t *stack) {
  return wasm_u64_to_f32(wasm_v_pop_raw(stack));
}

double wasm_v_stack_pop_f64(wasm_v_stack_t *stack) {
  return wasm_u64_to_f64(wasm_v_pop_raw(stack));
}

int32_t wasm_v_stack_peek_i32(wasm_v_stack_t *stack) {
  return (int32_t)wasm_v_peek_raw(stack);
}

int64_t wasm_v_stack_peek_i64(wasm_v_stack_t *stack) {
  return (int64_t)wasm_v_peek_raw(stack);
}

float wasm_v_stack_peek_f32(wasm_v_stack_t *stack) {
  return wasm_u64_to_f32(wasm_v_peek_raw(stack));
}

double wasm_v_stack_peek_f64(wasm_v_stack_t *stack) {
  return wasm_u64_to_f64(wasm_v_peek_raw(stack));
}
