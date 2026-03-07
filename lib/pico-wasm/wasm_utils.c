#include "wasm_utils.h"
#include <stdint.h>
#include <string.h>

inline uint64_t wasm_f32_to_u64(float value) {
  uint64_t r;
  memcpy(&r, &value, sizeof(r));
  return r;
}

inline float wasm_u64_to_f32(uint64_t value) {
  float r;
  memcpy(&r, &value, sizeof(r));
  return r;
}

inline uint64_t wasm_f64_to_u64(double value) {
  uint64_t r;
  memcpy(&r, &value, sizeof(r));
  return r;
}

inline double wasm_u64_to_f64(uint64_t value) {
  double r;
  memcpy(&r, &value, sizeof(r));
  return r;
}
