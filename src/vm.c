#include "vm.h"
#include "bh_common.h"
#include "wasm_export.h"
#include <stdio.h>

static uint8 wamr_heap[WASM_GLOBAL_HEAP_SIZE] = {0};
static char vm_error_buf[VM_ERROR_BUF_SIZE] = {0};

static uint8 wasm_linear_memory[WASM_GLOBAL_HEAP_SIZE] = {0};

static wasm_module_t vm_module;
static wasm_module_inst_t vm_module_inst;
static wasm_exec_env_t vm_module_inst_exec_env;

static wasm_function_inst_t vm_update_fn;
static wasm_function_inst_t vm_start_fn;

static void blit_native(wasm_exec_env_t exec_env, uint8 *sprite, int32 x, int32 y, int32 width, int32 height, int32 flags)
{ // 6*i
  (void)exec_env;
}

static void blit_sub_native(wasm_exec_env_t exec_env, uint8 *sprite, int32 x, int32 y, int32 width, int32 height, int32 src_x, int32 src_y, int32 stride, int32 flags)
{ // 9*i
  (void)exec_env;
}

static void line_native(wasm_exec_env_t exec_env, int32 a_x, int32 a_y, int32 b_x, int32 b_y)
{ // 4*i
  (void)exec_env;
}

static void hline_native(wasm_exec_env_t exec_env, int32 x, int32 y, int32 length)
{ // 3*i
  (void)exec_env;
}

static void vline_native(wasm_exec_env_t exec_env, int32 x, int32 y, int32 length)
{ // 3*i
  (void)exec_env;
}

static void oval_native(wasm_exec_env_t exec_env, int32 x, int32 y, int32 width, int32 height)
{ // 4*i
  (void)exec_env;
}

static void rect_native(wasm_exec_env_t exec_env, int32 x, int32 y, int32 width, int32 height)
{ // 4*i
  (void)exec_env;
}

static void text_native(wasm_exec_env_t exec_env, char *str, int32 x, int32 y)
{ // 3*i
  (void)exec_env;
}

static void text_utf8_native(wasm_exec_env_t exec_env, char *str, int32 byte_length, int32 x, int32 y)
{ // 4*i
  (void)exec_env;
}

static void text_utf16_native(wasm_exec_env_t exec_env, char *str, int32 byte_length, int32 x, int32 y)
{ // 4*i
  (void)exec_env;
}

static void tone_native(wasm_exec_env_t exec_env, int32 frequency, int32 duration, int32 volume, int32 flags)
{ // 4*i
  (void)exec_env;
}

static int32 diskr_native(wasm_exec_env_t exec_env, uint8 *dst, int32 size)
{ // 2*i -> i
  (void)exec_env;
  return size;
}

static int32 diskw_native(wasm_exec_env_t exec_env, uint8 *src, int32 size)
{ // 2*i -> i
  (void)exec_env;
  return size;
}

static void trace_native(wasm_exec_env_t exec_env, char *str)
{ // i
  (void)exec_env;
}

static void trace_utf8_native(wasm_exec_env_t exec_env, char *str, int32 byte_length)
{ // 2*i
  (void)exec_env;
}

static void trace_utf16_native(wasm_exec_env_t exec_env, char *str, int32 byte_length)
{ // 2*i
  (void)exec_env;
}

static void tracef_native(wasm_exec_env_t exec_env, char *str, void *stack)
{ // 2*i
  (void)exec_env;
}

static NativeSymbol native_symbols[] = {
    {"blit", blit_native, "(*iiiii)", NULL},
    {"blitSub", blit_sub_native, "(iiiiiiiii)", NULL},
    {"line", line_native, "(iiiiiiiii)", NULL},
    {"hline", hline_native, "(iii)", NULL},
    {"vline", vline_native, "(iii)", NULL},
    {"oval", oval_native, "(iiii)", NULL},
    {"rect", rect_native, "(iiii)", NULL},
    {"text", text_native, "($ii)", NULL},
    {"textUtf8", text_utf8_native, "($iii)", NULL},
    {"textUtf16", text_utf16_native, "($iii)", NULL},
    {"tone", tone_native, "(iiii)", NULL},
    {"diskr", diskr_native, "(*i)i", NULL},
    {"diskw", diskw_native, "(*i)i", NULL},
    {"trace", trace_native, "(*)", NULL},
    {"traceUtf8", trace_utf8_native, "(*i)", NULL},
    {"traceUtf16", trace_utf16_native, "(*i)", NULL},
    {"tracef", tracef_native, "(**)", NULL},
};

