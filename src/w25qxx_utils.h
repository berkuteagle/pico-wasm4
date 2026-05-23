#pragma once

#include <stdint.h>

uint8_t w25qxx_device_init(void);
uint8_t w25qxx_device_read(uint32_t addr, uint8_t *data, uint32_t size);
uint8_t w25qxx_device_write(uint32_t addr, const uint8_t *data, uint32_t size);
uint8_t w25qxx_device_erase(uint32_t block);
