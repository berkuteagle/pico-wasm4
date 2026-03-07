#include "pico/wasm_loader.h"
#include "wasm_loader_cursor.h"
#include "wasm_loader_parser.h"
#include <stdint.h>

void wasm_loader_load_module(wasm_module_t *module, const uint8_t *wasm,
                             const uint16_t wasm_size) {

  wasm_loader_cursor_t cursor = wasm_loader_cursor_create(wasm, wasm_size);

  while (wasm_loader_cursor_has_next(&cursor)) {
    uint8_t section_id = wasm_loader_cursor_next_u8(&cursor);
    uint32_t section_size = wasm_loader_cursor_next_leb128(&cursor);

    const uint8_t *section_end =
        wasm_loader_cursor_forward(&cursor, section_size);

    switch (section_id) {
    case WASM_SECTION_TYPES:
      wasm_loader_parser_parse_types_section(&cursor, module);
      break;
    case WASM_SECTION_IMPORTS:
      wasm_loader_parser_parse_imports_section(&cursor, module);
      break;
    case WASM_SECTION_FUNCS:
      wasm_loader_parser_parse_funcs_section(&cursor, module);
      break;
    case WASM_SECTION_TABLE:
      wasm_loader_parser_parse_table_section(&cursor, module);
      break;
    case WASM_SECTION_EXPORTS:
      wasm_loader_parser_parse_exports_section(&cursor, module);
      break;
    case WASM_SECTION_ELEMENT:
      wasm_loader_parser_parse_element_section(&cursor, module);
      break;
    case WASM_SECTION_CODE:
      wasm_loader_parser_parse_code_section(&cursor, module);
      break;
    case WASM_SECTION_DATA:
      wasm_loader_parser_parse_data_section(&cursor, module);
      break;
    }

    wasm_loader_cursor_set_current(&cursor, section_end);
  }
}
