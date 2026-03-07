#include "wasm_loader_parser.h"
#include "pico/wasm/module.h"
#include "wasm_loader_cursor.h"
#include <pico/wasm/func.h>
#include <stdint.h>
#include <string.h>

void wasm_loader_parser_parse_types_section(wasm_loader_cursor_t *cursor,
                                            wasm_module_t *module) {
  uint32_t count = wasm_loader_cursor_next_leb128(cursor);

  for (uint32_t i = 0; i < count; i++) {
    wasm_loader_cursor_jump(cursor, 1); // skip read func type (0x60)

    uint16_t params = (uint16_t)wasm_loader_cursor_next_leb128(cursor);

    wasm_loader_cursor_jump(cursor, params); // skip read params types

    uint16_t results = (uint16_t)wasm_loader_cursor_next_leb128(cursor);

    wasm_loader_cursor_jump(cursor, results); // skip read results types

    wasm_type_t *type = &module->types[module->types_count++];

    type->params = params;
    type->results = results;
  }
}

void wasm_loader_parser_parse_imports_section(wasm_loader_cursor_t *cursor,
                                              wasm_module_t *module) {
  uint32_t count = wasm_loader_cursor_next_leb128(cursor);

  for (uint32_t i = 0; i < count; i++) {
    const char *m_name = wasm_loader_cursor_next_str(cursor);
    const char *f_name = wasm_loader_cursor_next_str(cursor);

    uint8_t kind = wasm_loader_cursor_next_u8(cursor);

    if (kind == WASM_IMPORT_FUNC) {
      uint32_t index = wasm_loader_cursor_next_leb128(cursor);

      wasm_import_func_t *func =
          &module->import_funcs[module->import_funcs_count++];

      func->type_idx = index;
      func->kind = WASM_HOST_FUNC_UNKNOWN;

      if (strcmp(m_name, "env")) {
        if (strcmp(f_name, "blit")) {
          func->kind = WASM_HOST_FUNC_BLIT;
        } else if (strcmp(f_name, "blitSub")) {
          func->kind = WASM_HOST_FUNC_BLITSUB;
        } else if (strcmp(f_name, "line")) {
          func->kind = WASM_HOST_FUNC_LINE;
        } else if (strcmp(f_name, "hline")) {
          func->kind = WASM_HOST_FUNC_HLINE;
        } else if (strcmp(f_name, "vline")) {
          func->kind = WASM_HOST_FUNC_VLINE;
        } else if (strcmp(f_name, "oval")) {
          func->kind = WASM_HOST_FUNC_OVAL;
        } else if (strcmp(f_name, "rect")) {
          func->kind = WASM_HOST_FUNC_RECT;
        } else if (strcmp(f_name, "text")) {
          func->kind = WASM_HOST_FUNC_TEXT;
        } else if (strcmp(f_name, "textUtf8")) {
          func->kind = WASM_HOST_FUNC_TEXTUTF8;
        } else if (strcmp(f_name, "textUtf16")) {
          func->kind = WASM_HOST_FUNC_TEXTUTF16;
        } else if (strcmp(f_name, "tone")) {
          func->kind = WASM_HOST_FUNC_TONE;
        } else if (strcmp(f_name, "diskr")) {
          func->kind = WASM_HOST_FUNC_DISKR;
        } else if (strcmp(f_name, "diskw")) {
          func->kind = WASM_HOST_FUNC_DISKW;
        } else if (strcmp(f_name, "trace")) {
          func->kind = WASM_HOST_FUNC_TRACE;
        } else if (strcmp(f_name, "traceUtf8")) {
          func->kind = WASM_HOST_FUNC_TRACEUTF8;
        } else if (strcmp(f_name, "traceUtf16")) {
          func->kind = WASM_HOST_FUNC_TRACEUTF16;
        } else if (strcmp(f_name, "tracef")) {
          func->kind = WASM_HOST_FUNC_TRACEF;
        }
      }
    }
  }
}

void wasm_loader_parser_parse_funcs_section(wasm_loader_cursor_t *cursor,
                                            wasm_module_t *module) {
  uint32_t count = wasm_loader_cursor_next_leb128(cursor);

  for (uint32_t i = 0; i < count; i++) {
    uint32_t index = wasm_loader_cursor_next_leb128(cursor);

    wasm_func_t *func = &module->funcs[module->funcs_count++];

    func->type_idx = index;
  }
}

