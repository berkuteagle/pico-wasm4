#pragma once

#include <stddef.h>
#include <stdint.h>

void st7789_init(void);
void st7789_fill(uint16_t color);
void st7789_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void st7789_fill_window(const uint8_t *line_buffer, size_t line_buffer_size, uint8_t window_height,
                        void (*line_callback)(uint8_t line_number, void *buffer), void *buffer);
void st7789_dc_cmd(void);
void st7789_dc_data(void);
void st7789_spi_write(const uint8_t *buffer, size_t buffer_size);
