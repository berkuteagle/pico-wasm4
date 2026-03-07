#pragma once

#include <stdbool.h>
#include <stdint.h>

#define MAGIC_LENGTH 8

typedef struct {
  const uint8_t *current;
  const uint8_t *end;
} wasm_loader_cursor_t;

wasm_loader_cursor_t wasm_loader_cursor_create(const uint8_t *wasm,
                                               const uint16_t wasm_size);
const uint8_t *wasm_loader_cursor_get_current(wasm_loader_cursor_t *cursor);
void wasm_loader_cursor_set_current(wasm_loader_cursor_t *cursor,
                                    const uint8_t *current);
const uint8_t *wasm_loader_cursor_forward(wasm_loader_cursor_t *cursor,
                                          uint32_t n);
void wasm_loader_cursor_jump(wasm_loader_cursor_t *cursor, uint32_t n);
bool wasm_loader_cursor_has_next(wasm_loader_cursor_t *cursor);
uint8_t wasm_loader_cursor_next_u8(wasm_loader_cursor_t *cursor);
uint32_t wasm_loader_cursor_next_leb128(wasm_loader_cursor_t *cursor);
const char *wasm_loader_cursor_next_str(wasm_loader_cursor_t *cursor);
