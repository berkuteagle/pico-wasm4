#include "wasm4-gamepad.h"
#include "pico/tm1638.h"
#include "wasm4.h"

void w4_gamepad_init(void)
{
    tm1638_init();
}

void w4_gamepad_update(uint8_t *memory)
{
    uint8_t *gamepad = memory + W4_GAMEPADS_OFFSET;

    gamepad[0] = tm1638_read_buttons();
}
