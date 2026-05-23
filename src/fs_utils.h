#pragma once

int fs_init(void);
int fs_read_file(const char *path, uint8_t *buffer, uint32_t buffer_size, uint32_t *file_size_out);
