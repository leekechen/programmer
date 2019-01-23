#include <stm32f4xx_hal.h>
#include "crc32.h"

#define MODULE_DEBUG                            1
#define MODULE_ERROR                            1

#include "dbg_print.h"


#define CRC32_POLY                              (0xEDB88320U) //REV(0x04C11DB7U)
#define WORD_32BITS_MASK                        (0x3U)

extern CRC_HandleTypeDef hcrc;

uint32_t crc32_hw_calc_residue(uint32_t crc32, const uint8_t* data, size_t size)
{
    size_t i;
    size_t j;

    for (i = 0; i < size; i++) {
        crc32 ^= data[i];
        for (j = 0; j < 8; j++) {
            if (crc32 & 0x1) {
                crc32 = (crc32 >> 1) ^ CRC32_POLY;
            } else {
                crc32 >>= 1;
            }
        }
    }

    return crc32;
}


uint32_t crc32_hw_calc(const uint8_t* data, size_t size)
{
    size_t    i;
    size_t    aligned;
    size_t    residue;
    uint32_t  crc32;

    aligned = size & (~WORD_32BITS_MASK);
    residue = size & WORD_32BITS_MASK;

    __HAL_LOCK(&hcrc);

    hcrc.State = HAL_CRC_STATE_BUSY;

    __HAL_CRC_DR_RESET(&hcrc);

    for (i = 0; i < aligned; i += 4) {
        hcrc.Instance->DR = __RBIT(*(const uint32_t*)(&data[i]));
    }

    crc32 = __RBIT(hcrc.Instance->DR);
    hcrc.State = HAL_CRC_STATE_READY;

    __HAL_UNLOCK(&hcrc);

    if (residue) {
        crc32 = crc32_hw_calc_residue(crc32, &data[i], size - i);
    }

    crc32 ^= 0xFFFFFFFF;

    return crc32;
}

int32_t crc32_hw_reset(void)
{
    __HAL_LOCK(&hcrc);

    hcrc.State = HAL_CRC_STATE_BUSY;

    __HAL_CRC_DR_RESET(&hcrc);

    return 0;
}


void crc32_hw_inject(const uint32_t* data, size_t size)
{
    size_t    i;

    for (i = 0; i < size; i++ ) {
        hcrc.Instance->DR =  __RBIT(data[i]);
    }
}

uint32_t crc32_hw_get(void)
{
    uint32_t crc32;

    crc32 = hcrc.Instance->DR;
    hcrc.State = HAL_CRC_STATE_READY;
    __HAL_UNLOCK(&hcrc);

    return __RBIT(crc32);
}
