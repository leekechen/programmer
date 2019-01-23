#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__

#include <stdint.h>
#include <stdlib.h>

#define FLASH_MAP_TYPE_FRIMWARE_A       0
#define FLASH_MAP_TYPE_FRIMWARE_B       1


/**
 * SPI Flash device initialization
 * @return Successed return ERR_OK, others failed
 */
int32_t flash_map_init(void);

/**
 * Erase the area which the type indicated
 * @param type
 *       FLASH_MAP_TYPE_FRIMWARE_A       0
 *       FLASH_MAP_TYPE_FRIMWARE_B       1
 *       FLASH_MAP_TYPE_HEX              2 
 * @return Successed return ERR_OK, others failed
 */
int32_t flash_map_erase(int32_t type);

/**
 * read data from the area which the type indicated
 * @param type
 *       FLASH_MAP_TYPE_FRIMWARE_A       0
 *       FLASH_MAP_TYPE_FRIMWARE_B       1
 *       FLASH_MAP_TYPE_HEX              2 
 * @param offset  the offset address in the area
 * @param buf     the output buffer
 * @param size    the size of output buffer
 * @return Successed return ERR_OK, others failed
 */
int32_t flash_map_read(int32_t type, uint32_t offset, uint8_t* buf, size_t size);

/**
 * write data to the area which the type indicated
 * @param type
 *       FLASH_MAP_TYPE_FRIMWARE_A       0
 *       FLASH_MAP_TYPE_FRIMWARE_B       1
 *       FLASH_MAP_TYPE_HEX              2 
 * @param offset  the offset address in the area
 * @param data    the data source buffer
 * @param size    the size of data source
 * @return Successed return ERR_OK, others failed
 */
int32_t flash_map_write(int32_t type, uint32_t offset, const uint8_t* data, size_t size);

#endif /* __FLASH_MAP_H__ */
