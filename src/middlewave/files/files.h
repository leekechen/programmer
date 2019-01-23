#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#include <stdint.h>
#include <stdlib.h>

#include "tlv.h"

#define FILE_DEV              "file_dev.bin"
#define FILE_FIRMWARE         "file_firmware.bin"
#define FILE_HEX              "file_hex.bin"
#define FILE_IC               "file_ic.bin"
#define FILE_LOG              "file_log.bin"
#define FILE_CFG              "file_cfg.bin"


int32_t  file_init      (void);
void*    file_open      (int32_t id, const char* mode);
void*    file_code_open (int32_t id, struct tlv_code* code);
int32_t  file_read      (uint8_t* buf, size_t size, void* fp);
int32_t  file_write     (const uint8_t* data, size_t size, void* fp);
int32_t  file_seek      (void* fp, int32_t offset, int32_t whence);
int32_t  file_get_pos   (void* fp);
int32_t  file_close     (void* fp);
int32_t  file_get_size  (int32_t id);
int32_t  file_verify    (int32_t id);
int32_t  file_upgrade   (int32_t id);


#endif /*__FILE_SYSTEM_H__*/

