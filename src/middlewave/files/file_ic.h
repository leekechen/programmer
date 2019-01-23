#ifndef __FILE_IC_H__
#define __FILE_IC_H__

int32_t file_ic_init(void);
void* file_ic_open(const char* mode);
int32_t file_ic_write(const uint8_t* data, size_t size, void* fp);
int32_t file_ic_read(uint8_t* buf, size_t size, void* fp);
int32_t file_ic_close(void* fp);
uint32_t file_ic_get_size(void);

#endif /* __FILE_IC_H__ */