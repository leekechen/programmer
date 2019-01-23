#ifndef __FILE_LOG_H__
#define __FILE_LOG_H__

#include <stdint.h>

int32_t file_log_get_num_ok(uint32_t* num_ok);
int32_t file_log_get_num_failed(uint32_t* num_failed);

#endif /* __FILE_LOG_H__ */
