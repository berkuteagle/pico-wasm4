#pragma once

#include "pico/wasm/module.h"
#include <stdint.h>

void wasm_loader_load_module(wasm_module_t *module, const uint8_t *wasm,
                             const uint16_t wasm_size);
