#include "bsp/board_api.h"
#include "tusb.h"
#include "fat16.h"
#include "compile_date.h"

// whether host does safe-eject
static bool ejected = false;

#define README_CONTENTS \
    "This is tinyusb's MassStorage Class demo.\r\n\r\n\
If you find any bugs or get any questions, feel free to file an\r\n\
issue at github.com/hathach/tinyusb"

static file_content_t info[] = {
    {
        .name = "CART   WASM",
        .content = NULL,
        .size = WASM_BYTE_COUNT,
        .cluster_start = 2,
        .cluster_end = WASM_SECTOR_COUNT + 2,
    },
};

#define NUM_FILES (1)
#define NUM_DIRENTRIES (2)
#define NUM_SECTORS_IN_DATA_REGION (BPB_TOTAL_SECTORS - BPB_RESERVED_SECTORS - (BPB_NUMBER_OF_FATS * BPB_SECTORS_PER_FAT) - ROOT_DIR_SECTOR_COUNT)
#define CLUSTER_COUNT (NUM_SECTORS_IN_DATA_REGION / BPB_SECTORS_PER_CLUSTER)

#define FS_START_FAT0_SECTOR BPB_RESERVED_SECTORS
#define FS_START_FAT1_SECTOR (FS_START_FAT0_SECTOR + BPB_SECTORS_PER_FAT)
#define FS_START_ROOTDIR_SECTOR (FS_START_FAT1_SECTOR + BPB_SECTORS_PER_FAT)
#define FS_START_CLUSTERS_SECTOR (FS_START_ROOTDIR_SECTOR + ROOT_DIR_SECTOR_COUNT)

static boot_block_t boot_block = {
    .jump_instruction = {0xeb, 0x3c, 0x90},
    .oem_info = "PICO2 W4",
    .sector_size = BPB_SECTOR_SIZE,
    .sectors_per_cluster = BPB_SECTORS_PER_CLUSTER,
    .reserved_sectors = BPB_RESERVED_SECTORS,
    .fat_copies = BPB_NUMBER_OF_FATS,
    .root_directory_entries = BPB_ROOT_DIR_ENTRIES,
    .total_sectors_16 = (BPB_TOTAL_SECTORS > 0xFFFF) ? 0 : BPB_TOTAL_SECTORS,
    .media_descriptor = BPB_MEDIA_DESCRIPTOR_BYTE,
    .sectors_per_fat = BPB_SECTORS_PER_FAT,
    .sectors_per_track = 1,
    .heads = 1,
    .total_sectors_32 = (BPB_TOTAL_SECTORS > 0xFFFF) ? BPB_TOTAL_SECTORS : 0,
    .physical_drive_num = 0x80, // to match MediaDescriptor of 0xF8
    .extended_boot_sig = 0x29,
    .volume_serial_number = 0x00420042,
    .volume_label = "PICO2 WASM4",
    .filesystem_identifier = "FAT16   ",
};

static void padded_memcpy(char *dst, char const *src, int len)
{
    for (int i = 0; i < len; ++i)
    {
        if (*src)
        {
            *dst = *src++;
        }
        else
        {
            *dst = ' ';
        }
        dst++;
    }
}

