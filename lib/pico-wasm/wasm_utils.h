#pragma once

#include <stdint.h>

uint64_t wasm_f32_to_u64(float value);
float wasm_u64_to_f32(uint64_t value);
uint64_t wasm_f64_to_u64(double value);
double wasm_u64_to_f64(uint64_t value);
