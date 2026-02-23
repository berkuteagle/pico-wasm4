#include <stdint.h>
#include "wasm4-display.h"
#include "pico/st7789.h"
#include "wasm4.h"

static uint8_t line_buffer[LB_SIZE];
static uint16_t palette_rgb565[4];

static uint16_t rgb888_to_565(const uint8_t r, const uint8_t g,
                              const uint8_t b)
{
    return (uint16_t)((r >> 3) << 11) | (g >> 2) << 5 | b >> 3;
}

void w4_display_init()
{
    st7789_init();
    st7789_fill(SCREEN_BG);
}

void w4_display_clear()
{
    st7789_fill(SCREEN_BG);
}

void w4_display_update(const uint8_t *memory)
{
    const uint8_t *framebuffer = memory + W4_FRAMEBUFFER_OFFSET;
    const uint32_t *palette = (const uint32_t *)(memory + W4_PALETTE_OFFSET);

    st7789_set_window(OFFSET_X, OFFSET_Y, OFFSET_X + W4_FB_W - 1,
                      OFFSET_Y + W4_FB_H - 1);

    st7789_dc_data();

    const uint8_t *pal = (uint8_t *)(palette);

    // Convert palette from rgb888 to rgb565
    for (int i = 0; i < 4; i++)
    {
        palette_rgb565[i] =
            rgb888_to_565(pal[i * 4 + 2], pal[i * 4 + 1], pal[i * 4]);
    }

    for (int line_number = 0; line_number < W4_FB_H; line_number++)
    {

        uint8_t *dst = line_buffer;

        for (int x = 0; x < W4_FB_W; x++)
        {
            const int pixel_index = line_number * W4_FB_W + x;
            const int byte_index = pixel_index >> 2;
            const int shift = (pixel_index & 3) * 2;

            const uint8_t color_index = (framebuffer[byte_index] >> shift) & 0x03;
            const uint16_t color = palette_rgb565[color_index];

            *dst++ = color >> 8;
            *dst++ = color & 0xFF;
        }

        st7789_spi_write(line_buffer, LB_SIZE);
    }
}