void wasm_loader_parser_parse_table_section(wasm_loader_cursor_t *cursor,
                                            wasm_module_t *module) {
  uint32_t count = wasm_loader_cursor_next_leb128(cursor);

  for (uint32_t i = 0; i < count; i++) {
    wasm_loader_cursor_jump(cursor, 1); // skip read table type (0x70 = funcref)

    uint8_t flags = wasm_loader_cursor_next_u8(cursor);

    uint32_t min = wasm_loader_cursor_next_leb128(cursor);
    uint32_t max = min;

    if (flags & 1)
      max = wasm_loader_cursor_next_leb128(cursor);

    module->table.size = min;
  }
}

void wasm_loader_parser_parse_exports_section(wasm_loader_cursor_t *cursor,
                                              wasm_module_t *module) {
  uint32_t count = wasm_loader_cursor_next_leb128(cursor);

  for (uint32_t i = 0; i < count; i++) {
    const char *f_name = wasm_loader_cursor_next_str(cursor);

    uint8_t kind = wasm_loader_cursor_next_u8(cursor);
    uint32_t index = wasm_loader_cursor_next_leb128(cursor);

    wasm_export_func_t *func =
        &module->export_funcs[module->export_funcs_count++];

    func->type_idx = index;
    func->kind = WASM_EXPORT_FUNC_UNKNOWN;

    if (strcmp(f_name, "start")) {
      func->kind = WASM_EXPORT_FUNC_UPDATE;
    } else if (strcmp(f_name, "update")) {
      func->kind = WASM_EXPORT_FUNC_UPDATE;
    }
  }
}

void wasm_loader_parser_parse_element_section(wasm_loader_cursor_t *cursor,
                                              wasm_module_t *module) {
  uint32_t count = wasm_loader_cursor_next_leb128(cursor);

  for (uint32_t i = 0; i < count; i++) {
    uint32_t flags = wasm_loader_cursor_next_leb128(cursor);

    if (flags != 0)
      continue; // пока игнорируем другие типы

    uint8_t opcode = wasm_loader_cursor_next_u8(cursor); // i32.const
    uint32_t offset = wasm_loader_cursor_next_leb128(cursor);
    wasm_loader_cursor_jump(cursor, 1); // end

    uint32_t func_count = wasm_loader_cursor_next_leb128(cursor);

    for (uint32_t j = 0; j < func_count; j++) {
      uint32_t f = wasm_loader_cursor_next_leb128(cursor);

      module->table.entries[offset + j] = f;
    }
  }
}

void wasm_loader_parser_parse_code_section(wasm_loader_cursor_t *cursor,
                                           wasm_module_t *module) {

  uint32_t count = wasm_loader_cursor_next_leb128(cursor);

  for (uint32_t i = 0; i < count; i++) {
    uint32_t size = wasm_loader_cursor_next_leb128(cursor);
    uint32_t locals_count = wasm_loader_cursor_next_leb128(cursor);

    uint32_t locals = 0;

    for (uint32_t l = 0; l < locals_count; l++) {
      uint32_t n = wasm_loader_cursor_next_leb128(cursor);
      wasm_loader_cursor_jump(cursor, 1);
      locals += n;
    }

    wasm_func_t *func = &module->funcs[i];

    func->code = wasm_loader_cursor_get_current(cursor);
    func->size = size;
    func->locals = locals;

    wasm_loader_cursor_jump(cursor, size);
  }
}

void wasm_loader_parser_parse_data_section(wasm_loader_cursor_t *cursor,
                                           wasm_module_t *module) {
  uint32_t count = wasm_loader_cursor_next_leb128(cursor);

  for (uint32_t i = 0; i < count; i++) {
    uint32_t flags = wasm_loader_cursor_next_leb128(cursor);

    if (flags != 0)
      continue; // пока игнорируем другие типы

    uint8_t opcode = wasm_loader_cursor_next_u8(cursor); // i32.const
    uint32_t offset = wasm_loader_cursor_next_leb128(cursor);
    wasm_loader_cursor_jump(cursor, 1); // end

    uint32_t size = wasm_loader_cursor_next_leb128(cursor);

    memcpy(module->memory + offset, wasm_loader_cursor_get_current(cursor), size);

    wasm_loader_cursor_jump(cursor, size);
  }
}
