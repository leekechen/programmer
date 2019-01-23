#ifndef __FILE_FIRMWARE_H__
#define __FILE_FIRMWARE_H__

#include <stdint.h>

int32_t  file_firmware_init(void);
void*    file_firmware_open(const char* mode);
uint32_t file_firmware_get_size(void);
int32_t  file_firmware_read(uint8_t* buf, size_t size, void* fp);
int32_t  file_firmware_write(const uint8_t* data, size_t size, void* fp);
int32_t  file_firmware_close(void* fp);
int32_t  file_firmware_upgrade(void);
int32_t  file_firmware_backup(void);

#ifdef BOOTLOADER
int32_t file_firmware_borken(void);      
int32_t file_firmware_load(struct tlv_file* tlv);
void*   file_firmware_open_code(struct tlv_file* tlv);
int32_t file_firmware_read_code(uint8_t* buf, size_t size, void* fp);
int32_t file_firmware_close_code(void* fp);
int32_t file_firmware_updated(void);
struct tlv_code* file_firmware_get_code(struct tlv_file* tlv);
int32_t file_firmware_recovery(void);

#endif


#endif /* __FILE_FIRMWARE_H__ */