bool vm_init()
{
  if (!wasm_runtime_init())
  {
    printf("wasm_runtime_init failed\n");
    return false;
  }

  RuntimeInitArgs init_args = {0};
  init_args.mem_alloc_type = Alloc_With_Pool;
  init_args.mem_alloc_option.pool.heap_buf = wamr_heap;
  init_args.mem_alloc_option.pool.heap_size = WASM_GLOBAL_HEAP_SIZE;

  if (!wasm_runtime_full_init(&init_args))
  {
    printf("wasm_runtime_full_init failed\n");
    return false;
  }

  if (!wasm_runtime_register_natives("env", native_symbols, 1))
  {
    printf("wasm_runtime_register_natives failed\n");
    return false;
  }

  return true;
}

void vm_destroy() { wasm_runtime_destroy(); }

char *vm_get_error() { return vm_error_buf; }

bool vm_load_module(uint8_t *data, size_t data_len)
{
  vm_module =
      wasm_runtime_load(data, data_len, vm_error_buf, VM_ERROR_BUF_SIZE);

  if (!vm_module)
  {
    printf("wasm_runtime_load failed\n");
    return false;
  }

  vm_module_inst = wasm_runtime_instantiate(vm_module, VM_STACK_SIZE, 0,
                                            vm_error_buf, VM_ERROR_BUF_SIZE);

  if (!vm_module_inst)
  {
    printf("wasm_runtime_instantiate failed\n");
    return false;
  }

  vm_update_fn = wasm_runtime_lookup_function(vm_module_inst, "update");
  vm_start_fn = wasm_runtime_lookup_function(vm_module_inst, "start");

  if (!vm_update_fn)
  {
    printf("update not found\n");
    return false;
  }

  vm_module_inst_exec_env =
      wasm_runtime_create_exec_env(vm_module_inst, VM_STACK_SIZE);

  if (!vm_module_inst_exec_env)
  {
    printf("wasm_runtime_create_exec_env failed\n");
    return false;
  }

  wasm_function_inst_t init_fn = wasm_runtime_lookup_function(vm_module_inst, "_start");

  if (init_fn)
  {
    if (!wasm_runtime_call_wasm(vm_module_inst_exec_env, init_fn, 0, NULL))
    {
      printf("wasm_runtime_call_wasm _start failed\n");
      return false;
    }
  }

  init_fn = wasm_runtime_lookup_function(vm_module_inst, "_initialize");

  if (init_fn)
  {
    if (!wasm_runtime_call_wasm(vm_module_inst_exec_env, init_fn, 0, NULL))
    {
      printf("wasm_runtime_call_wasm _initialize failed\n");
      return false;
    }
  }

  init_fn = NULL;

  return true;
}

void vm_unload_module()
{
  vm_update_fn = NULL;

  if (vm_module_inst_exec_env)
  {
    wasm_runtime_destroy_exec_env(vm_module_inst_exec_env);
    vm_module_inst_exec_env = NULL;
  }

  if (vm_module_inst)
  {
    wasm_runtime_deinstantiate(vm_module_inst);
    vm_module_inst = NULL;
  }

  if (vm_module)
  {
    wasm_runtime_unload(vm_module);
    vm_module = NULL;
  }
}

void vm_update()
{

  if (vm_start_fn && vm_module_inst_exec_env)
  {
    if (!wasm_runtime_call_wasm(vm_module_inst_exec_env, vm_start_fn, 0, NULL))
    {
      printf("wasm_runtime_call_wasm start failed\n");
      return;
    }
  }

  if (vm_update_fn && vm_module_inst_exec_env)
  {
    if (!wasm_runtime_call_wasm(vm_module_inst_exec_env, vm_update_fn, 0, NULL))
    {
      printf("wasm_runtime_call_wasm update failed\n");
      return;
    }
  }
}
