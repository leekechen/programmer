#ifndef __EMBED_FLASH_H__
#define __EMBED_FLASH_H__

#include <stdlib.h>
#include <stdint.h>

#define FLASH_ADDR_BASE                 (0x08000000U)
#define FLASH_MAX_SIZE                  (0x00080000U)

#define BOOTLOADER_ADDR_BASE            (FLASH_ADDR_BASE)
#define BOOTLOADER_MAX_SIZE             (0x00020000U)

#define CSWRITER_UNIQUE_ID_ADDR_BASE    (0x08020000U)
#define CSWRITER_UNIQUE_ID_MAX_SIZE     (0x00000004U)

#define FIRMWARE_ADDR_BASE              (0x08020000U)
#define FIRMWARE_MAX_SIZE               (0x00060000U)


int32_t embed_flash_unlock(void);
int32_t embed_flash_lock(void);
int32_t embed_flash_erase(uint32_t addr, size_t size);

int32_t embed_flash_program(uint32_t addr, const uint8_t* data, size_t size);

#endif /**/
