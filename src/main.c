#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "pico/m3/env.h"
#include "wasm/cart1.h"
#include "wasm4.h"
#include "wasm4-runtime.h"
#include "wasm4-display.h"
#include "wasm4-gamepad.h"

static uint8_t *memory = NULL;

int main()
{
    stdio_init_all();

    w4_display_init();
    w4_gamepad_init();

    memory = w4_runtime_init();

    w4_runtime_load_wasm(cart1_wasm, cart1_wasm_len);

    while (true)
    {
        w4_gamepad_update(memory);
        // w4_netplay_update(memory);
        w4_runtime_update(memory);
        w4_display_update(memory);
        // sleep_ms(16);
    }
}
