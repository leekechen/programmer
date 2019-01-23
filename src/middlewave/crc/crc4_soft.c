#include "crc4_soft.h"

static const uint8_t crc4_table[] = {
    0x03, 0x08, 0x04, 0x02,
    0x01, 0x09, 0x0d, 0x0f,
    0x0e, 0x07, 0x0a, 0x05,
    0x0b, 0x0c, 0x06, 0x03
};

uint8_t crc4_sw_calc(uint16_t data)
{
    int32_t i;
    uint8_t crc4;

    for (i = 0, crc4 = 0; i < 16; i++) {
        if (data & 0x8000) {
            crc4 ^= crc4_table[i];
        }
        data <<= 1;
    }
        
    return crc4;
}
