#pragma once

#include <stdint.h>

/**
 * m3_LinkRawFunction(module, "env", "blit", "v(iiiiii)", blit);
     m3_LinkRawFunction(module, "env", "blitSub", "v(iiiiiiiii)", blitSub);
     m3_LinkRawFunction(module, "env", "line", "v(iiii)", line);
     m3_LinkRawFunction(module, "env", "hline", "v(iii)", hline);
     m3_LinkRawFunction(module, "env", "vline", "v(iii)", vline);
     m3_LinkRawFunction(module, "env", "oval", "v(iiii)", oval);
     m3_LinkRawFunction(module, "env", "rect", "v(iiii)", rect);
     m3_LinkRawFunction(module, "env", "text", "v(iii)", text);
     m3_LinkRawFunction(module, "env", "textUtf8", "v(iiii)", textUtf8);
     m3_LinkRawFunction(module, "env", "textUtf16", "v(iiii)", textUtf16);

     m3_LinkRawFunction(module, "env", "tone", "v(iiii)", tone);

     m3_LinkRawFunction(module, "env", "diskr", "i(ii)", diskr);
     m3_LinkRawFunction(module, "env", "diskw", "i(ii)", diskw);

     m3_LinkRawFunction(module, "env", "trace", "v(i)", trace);
     m3_LinkRawFunction(module, "env", "traceUtf8", "v(ii)", traceUtf8);
     m3_LinkRawFunction(module, "env", "traceUtf16", "v(ii)", traceUtf16);
     m3_LinkRawFunction(module, "env", "tracef", "v(ii)", tracef);
 */

typedef struct {
  void (*blit)(uint8_t *memory);
  void (*blitSub)(uint8_t *memory);
  void (*line)(uint8_t *memory);
  void (*hline)(uint8_t *memory);
  void (*vline)(uint8_t *memory);
  void (*oval)(uint8_t *memory);
  void (*rect)(uint8_t *memory);
  void (*text)(uint8_t *memory);
  void (*textUtf8)(uint8_t *memory);
  void (*textUtf16)(uint8_t *memory);
  void (*tone)(uint8_t *memory);
  uint32_t (*diskr)(uint8_t *memory);
  uint32_t (*diskw)(uint8_t *memory);
  void (*trace)(uint8_t *memory);
  void (*traceUtf8)(uint8_t *memory);
  void (*traceUtf16)(uint8_t *memory);
  void (*tracef)(uint8_t *memory);
} wasm_host_t;
