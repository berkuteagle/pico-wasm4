#include "pico/wasm/module.h"
#include <stdlib.h>

wasm_module_t *wasm_module_new(uint8_t *memory) {
  wasm_module_t *module_p = malloc(sizeof(wasm_module_t));

  module_p->memory = memory;

  return module_p;
}
