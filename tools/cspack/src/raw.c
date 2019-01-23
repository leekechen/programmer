#include <stdio.h>
#include <stdint.h>

#include "crc32_soft.h"
#include "copy.h"

int cspack_raw(const char* dst, const char* src, size_t aligment) 
{
    int err;
    
    err = copy_by_name(dst, src);
    if (err) {
        return err;
    }

    return 0;
}


int csapck_verify(const char* dst, const char* src) 
{
    return 0;
}

