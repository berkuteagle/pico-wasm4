#include "pico/wasm.h"
#include "wasm_c_stack.h"
#include "wasm_locals.h"
#include <pico/wasm/c_stack.h>
#include <pico/wasm/module.h>
#include <stdint.h>
#include <stdlib.h>

wasm_vm_t *wasm_vm_new(wasm_module_t *module, wasm_host_t *host) {
  wasm_vm_t *vm_p = malloc(sizeof(wasm_vm_t));

  vm_p->module = module;
  vm_p->host = host;

  return vm_p;
}

void wasm_vm_func_call(wasm_vm_t *vm, wasm_func_t *func) {
  wasm_c_frame_t *frame = wasm_c_stack_push_frame(&vm->c_stack);

  frame->return_pc = vm->pc; //?
  frame->code_end = func->code + func->size;
  frame->l_count = func->locals;
  frame->l_base =
      func->locals ? wasm_locals_alloc(&vm->locals, func->locals) : 0;

  vm->pc = func->code;
}

void wasm_vm_func_return(wasm_vm_t *vm) {
  wasm_c_frame_t *frame = wasm_c_stack_pop_frame(&vm->c_stack);

  if (frame->l_count) {
    wasm_locals_free(&vm->locals, frame->l_count);
  }

  vm->pc = frame->return_pc;
}
