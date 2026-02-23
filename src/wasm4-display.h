#pragma once

#include "wasm4.h"
#include "pico/st7789/config.h"

#define SCREEN_BG 0x0000

#define LB_SIZE (W4_FB_W * 2)

#define OFFSET_X ((ST7789_SCREEN_W - W4_FB_W) / 2)
#define OFFSET_Y ((ST7789_SCREEN_H - W4_FB_H) / 2)

void w4_display_init(void);
void w4_display_clear(void);
void w4_display_update(const uint8_t *memory);
