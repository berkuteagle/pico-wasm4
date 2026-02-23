#include <stdint.h>
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/time.h"
#include "pico/st7789.h"
#include "pico/st7789/config.h"
#include "st7789_commands.h"

static void st7789_reset()
{
    gpio_put(ST7789_PIN_RST, 0);
    sleep_ms(50);
    gpio_put(ST7789_PIN_RST, 1);
    sleep_ms(120);
}

static void st7789_cmd(const uint8_t cmd)
{
    st7789_dc_cmd();
    st7789_spi_write(&cmd, 1);
}

static void st7789_data(const uint8_t *data, const size_t len)
{
    st7789_dc_data();
    st7789_spi_write(data, len);
}

void st7789_init()
{
    gpio_init(ST7789_PIN_DC);
    gpio_init(ST7789_PIN_RST);
    gpio_init(ST7789_PIN_BLK);

    gpio_set_dir(ST7789_PIN_DC, GPIO_OUT);
    gpio_set_dir(ST7789_PIN_RST, GPIO_OUT);
    gpio_set_dir(ST7789_PIN_BLK, GPIO_OUT);

    gpio_put(ST7789_PIN_BLK, 1);

    spi_init(ST7789_SPI_PORT, ST7789_SPI_FREQ);
    gpio_set_function(ST7789_SPI_PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(ST7789_SPI_PIN_MOSI, GPIO_FUNC_SPI);

    spi_set_format(ST7789_SPI_PORT, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

    st7789_reset();

    st7789_cmd(SWRESET_CMD);
    sleep_ms(150);

    st7789_cmd(SLPOUT_CMD);
    sleep_ms(120);

    st7789_cmd(INVON_CMD);
    sleep_ms(10);

    const uint8_t madctl = 0x00;
    st7789_cmd(MADCTL_CMD);
    st7789_data(&madctl, 1);

    const uint8_t colmod = 0x55;
    st7789_cmd(COLMOD_CMD);
    st7789_data(&colmod, 1);

    st7789_cmd(DISPON_CMD);
    sleep_ms(20);
}

void st7789_fill(uint16_t color)
{
    const uint8_t data[2] = {color >> 8, color & 0xFF};

    st7789_cmd(CASET_CMD);
    const uint8_t col[] = {0, 0, 0, 239};
    st7789_data(col, 4);

    st7789_cmd(RASET_CMD);
    const uint8_t row[] = {0, 0, 0, 239};
    st7789_data(row, 4);

    st7789_cmd(RAMWR_CMD);
    st7789_dc_data();

    for (int i = 0; i < ST7789_SCREEN_W * ST7789_SCREEN_H; i++)
    {
        st7789_spi_write(data, 2);
    }
}

void st7789_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t data[4];

    st7789_cmd(CASET_CMD);
    data[0] = x0 >> 8;
    data[1] = x0 & 0xFF;
    data[2] = x1 >> 8;
    data[3] = x1 & 0xFF;
    st7789_data(data, 4);

    st7789_cmd(RASET_CMD);
    data[0] = y0 >> 8;
    data[1] = y0 & 0xFF;
    data[2] = y1 >> 8;
    data[3] = y1 & 0xFF;
    st7789_data(data, 4);

    st7789_cmd(RAMWR_CMD);
}

void st7789_fill_window(const uint8_t *line_buffer, size_t line_buffer_size, uint8_t window_height,
                        void (*line_callback)(uint8_t line_number, void *buffer), void *buffer)
{
    st7789_dc_data();

    for (int line_number = 0; line_number < window_height; line_number++)
    {
        line_callback(line_number, buffer);
        st7789_spi_write(line_buffer, line_buffer_size);
    }
}

void st7789_dc_cmd() { gpio_put(ST7789_PIN_DC, 0); }

void st7789_dc_data() { gpio_put(ST7789_PIN_DC, 1); }

void st7789_spi_write(const uint8_t *data, const size_t data_size)
{
    spi_write_blocking(ST7789_SPI_PORT, data, data_size);
}
