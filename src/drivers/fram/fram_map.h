#ifndef __FRAM_MAP_H__
#define __FRAM_MAP_H__

#include <stdint.h>
#include <stdlib.h>



int32_t fram_map_init(void);


int32_t fram_map_log_read(uint32_t addr, uint8_t* buf, size_t size);
int32_t fram_map_log_write(int32_t addr, const uint8_t* buf, size_t size);

int32_t fram_map_cfg_read(int32_t addr, uint8_t* buf, size_t size);
int32_t fram_map_cfg_write(int32_t addr, const uint8_t* buf, size_t size);

#endif /* __FRAM_MAP_H__ */
