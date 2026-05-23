#include "bsp/board_api.h"
#include "tusb.h"

#include "mimic_fat.h"
#include "fs_utils.h"

// whether host does safe-eject
static bool ejected = false;
static bool is_initialized = false;

static const char msc_vid[8] = "Pico2 W4";
static const char msc_pid[16] = "Mass Storage";
static const char msc_rev[4] = "1.0";

void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
    (void)lun;

    memcpy(vendor_id, msc_vid, 8);
    memcpy(product_id, msc_pid, 16);
    memcpy(product_rev, msc_rev, 4);
}

// Invoked when received SCSI_CMD_INQUIRY, v2 with full inquiry response
// Some inquiry_resp's fields are already filled with default values, application can update them
// Return length of inquiry response, typically sizeof(scsi_inquiry_resp_t) (36 bytes), can be longer if included vendor data.
uint32_t tud_msc_inquiry2_cb(uint8_t lun, scsi_inquiry_resp_t *inquiry_resp, uint32_t bufsize)
{
    (void)lun;
    (void)bufsize;

    (void)strncpy((char *)inquiry_resp->vendor_id, msc_vid, 8);
    (void)strncpy((char *)inquiry_resp->product_id, msc_pid, 16);
    (void)strncpy((char *)inquiry_resp->product_rev, msc_rev, 4);

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

    *block_count = 4096;
    *block_size = DISK_SECTOR_SIZE;
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
    (void)offset;

    if (!is_initialized)
    {
        mimic_fat_init(fs_get_lfs_config());
        mimic_fat_update_usb_device_is_enabled(true);
        mimic_fat_create_cache();
        is_initialized = true;
    }
    mimic_fat_read(lun, lba, buffer, bufsize);

    return (int32_t)bufsize;
}

bool tud_msc_is_writable_cb(uint8_t lun)
{
    (void)lun;

    return true;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
    (void)offset;

    mimic_fat_write(lun, lba, buffer, bufsize);

    return bufsize;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize)
{
    void const *response = NULL;
    int32_t resplen = 0;

    // most scsi handled is input
    bool in_xfer = true;

    switch (scsi_cmd[0])
    {
    default:
        // Set Sense = Invalid Command Operation
        tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);
        // negative means error -> tinyusb could stall and/or response with failed status
        resplen = -1;
        break;
    }

    // return resplen must not larger than bufsize
    if (resplen > bufsize)
        resplen = bufsize;

    if (response && (resplen > 0))
    {
        if (in_xfer)
        {
            memcpy(buffer, response, (size_t)resplen);
        }
        else
        {
            ; // SCSI output
        }
    }
    return (int32_t)resplen;
}

void tud_mount_cb(void)
{
    printf("\e[45mmount\e[0m\n");
    /*
     * NOTE:
     * This callback must be returned immediately. Time-consuming processing
     * here will cause TinyUSB to PANIC `ep 0 in was already available`.
     */
    is_initialized = false;
}

void tud_umount_cb(void)
{
    //   blink_interval_ms = BLINK_NOT_MOUNTED;
    printf("Unmounted!");
}

void tud_suspend_cb(bool remote_wakeup_en)
{
    (void)remote_wakeup_en;

    printf("\e[45msuspend\e[0m\n");
    mimic_fat_cleanup_cache();
    mimic_fat_update_usb_device_is_enabled(false);
}

void tud_resume_cb(void)
{
    //   blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
    printf("Resume!");
}
