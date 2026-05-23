#include <stdint.h>
#include <string.h>

#include "lfs.h"
#include "fs_utils.h"
#include "w25qxx_utils.h"

static int littlefs_read(const struct lfs_config *c,
                         lfs_block_t block,
                         lfs_off_t offset,
                         void *buffer,
                         lfs_size_t size)
{
    if (w25qxx_device_read(block * 4096 + offset, buffer, size))
        return -1;

    return 0;
}

static int littlefs_prog(const struct lfs_config *c,
                         lfs_block_t block,
                         lfs_off_t offset,
                         const void *buffer,
                         lfs_size_t size)
{
    if (w25qxx_device_write(block * 4096 + offset, buffer, size))
        return -1;

    return 0;
}

static int littlefs_erase(const struct lfs_config *c, lfs_block_t block)
{
    if (w25qxx_device_erase(block))
        return -1;

    return 0;
}

static int littlefs_sync(const struct lfs_config *c)
{
    return 0;
}

static lfs_t fs_external;
const struct lfs_config fs_external_config = {
    .read = littlefs_read,
    .prog = littlefs_prog,
    .erase = littlefs_erase,
    .sync = littlefs_sync,
    .read_size = 256,
    .prog_size = 256,
    .block_size = 4096,
    .block_count = 4096,
    .cache_size = 256,
    .lookahead_size = 8,
    .block_cycles = 100,
};

int fs_init()
{
    if (w25qxx_device_init())
        return -1;

    if (lfs_mount(&fs_external, &fs_external_config))
    {
        if (lfs_format(&fs_external, &fs_external_config))
        {
            return -1;
        }
        if (lfs_mount(&fs_external, &fs_external_config))
        {
            return -1;
        }
    }

    return 0;
}

int fs_read_file(const char *path, uint8_t *buffer, uint32_t buffer_size, uint32_t *file_size_out)
{

    lfs_file_t file;
    lfs_soff_t file_size;

    if (lfs_file_open(&fs_external, &file, path, LFS_O_RDONLY))
        return -1;

    file_size = lfs_file_size(&fs_external, &file);
    if (file_size < 0)
        return -1;

    if (buffer_size < file_size)
        return -1;

    memset(buffer, 0, buffer_size);

    if (lfs_file_read(&fs_external, &file, buffer, (uint32_t)file_size) < 0)
        return -1;

    *file_size_out = (uint32_t)file_size;

    if (lfs_file_close(&fs_external, &file))
        return -1;

    return 0;
}
