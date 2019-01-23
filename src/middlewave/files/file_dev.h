#ifndef __FILE_DEV_H__
#define __FILE_DEV_H__

#include <stdint.h>
#include <stdlib.h>

int32_t file_dev_init(void);

void*   file_dev_open(const char* mode);
int32_t file_dev_read(uint8_t* buf, size_t size, void* fp);
int32_t file_dev_write(const uint8_t* data, size_t size, void* fp);
int32_t file_dev_close(void* fp);
int32_t file_dev_upgrade(void);


#endif /* __FILE_DEV_H__ */

