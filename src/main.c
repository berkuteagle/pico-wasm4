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
#include "wasm/cart1.h"
#include "wasm4.h"
#include "wasm4-runtime.h"
#include "wasm4-display.h"
#include "pico/w25qxx.h"
#include "pico/w25qxx/interface.h"

static uint8_t cart[W4_CART_SIZE] = {};
static uint8_t framebuffer[W4_FB_SIZE] = {};
static uint8_t palette[W4_PALETTE_SIZE] = {};
static uint8_t gamepad = 0;

static repeating_timer_t gamepad_timer;

static w25qxx_handle_t gs_handle;

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
    w4_runtime_load_wasm(cart, cart1_wasm_len);

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

    /* link interface function */
    DRIVER_W25QXX_LINK_INIT(&gs_handle, w25qxx_handle_t);
    DRIVER_W25QXX_LINK_SPI_QSPI_INIT(&gs_handle, w25qxx_interface_spi_qspi_init);
    DRIVER_W25QXX_LINK_SPI_QSPI_DEINIT(&gs_handle, w25qxx_interface_spi_qspi_deinit);
    DRIVER_W25QXX_LINK_SPI_QSPI_WRITE_READ(&gs_handle, w25qxx_interface_spi_qspi_write_read);
    DRIVER_W25QXX_LINK_DELAY_MS(&gs_handle, w25qxx_interface_delay_ms);
    DRIVER_W25QXX_LINK_DELAY_US(&gs_handle, w25qxx_interface_delay_us);
    DRIVER_W25QXX_LINK_DEBUG_PRINT(&gs_handle, w25qxx_interface_debug_print);

    w25qxx_set_type(&gs_handle, W25Q128);
    w25qxx_set_interface(&gs_handle, W25QXX_INTERFACE_SPI);
    w25qxx_set_dual_quad_spi(&gs_handle, false);

    w25qxx_init(&gs_handle);

    uint8_t manufacturer;
    uint8_t device_id;
    uint8_t res;

    res = w25qxx_get_manufacturer_device_id(&gs_handle, &manufacturer, &device_id);

    printf("ChipID read res: %d\n", res);
    printf("w25qxx: manufacturer is 0x%02X device id is 0x%02X.\n", manufacturer, device_id);

    res = w25qxx_read(&gs_handle, 0, cart, W4_CART_SIZE);

    printf("Cart read res: %d\n", res);

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

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+
void tud_mount_cb(void)
{
    //   blink_interval_ms = BLINK_MOUNTED;
}

void tud_umount_cb(void)
{
    //   blink_interval_ms = BLINK_NOT_MOUNTED;
}

void tud_suspend_cb(bool remote_wakeup_en)
{
    (void)remote_wakeup_en;
    //   blink_interval_ms = BLINK_SUSPENDED;
}

void tud_resume_cb(void)
{
    //   blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}
