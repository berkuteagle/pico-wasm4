#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "bsp/board_api.h"
#include "tusb.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/buttons.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "pico/m3/env.h"
#include "wasm4.h"
#include "wasm4-runtime.h"
#include "wasm4-display.h"
#include "fs_utils.h"

static uint8_t cart[W4_CART_SIZE] = {};
static uint32_t cart_size = 0;

static uint8_t framebuffer[W4_FB_SIZE] = {};
static uint8_t palette[W4_PALETTE_SIZE] = {};
static uint8_t gamepad = 0;

static repeating_timer_t gamepad_timer;

volatile bool frame_ready = false;
volatile bool gamepad_ready = false;

bool gamepad_timer_callback(repeating_timer_t *t)
{
    gamepad_ready = true;
    return true;
}

static void usb_device_init(void)
{
    tusb_rhport_init_t dev_init = {
        .role = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO};
    tusb_init(BOARD_TUD_RHPORT, &dev_init);
    board_init_after_tusb();
}

void core1_entry()
{
    w4_runtime_init();
    w4_runtime_load_wasm(cart, cart_size);

    absolute_time_t next_frame = get_absolute_time();

    while (true)
    {
        while (frame_ready)
            tight_loop_contents();

        next_frame = delayed_by_us(next_frame, 16666);

        w4_runtime_update(framebuffer, palette, &gamepad);
        frame_ready = true;

        sleep_until(next_frame);
    }
}

int main()
{
    board_init();
    usb_device_init();
    stdio_init_all();
    w4_display_init();
    buttons_init();

    fs_init();
    fs_read_file("/cart.wasm", cart, W4_CART_SIZE, &cart_size);

    multicore_launch_core1(core1_entry);

    gamepad = buttons_read();

    add_repeating_timer_ms(-50, gamepad_timer_callback, NULL, &gamepad_timer);

    while (true)
    {
        if (gamepad_ready)
        {
            gamepad_ready = false;
            gamepad = buttons_read();
        }

        if (frame_ready)
        {
            frame_ready = false;
            w4_display_update((const uint8_t *)framebuffer, (const uint8_t *)palette);
        }

        tud_task();
    }
}
