#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  const uint8_t *current;
  const uint8_t *end;
} wasm_loader_cursor_t;

wasm_loader_cursor_t wasm_loader_cursor_create(const uint8_t *wasm,
                                               const uint16_t wasm_size);

bool wasm_loader_cursor_has_next(wasm_loader_cursor_t *cursor);

uint8_t wasm_loader_cursor_next_u8(wasm_loader_cursor_t *cursor);
uint32_t wasm_loader_cursor_next_leb128(wasm_loader_cursor_t *cursor);
