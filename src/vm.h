#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VM_ERROR_BUF_SIZE 256
#define VM_STACK_SIZE 4096

bool vm_init(void);
void vm_destroy(void);

char *vm_get_error(void);

bool vm_load_module(uint8_t *data, size_t data_len);
void vm_unload_module(void);
