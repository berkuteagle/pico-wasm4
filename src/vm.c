#include "vm.h"
#include "bh_common.h"
#include "wasm_export.h"
#include <stdio.h>

static uint8_t wamr_heap[WASM_GLOBAL_HEAP_SIZE] = {0};
static char vm_error_buf[VM_ERROR_BUF_SIZE] = {0};

static wasm_module_t vm_module;
static wasm_module_inst_t vm_module_inst;
static wasm_exec_env_t vm_module_inst_exec_env;
static wasm_function_inst_t vm_update_fn;

static void pong_native(wasm_exec_env_t exec_env, int32_t value) {
  (void)exec_env;
  printf("[pong] received %d\n", (int)value);
}

static NativeSymbol native_symbols[] = {
    {"pong", pong_native, "(i)", NULL},
};

bool vm_init() {
  if (!wasm_runtime_init()) {
    printf("wasm_runtime_init failed\n");
    return false;
  }

  RuntimeInitArgs init_args = {0};
  init_args.mem_alloc_type = Alloc_With_Pool;
  init_args.mem_alloc_option.pool.heap_buf = wamr_heap;
  init_args.mem_alloc_option.pool.heap_size = WASM_GLOBAL_HEAP_SIZE;

  if (!wasm_runtime_full_init(&init_args)) {
    printf("wasm_runtime_full_init failed\n");
    return false;
  }

  if (!wasm_runtime_register_natives("env", native_symbols, 1)) {
    printf("wasm_runtime_register_natives failed\n");
    return false;
  }

  return true;
}

void vm_destroy() { wasm_runtime_destroy(); }

char *vm_get_error() { return vm_error_buf; }

bool vm_load_module(uint8_t *data, size_t data_len) {
  vm_module =
      wasm_runtime_load(data, data_len, vm_error_buf, VM_ERROR_BUF_SIZE);

  if (!vm_module) {
    printf("wasm_runtime_load failed\n");
    return false;
  }

  vm_module_inst = wasm_runtime_instantiate(vm_module, VM_STACK_SIZE, 0,
                                            vm_error_buf, VM_ERROR_BUF_SIZE);

  if (!vm_module_inst) {
    printf("wasm_runtime_instantiate failed\n");
    return false;
  }

  vm_update_fn = wasm_runtime_lookup_function(vm_module_inst, "update");

  if (!vm_update_fn) {
    printf("update not found\n");
    return false;
  }

  vm_module_inst_exec_env =
      wasm_runtime_create_exec_env(vm_module_inst, VM_STACK_SIZE);

  if (!vm_module_inst_exec_env) {
    printf("wasm_runtime_create_exec_env failed\n");
    return false;
  }

  return true;
}

void vm_unload_module() {
  vm_update_fn = NULL;

  if (vm_module_inst_exec_env) {
    wasm_runtime_destroy_exec_env(vm_module_inst_exec_env);
    vm_module_inst_exec_env = NULL;
  }

  if (vm_module_inst) {
    wasm_runtime_deinstantiate(vm_module_inst);
    vm_module_inst = NULL;
  }

  if (vm_module) {
    wasm_runtime_unload(vm_module);
    vm_module = NULL;
  }
}

void vm_update() {
  if (vm_update_fn && vm_module_inst_exec_env) {
    wasm_val_t args[1] = {{.kind = WASM_I32, .of.i32 = 100}};

    wasm_runtime_call_wasm_a(vm_module_inst_exec_env, vm_update_fn, 0, NULL, 1,
                             args);
  }
}
