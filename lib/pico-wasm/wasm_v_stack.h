#pragma once

#include "pico/wasm/v_stack.h"
#include <stdint.h>

void wasm_v_stack_push_i32(wasm_v_stack_t *stack, int32_t value);
void wasm_v_stack_push_i64(wasm_v_stack_t *stack, int64_t value);
void wasm_v_stack_push_f32(wasm_v_stack_t *stack, float value);
void wasm_v_stack_push_f64(wasm_v_stack_t *stack, double value);

int32_t wasm_v_stack_pop_i32(wasm_v_stack_t *stack);
int64_t wasm_v_stack_pop_i64(wasm_v_stack_t *stack);
float wasm_v_stack_pop_f32(wasm_v_stack_t *stack);
double wasm_v_stack_pop_f64(wasm_v_stack_t *stack);

int32_t wasm_v_stack_peek_i32(wasm_v_stack_t *stack);
int64_t wasm_v_stack_peek_i64(wasm_v_stack_t *stack);
float wasm_v_stack_peek_f32(wasm_v_stack_t *stack);
double wasm_v_stack_peek_f64(wasm_v_stack_t *stack);
