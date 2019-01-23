#ifndef __CRC32_SOFT_H__
#define __CRC32_SOFT_H__

#include <stdint.h>
#include <stdlib.h>

uint32_t crc32_sw_calc(uint32_t crc32, const uint8_t* data, size_t len);

#endif /* __CRC32_SOFT_H__ */
