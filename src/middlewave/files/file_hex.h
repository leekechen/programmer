#ifndef __FILE_HEX_H__
#define __FILE_HEX_H__

#include <stdint.h>
#include <stdlib.h>

#include "tlv.h"



void*   file_hex_open(const char* mode);
int32_t file_hex_read(uint8_t* buf, size_t size, void* fp);
int32_t file_hex_write(const uint8_t* data, size_t size, void* fp);
int32_t file_hex_close(void* fp);
int32_t file_hex_upgrade(void);

int32_t                     file_hex_init               (void);
int32_t                     file_hex_load               (void);
int32_t                     file_hex_free               (void);
struct tlv_voltage*         file_hex_get_voltage        (int32_t type);
struct tlv_chip*            file_hex_get_chip           (int32_t index);
struct tlv_option*          file_hex_get_option         (int32_t index);
struct tlv_osc*             file_hex_get_osc            (int32_t type);
struct tlv_code*            file_hex_get_code           (int32_t index);
struct tlv_rolling_code*    file_hex_get_rolling_code   (int32_t index);
struct tlv_limit*           file_hex_get_limit          (int32_t index);
int32_t                     file_hex_read_code          (struct tlv_code* code, uint32_t offset, uint8_t* buf, size_t size);
int32_t                     file_hex_get_program        (void);
uint32_t                    file_hex_get_crc32          (void);

#endif /**/

