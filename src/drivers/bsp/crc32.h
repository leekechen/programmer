#ifndef __CRC32_H__
#define __CRC32_H__

#include <stdint.h>
#include <stdlib.h>


uint32_t crc32_hw_calc(const uint8_t* data, size_t size);
uint32_t crc32_hw_calc_residue(uint32_t crc32, const uint8_t* data, size_t size);
int32_t  crc32_hw_reset(void);
void     crc32_hw_inject(const uint32_t* data, size_t size);
uint32_t crc32_hw_get(void);

#endif /* __CRC32_H__ */
