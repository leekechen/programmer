#ifndef __W25QXX_H__
#define __W25QXX_H__

#include <stdint.h>
#include "Driver_SPI.h"

#define DEV_SPI1                                "/dev/spi1"
#define DEV_SPI2                                "/dev/spi2"
#define DEV_SPI3                                "/dev/spi3"
#define DEV_SPI4                                "/dev/spi4"
#define DEV_SPI5                                "/dev/spi5"
#define DEV_SPI6                                "/dev/spi6"

#define FLASH_DEV_MAX_NUM                       6
#define PAGE_SIZE                               (0x100U)
#define SECTOR_SIZE                             (0x1000U)

#define FLASH_8M_SIZE                           (1024*1024*8)
#define FLASH_2M_SIZE                           (1024*1024*2)

#define FLASH_QUAD_MODE_ENABLE                  (1)
#define FLASH_QUAD_MODE_DISABLE                 (0)


typedef uint8_t uniqueID_t[16];

int32_t w25qxx_init          (void);
int32_t w25qxx_open          (const char* dev, size_t size);
int32_t w25qxx_read_uid      (int32_t handle, uniqueID_t id);
int32_t w25qxx_read          (int32_t handle, uint32_t addr, uint8_t* buf, size_t size);
int32_t w25qxx_program       (int32_t handle, uint32_t addr, const uint8_t* data, size_t size);
int32_t w25qxx_erase_sector   (int32_t handle, uint32_t addr);
int32_t w25qxx_erase         (int32_t handle, uint32_t addr, size_t size);
int32_t w25qxx_chip_erase     (int32_t handle);
int32_t w25qxx_power_control  (int32_t handle, ARM_POWER_STATE state, uint32_t speed);
int32_t w25qxx_close         (int32_t handle);
int32_t w25qxx_read_id        (int32_t handle, uint8_t* buf, size_t size);
int32_t w25qxx_enable_qe      (int32_t handle, uint32_t flag);


#endif /* __W25QXX_H__ */
