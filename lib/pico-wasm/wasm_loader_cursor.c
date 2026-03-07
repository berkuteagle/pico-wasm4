#include "wasm_loader_cursor.h"
#include <stdbool.h>
#include <stdint.h>

inline wasm_loader_cursor_t
wasm_loader_cursor_create(const uint8_t *wasm, const uint16_t wasm_size) {
  return (wasm_loader_cursor_t){
      .current = wasm,
      .end = wasm + wasm_size,
  };
}

inline bool wasm_loader_cursor_has_next(wasm_loader_cursor_t *cursor) {
  return cursor->current < cursor->end;
}

inline uint8_t wasm_loader_cursor_next_u8(wasm_loader_cursor_t *cursor) {
  return *cursor->current++;
}

uint32_t wasm_loader_cursor_next_leb128(wasm_loader_cursor_t *cursor) {
  uint32_t result = 0;
  uint8_t shift = 0;

  while (true) {
    uint8_t b = wasm_loader_cursor_next_u8(cursor);

    result |= (b & 0x7f) << shift;

    if (!(b & 0x80))
      break;

    shift += 7;
  }

  return result;
}
