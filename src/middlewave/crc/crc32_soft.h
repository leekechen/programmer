#ifndef __CRC32_SOFT_H__
#define __CRC32_SOFT_H__

#include <stdint.h>
#include <stdlib.h>

/* __REV32(0x04C11DB7U) */  
#define CRC32_POLY               0xEDB88320
#define CRC32_INIT_VALUE         0xFFFFFFFF
#define CRC32_XOR_VALUE          0xFFFFFFFF

uint32_t crc32_sw_calc(uint32_t crc32, const uint8_t* data, size_t len);

#endif /* __CRC32_SOFT_H__ */
