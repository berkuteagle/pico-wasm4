#pragma once

#include "pico/wasm/c_stack.h"
#include <stdbool.h>

wasm_c_frame_t *wasm_c_stack_push_frame(wasm_c_stack_t *stack);
wasm_c_frame_t *wasm_c_stack_pop_frame(wasm_c_stack_t *stack);
wasm_c_frame_t *wasm_c_stack_peek_frame(wasm_c_stack_t *stack);
bool wasm_c_stack_is_empty(wasm_c_stack_t *stack);