static void wasm_read_block(uint32_t block_no, uint8_t *data)
{
    memset(data, 0, BPB_SECTOR_SIZE);
    uint32_t sectionRelativeSector = block_no;

    if (block_no == 0)
    {
        // Request was for the Boot block
        memcpy(data, &boot_block, sizeof(boot_block));
        data[510] = 0x55; // Always at offsets 510/511, even when BPB_SECTOR_SIZE is larger
        data[511] = 0xaa; // Always at offsets 510/511, even when BPB_SECTOR_SIZE is larger
    }
    else if (block_no < FS_START_ROOTDIR_SECTOR)
    {
        // Request was for a FAT table sector
        sectionRelativeSector -= FS_START_FAT0_SECTOR;

        // second FAT is same as the first... use sectionRelativeSector to write data
        if (sectionRelativeSector >= BPB_SECTORS_PER_FAT)
        {
            sectionRelativeSector -= BPB_SECTORS_PER_FAT;
        }

        uint16_t *data16 = (uint16_t *)(void *)data;
        uint32_t sectorFirstCluster = sectionRelativeSector * FAT_ENTRIES_PER_SECTOR;
        uint32_t firstUnusedCluster = info[0].cluster_end + 1;

        // OPTIMIZATION:
        // Because all files are contiguous, the FAT CHAIN entries
        // are all set to (cluster+1) to point to the next cluster.
        // All clusters past the last used cluster of the last file
        // are set to zero.
        //
        // EXCEPTIONS:
        // 1. Clusters 0 and 1 require special handling
        // 2. Final cluster of each file must be set to END_OF_CHAIN
        //

        // Set default FAT values first.
        for (uint16_t i = 0; i < FAT_ENTRIES_PER_SECTOR; i++)
        {
            uint32_t cluster = i + sectorFirstCluster;
            if (cluster >= firstUnusedCluster)
            {
                data16[i] = 0;
            }
            else
            {
                data16[i] = cluster + 1;
            }
        }

        // Exception #1: clusters 0 and 1 need special handling
        if (sectionRelativeSector == 0)
        {
            data[0] = BPB_MEDIA_DESCRIPTOR_BYTE;
            data[1] = 0xff;
            data16[1] = FAT_END_OF_CHAIN; // cluster 1 is reserved
        }

        // Exception #2: the final cluster of each file must be set to END_OF_CHAIN
        for (uint32_t i = 0; i < NUM_FILES; i++)
        {
            uint32_t lastClusterOfFile = info[i].cluster_end;
            if (lastClusterOfFile >= sectorFirstCluster)
            {
                uint32_t idx = lastClusterOfFile - sectorFirstCluster;
                if (idx < FAT_ENTRIES_PER_SECTOR)
                {
                    // that last cluster of the file is in this sector
                    data16[idx] = FAT_END_OF_CHAIN;
                }
            }
        }
    }
    else if (block_no < FS_START_CLUSTERS_SECTOR)
    {
        // Request was for a (root) directory sector .. root because not supporting subdirectories (yet)
        sectionRelativeSector -= FS_START_ROOTDIR_SECTOR;

        dir_entry_t *d = (void *)data;                // pointer to next free dir_entry_t this sector
        int remainingEntries = DIRENTRIES_PER_SECTOR; // remaining count of DirEntries this sector

        uint32_t startingFileIndex;

        if (sectionRelativeSector == 0)
        {
            // volume label is first directory entry
            padded_memcpy(d->name, (char const *)boot_block.volume_label, 11);
            d->attrs = 0x28;
            d++;
            remainingEntries--;

            startingFileIndex = 0;
        }
        else
        {
            // -1 to account for volume label in first sector
            startingFileIndex = DIRENTRIES_PER_SECTOR * sectionRelativeSector - 1;
        }

        for (uint32_t fileIndex = startingFileIndex;
             remainingEntries > 0 && fileIndex < NUM_FILES; // while space remains in buffer and more files to add...
             fileIndex++, d++)
        {
            // WARNING -- code presumes all files take exactly one directory entry (no long file names!)
            uint32_t const startCluster = info[fileIndex].cluster_start;

            file_content_t const *inf = &info[fileIndex];
            padded_memcpy(d->name, inf->name, 11);

            d->create_time_fine = COMPILE_SECONDS_INT % 2 * 100;
            d->create_time = COMPILE_DOS_TIME;
            d->create_date = COMPILE_DOS_DATE;
            d->last_access_date = COMPILE_DOS_DATE;
            d->high_start_cluster = startCluster >> 16;
            d->update_time = COMPILE_DOS_TIME;
            d->update_date = COMPILE_DOS_DATE;
            d->start_cluster = startCluster & 0xFFFF;
            d->size = (inf->content ? inf->size : WASM_BYTE_COUNT);
        }
    }
}

void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
    (void)lun;
    (void)vendor_id;
    (void)product_id;
    (void)product_rev;
}

// Invoked when received SCSI_CMD_INQUIRY, v2 with full inquiry response
// Some inquiry_resp's fields are already filled with default values, application can update them
// Return length of inquiry response, typically sizeof(scsi_inquiry_resp_t) (36 bytes), can be longer if included vendor data.
uint32_t tud_msc_inquiry2_cb(uint8_t lun, scsi_inquiry_resp_t *inquiry_resp, uint32_t bufsize)
{
    (void)lun;
    (void)bufsize;
    const char vid[] = "Pico2 W4";
    const char pid[] = "Mass Storage";
    const char rev[] = "1.0";

    (void)strncpy((char *)inquiry_resp->vendor_id, vid, 8);
    (void)strncpy((char *)inquiry_resp->product_id, pid, 16);
    (void)strncpy((char *)inquiry_resp->product_rev, rev, 4);

    return sizeof(scsi_inquiry_resp_t); // 36 bytes
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
    (void)lun;

    // RAM disk is ready until ejected
    if (ejected)
    {
        // Additional Sense 3A-00 is NOT_FOUND
        return tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
    }

    return true;
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size)
{
    (void)lun;
    *block_count = WASM_SECTOR_COUNT + 2;
    *block_size = BPB_SECTOR_SIZE;
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
    (void)lun;
    (void)power_condition;

    if (load_eject)
    {
        if (start)
        {
            // load disk storage
        }
        else
        {
            // unload disk storage
            ejected = true;
        }
    }

    return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
    (void)lun;
    memset(buffer, 0, bufsize);

    // since we return block size each, offset should always be zero
    if (offset != 0)
    {
        return -1;
    }

    uint32_t count = 0;

    while (count < bufsize)
    {
        wasm_read_block(lba, buffer);

        lba++;
        buffer += BPB_SECTOR_SIZE;
        count += BPB_SECTOR_SIZE;
    }

    return count;
}

bool tud_msc_is_writable_cb(uint8_t lun)
{
    (void)lun;
    return false;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
    (void)lun;
    (void)lba;
    (void)offset;
    (void)buffer;
    (void)bufsize;

    return -1;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize)
{
    (void)lun;
    (void)scsi_cmd;
    (void)buffer;
    (void)bufsize;

    // currently no other commands are supported

    // Set Sense = Invalid Command Operation
    (void)tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

    return -1; // stall/failed command request;
}
