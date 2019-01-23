#ifndef __FILE_CFG__
#define __FILE_CFG__

#include <stdint.h>
#include <stdlib.h>
#include "tlv.h"

void*   file_cfg_open(const char* mode);
int32_t file_cfg_read(uint8_t* buf, size_t size, void* fp);
int32_t file_cfg_write(const uint8_t* data, size_t size, void* fp);
int32_t file_cfg_close(void* fp);
int32_t file_cfg_upgrade(void);


int32_t file_cfg_load(struct tlv_file* tlv);
int32_t file_cfg_get_program(struct tlv_file* tlv);
struct tlv_date* file_cfg_get_date(struct tlv_file* tlv);
struct tlv_time* file_cfg_get_time(struct tlv_file* tlv);



#endif /* __FILE_CFG__ */
