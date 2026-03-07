#pragma once

#include "pico/wasm/module.h"
#include "wasm_loader_cursor.h"

#define WASM_SECTION_CUSTOM 0x00
#define WASM_SECTION_TYPES 0x01
#define WASM_SECTION_IMPORTS 0x02
#define WASM_SECTION_FUNCS 0x03
#define WASM_SECTION_TABLE 0x04
// 0x05
// 0x06
#define WASM_SECTION_EXPORTS 0x07
// 0x08
#define WASM_SECTION_ELEMENT 0x09
#define WASM_SECTION_CODE 0x0a
#define WASM_SECTION_DATA 0x0b
// 0x0c

#define WASM_IMPORT_FUNC 0x00
#define WASM_IMPORT_TABLE 0x01
#define WASM_IMPORT_MEMORY 0x02
#define WASM_IMPORT_GLOBAL 0x03

void wasm_loader_parser_parse_types_section(wasm_loader_cursor_t *cursor,
                                            wasm_module_t *module);
void wasm_loader_parser_parse_imports_section(wasm_loader_cursor_t *cursor,
                                              wasm_module_t *module);
void wasm_loader_parser_parse_funcs_section(wasm_loader_cursor_t *cursor,
                                            wasm_module_t *module);
void wasm_loader_parser_parse_table_section(wasm_loader_cursor_t *cursor,
                                            wasm_module_t *module);
void wasm_loader_parser_parse_exports_section(wasm_loader_cursor_t *cursor,
                                              wasm_module_t *module);
void wasm_loader_parser_parse_element_section(wasm_loader_cursor_t *cursor,
                                              wasm_module_t *module);
void wasm_loader_parser_parse_code_section(wasm_loader_cursor_t *cursor,
                                           wasm_module_t *module);
void wasm_loader_parser_parse_data_section(wasm_loader_cursor_t *cursor,
                                           wasm_module_t *module);
