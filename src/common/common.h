#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

#if (__ARMCC_VERSION > 6000000)
#include <arm_compat.h>
#endif

#ifndef __REV16
#define __REV16(v)                 ((((v) & 0xFF) << 8) | (((v) & 0xFF00) >> 8))
#endif



#ifndef __REV32
#define __REV32(v)                 __rev(v)
#endif


static inline uint32_t byte2word(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
    return (uint32_t)(((b0) << 24) | ((b1) << 16) | ((b2) << 8) | (b3));
}

#endif /* __COMMON_H__ */
