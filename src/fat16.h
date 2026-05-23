#pragma once

#include <stdint.h>

typedef struct __attribute__((packed))
{
    uint8_t jump_instruction[3];
    uint8_t oem_info[8];
    uint16_t sector_size;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_copies;
    uint16_t root_directory_entries;
    uint16_t total_sectors_16;
    uint8_t media_descriptor;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    uint8_t physical_drive_num;
    uint8_t reserved;
    uint8_t extended_boot_sig;
    uint32_t volume_serial_number;
    uint8_t volume_label[11];
    uint8_t filesystem_identifier[8];
} boot_block_t;

typedef struct __attribute__((packed))
{
    char name[8];
    char ext[3];
    uint8_t attrs;
    uint8_t reserved;
    uint8_t create_time_fine;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t last_access_date;
    uint16_t high_start_cluster;
    uint16_t update_time;
    uint16_t update_date;
    uint16_t start_cluster;
    uint32_t size;
} dir_entry_t;

typedef struct
{
    char const name[11];
    void const *content;
    uint32_t size;
    // computing fields based on index and size
    uint16_t cluster_start;
    uint16_t cluster_end;
} file_content_t;

#define DIV_CEIL(_v, _d) (((_v) / (_d)) + ((_v) % (_d) ? 1 : 0))

#define BPB_SECTOR_SIZE (512)
#define BPB_SECTORS_PER_CLUSTER (1)
#define BPB_RESERVED_SECTORS (1)
#define BPB_NUMBER_OF_FATS (2)
#define BPB_ROOT_DIR_ENTRIES (64)
#define BPB_TOTAL_SECTORS (130)
#define BPB_MEDIA_DESCRIPTOR_BYTE (0xF8)
#define FAT_ENTRY_SIZE (2)
#define FAT_ENTRIES_PER_SECTOR (BPB_SECTOR_SIZE / FAT_ENTRY_SIZE)
#define FAT_END_OF_CHAIN (0xFFFF)

#define TOTAL_CLUSTERS_ROUND_UP DIV_CEIL(BPB_TOTAL_SECTORS, BPB_SECTORS_PER_CLUSTER)
#define BPB_SECTORS_PER_FAT DIV_CEIL(TOTAL_CLUSTERS_ROUND_UP, FAT_ENTRIES_PER_SECTOR)
#define DIRENTRIES_PER_SECTOR (BPB_SECTOR_SIZE / sizeof(dir_entry_t))
#define ROOT_DIR_SECTOR_COUNT DIV_CEIL(BPB_ROOT_DIR_ENTRIES, DIRENTRIES_PER_SECTOR)
#define BPB_BYTES_PER_CLUSTER (BPB_SECTOR_SIZE * BPB_SECTORS_PER_CLUSTER)

#define WASM_FIRMWARE_BYTES_PER_SECTOR 256
#define WASM_SECTOR_COUNT (64 * 1024 / WASM_FIRMWARE_BYTES_PER_SECTOR)
#define WASM_BYTE_COUNT (WASM_SECTOR_COUNT * BPB_SECTOR_SIZE)
