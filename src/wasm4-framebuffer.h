#pragma once

#include <stdint.h>
#include <stdbool.h>

void w4_framebuffer_blit(uint8_t *framebuffer, const uint8_t *const draw_colors, const uint8_t *sprite, int dstX, int dstY, int width,
                         int height, int srcX, int srcY, int srcStride,
                         bool bpp2, bool flipX, bool flipY, bool rotate);
void w4_framebuffer_text(uint8_t *framebuffer, const uint8_t *const draw_colors, const uint8_t *str, uint32_t x, uint32_t y);
void w4_framebuffer_text_utf8(uint8_t *framebuffer, const uint8_t *const draw_colors, const uint8_t *str, uint32_t byteLength, uint32_t x, uint32_t y);
void w4_framebuffer_text_utf16(uint8_t *framebuffer, const uint8_t *const draw_colors, const uint16_t *str, uint32_t byteLength, uint32_t x, uint32_t y);
void w4_framebuffer_line(uint8_t *framebuffer, const uint8_t *const draw_colors, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
void w4_framebuffer_hline(uint8_t *framebuffer, const uint8_t *const draw_colors, uint32_t x, uint32_t y, uint32_t len);
void w4_framebuffer_vline(uint8_t *framebuffer, const uint8_t *const draw_colors, uint32_t x, uint32_t y, uint32_t len);
void w4_framebuffer_rect(uint8_t *framebuffer, const uint8_t *const draw_colors, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void w4_framebuffer_oval(uint8_t *framebuffer, const uint8_t *const draw_colors, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
