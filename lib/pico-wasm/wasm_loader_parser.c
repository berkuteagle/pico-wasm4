#include "wasm_loader_parser.h"
#include "wasm_loader_cursor.h"
#include <stdbool.h>
#include <stdint.h>

#define MAGIC_LENGTH 8

static const uint8_t MAGIC[MAGIC_LENGTH] = {0x00, 0x61, 0x73, 0x6D,
                                            0x01, 0x00, 0x00, 0x00};

inline bool wasm_loader_parser_parse_header(wasm_loader_cursor_t *cursor) {
  uint8_t index = 0;

  while (wasm_loader_cursor_has_next(cursor) && index < MAGIC_LENGTH) {
    if (MAGIC[index] != wasm_loader_cursor_next_u8(cursor)) {
      return false;
    }

    index++;
  }

  return true;
}
