#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include <hardware/spi.h>
#include <hardware/gpio.h>
#include <pico/time.h>
#include "pico/w25qxx.h"

#include "w25qxx_utils.h"

static w25qxx_handle_t gs_handle;

static uint8_t w25qxx_interface_spi_qspi_init(void)
{
    spi_init(spi0, 1 * 1000 * 1000);
    spi_set_format(spi0, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

    gpio_set_function(2, GPIO_FUNC_SPI);
    gpio_set_function(3, GPIO_FUNC_SPI);
    gpio_set_function(4, GPIO_FUNC_SPI);

    gpio_init(5);
    gpio_set_dir(5, GPIO_OUT);
    gpio_put(5, true);

    return 0;
}

static uint8_t w25qxx_interface_spi_qspi_deinit(void)
{
    spi_deinit(spi0);

    return 0;
}

static uint8_t w25qxx_interface_spi_qspi_write_read(uint8_t instruction, uint8_t instruction_line,
                                                    uint32_t address, uint8_t address_line, uint8_t address_len,
                                                    uint32_t alternate, uint8_t alternate_line, uint8_t alternate_len,
                                                    uint8_t dummy, const uint8_t *in_buf, uint32_t in_len,
                                                    uint8_t *out_buf, uint32_t out_len, uint8_t data_line)
{
    if (instruction_line || address_line || alternate_line || dummy || (data_line != 1))
    {
        return 1;
    }

    gpio_put(5, false);

    if (in_buf != NULL && in_len > 0)
    {
        spi_write_blocking(spi0, in_buf, in_len);
    }

    if (out_buf != NULL && out_len > 0)
    {
        spi_read_blocking(spi0, 0xFF, out_buf, out_len);
    }

    gpio_put(5, true);

    return 0;
}

static void w25qxx_interface_delay_ms(uint32_t ms)
{
    sleep_ms(ms);
}

static void w25qxx_interface_delay_us(uint32_t us)
{
    sleep_us(us);
}

static void w25qxx_interface_debug_print(const char *const fmt, ...)
{
    char str[256];
    va_list args;

    memset((char *)str, 0, sizeof(char) * 256);
    va_start(args, fmt);
    vsnprintf((char *)str, 255, (char const *)fmt, args);
    va_end(args);

    (void)printf((uint8_t *)str);
}

uint8_t w25qxx_device_init()
{
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

    return w25qxx_init(&gs_handle);
}

uint8_t w25qxx_device_read(uint32_t addr, uint8_t *data, uint32_t size)
{
    return w25qxx_read(&gs_handle, addr, data, size);
}

uint8_t w25qxx_device_write(uint32_t addr, const uint8_t *data, uint32_t size)
{
    return w25qxx_write(&gs_handle, addr, data, size);
}

uint8_t w25qxx_device_erase(uint32_t block)
{
    return w25qxx_sector_erase_4k(&gs_handle, block);
}
