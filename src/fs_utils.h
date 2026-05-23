#pragma once

#include <stdint.h>
#include "lfs.h"

int fs_init(void);
int fs_read_file(const char *path, uint8_t *buffer, uint32_t buffer_size, uint32_t *file_size_out);
int fs_mkdir(const char *path);

const struct lfs_config * fs_get_lfs_config(void);
