#pragma once

#include <stdbool.h>
#include <stdint.h>

void tm1638_init(void);
void tm1638_clear(void);
void tm1638_set_brightness(uint8_t brightness);
void tm1638_set_segment(uint8_t pos, uint8_t segments);
void tm1638_set_led(uint8_t pos, bool on);
uint8_t tm1638_read_buttons(void);
