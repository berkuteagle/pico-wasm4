#include "wasm_c_stack.h"

inline wasm_c_frame_t *wasm_c_stack_push_frame(wasm_c_stack_t *stack) {
  wasm_c_frame_t *frame = &stack->frames[stack->fp++];
  return frame;
}

inline wasm_c_frame_t *wasm_c_stack_pop_frame(wasm_c_stack_t *stack) {
  return stack->fp ? &stack->frames[--stack->fp] : NULL;
}

inline wasm_c_frame_t *wasm_c_stack_peek_frame(wasm_c_stack_t *stack) {
  return stack->fp ? &stack->frames[stack->fp - 1] : NULL;
}

inline bool wasm_c_stack_is_empty(wasm_c_stack_t *stack) {
  return stack->fp == 0;
}
