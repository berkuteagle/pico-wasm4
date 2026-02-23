#include "pico/tm1638.h"
#include "pico/tm1638/config.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "tm1638_private.h"

static void tm_stb_low() { gpio_put(TM1638_PIN_STB, false); }

static void tm_stb_high() { gpio_put(TM1638_PIN_STB, true); }

static void tm_send_data(const uint8_t data)
{
    for (int i = 0; i < 8; i++)
    {
        gpio_put(TM1638_PIN_DIO, !!(data & (1 << i)));
        gpio_put(TM1638_PIN_CLK, true);
        busy_wait_us(TM1638_IO_DELAY);
        gpio_put(TM1638_PIN_CLK, false);
        busy_wait_us(TM1638_IO_DELAY);
    }
}

static void tm_send_command(const uint8_t cmd)
{
    tm_stb_low();
    tm_send_data(cmd);
    tm_stb_high();
}

static uint8_t tm_read_data()
{
    uint8_t data = 0;

    for (int i = 0; i < 8; i++)
    {
        gpio_put(TM1638_PIN_CLK, true);
        busy_wait_us(TM1638_IO_DELAY);
        data |= gpio_get(TM1638_PIN_DIO) << i;
        gpio_put(TM1638_PIN_CLK, false);
        busy_wait_us(TM1638_IO_DELAY);
    }

    return data;
}

void tm1638_init()
{
    gpio_init(TM1638_PIN_STB);
    gpio_init(TM1638_PIN_CLK);
    gpio_init(TM1638_PIN_DIO);

    gpio_set_dir(TM1638_PIN_STB, GPIO_OUT);
    gpio_set_dir(TM1638_PIN_CLK, GPIO_OUT);
    gpio_set_dir(TM1638_PIN_DIO, GPIO_OUT);

    tm_send_command(TM1638_CMD_ACTIVATE);

    tm1638_set_brightness(TM1638_BRIGHTNESS_DEFAULT);
    tm1638_clear();
}

void tm1638_set_brightness(const uint8_t brightness)
{
    tm_send_command(TM1638_BRIGHTNESS_ADR +
                    (TM1638_BRIGHTNESS_MASK & MIN(brightness, 7)));
}

void tm1638_clear()
{
    tm_send_command(TM1638_CMD_WRITE_INC);
    tm_stb_low();
    tm_send_data(TM1638_SEGMENTS_ADR);

    for (int i = 0; i < 16; i++)
        tm_send_data(0x00);

    tm_stb_high();
}

void tm1638_set_segment(const uint8_t pos, const uint8_t segments)
{
    tm_send_command(TM1638_CMD_WRITE_LOC);
    tm_stb_low();
    tm_send_data(TM1638_SEGMENTS_ADR + (MIN(pos, 7) << 1));
    tm_send_data(segments);
    tm_stb_high();
}

void tm1638_set_led(const uint8_t pos, const bool on)
{
    tm_send_command(TM1638_CMD_WRITE_LOC);
    tm_stb_low();
    tm_send_data(TM1638_LEDS_ADR + (MIN(pos, 7) << 1));
    tm_send_data(on ? 1 : 0);
    tm_stb_high();
}

uint8_t tm1638_read_buttons(void)
{
    uint8_t data[4];

    tm_stb_low();
    tm_send_data(TM1638_CMD_READ_KEYS);
    gpio_set_dir(TM1638_PIN_DIO, GPIO_IN);

    for (int i = 0; i < 4; i++)
        data[i] = tm_read_data();

    gpio_set_dir(TM1638_PIN_DIO, GPIO_OUT);
    tm_stb_high();

    uint8_t keys = 0;

    if (data[0] & 0x01)
        keys |= (1 << 0);
    if (data[1] & 0x01)
        keys |= (1 << 1);
    if (data[2] & 0x01)
        keys |= (1 << 2);
    if (data[3] & 0x01)
        keys |= (1 << 3);
    if (data[0] & 0x10)
        keys |= (1 << 4);
    if (data[1] & 0x10)
        keys |= (1 << 5);
    if (data[2] & 0x10)
        keys |= (1 << 6);
    if (data[3] & 0x10)
        keys |= (1 << 7);

    return keys;
}
