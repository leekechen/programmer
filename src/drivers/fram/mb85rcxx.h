#ifndef __MB85RCXX_H__
#define __MB85RCXX_H__

#include <stdlib.h>
#include <stdint.h>

#define EEPROM_SIZE                  (1024*2)
#define EEPROM_SIZE_MASK             (EEPROM_SIZE-1)

int32_t fram_init(void);
int32_t fram_read(uint32_t addr, uint8_t* buf, size_t size);
int32_t fram_write(uint32_t addr, uint8_t* buf, size_t size);

#endif /* __MB85RCXX_H__ */
