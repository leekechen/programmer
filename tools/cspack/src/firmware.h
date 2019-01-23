#ifndef __FIRMWARE_H__
#define __FIRMWARE_H__

#include <stdint.h>

#define MAX_VERSION_NUM              4


struct firmware_info {
    const char* src;
    const char* dst;
    int32_t     chip_num;
    char*       chip_name[100];
    uint32_t    version[MAX_VERSION_NUM];
    uint32_t    addr;
} ;


#endif /* __FIRMWARE_H__ */
