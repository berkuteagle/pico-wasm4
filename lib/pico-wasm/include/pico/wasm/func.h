#pragma once

#include <stdint.h>

typedef enum {
  WASM_HOST_FUNC_UNKNOWN,
  WASM_HOST_FUNC_BLIT,
  WASM_HOST_FUNC_BLITSUB,
  WASM_HOST_FUNC_LINE,
  WASM_HOST_FUNC_HLINE,
  WASM_HOST_FUNC_VLINE,
  WASM_HOST_FUNC_OVAL,
  WASM_HOST_FUNC_RECT,
  WASM_HOST_FUNC_TEXT,
  WASM_HOST_FUNC_TEXTUTF8,
  WASM_HOST_FUNC_TEXTUTF16,
  WASM_HOST_FUNC_TONE,
  WASM_HOST_FUNC_DISKR,
  WASM_HOST_FUNC_DISKW,
  WASM_HOST_FUNC_TRACE,
  WASM_HOST_FUNC_TRACEUTF8,
  WASM_HOST_FUNC_TRACEUTF16,
  WASM_HOST_FUNC_TRACEF,
} wasm_import_func_kind_t;

typedef enum {
  WASM_EXPORT_FUNC_UNKNOWN,
  WASM_EXPORT_FUNC_START,
  WASM_EXPORT_FUNC_UPDATE,
} wasm_export_func_kind_t;

typedef struct {
  uint16_t params;
  uint16_t results;
} wasm_type_t;

typedef struct {
  uint16_t type_idx;

  const uint8_t *code;
  uint16_t size;
  uint16_t locals;
} wasm_func_t;

typedef struct {
  uint16_t type_idx;
  wasm_import_func_kind_t kind;
} wasm_import_func_t;

typedef struct {
  uint16_t type_idx;
  wasm_export_func_kind_t kind;
} wasm_export_func_t;
