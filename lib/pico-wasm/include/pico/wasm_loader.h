#pragma once

#include "pico/wasm.h"
#include <stdbool.h>
#include <stdint.h>

bool wasm_loader_load_module(wasm_module_t *module, const uint8_t *wasm,
                             const uint16_t wasm_size);
