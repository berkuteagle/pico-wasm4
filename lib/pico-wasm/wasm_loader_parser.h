#pragma once

#include "wasm_loader_cursor.h"
#include <stdbool.h>

bool wasm_loader_parser_parse_header(wasm_loader_cursor_t *cursor);
