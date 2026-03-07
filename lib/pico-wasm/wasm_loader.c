#include "pico/wasm_loader.h"
#include "wasm_loader_cursor.h"
#include "wasm_loader_parser.h"
#include <stdbool.h>

bool wasm_loader_load_module(wasm_module_t *module, const uint8_t *wasm,
                             const uint16_t wasm_size) {

  wasm_loader_cursor_t cursor = wasm_loader_cursor_create(wasm, wasm_size);

  if (!wasm_loader_parser_parse_header(&cursor)) {
    return false;
  }

  while (wasm_loader_cursor_has_next(&cursor)) {
      switch (wasm_loader_cursor_next_u8(&cursor)) {
          case 0x01:
              break;
      }
  }

  return true;
}
