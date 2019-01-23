#include <stdint.h>
#include <string.h>
#include <stm32f4xx_hal.h>
#include "embed_flash.h"

#define EMBED_FLASH_BASE                	(0x08000000U)

#define EMBED_FLASH_SECTOR_16K_BASE      	(0x08000000U)
#define EMBED_FLASH_SECTOR_64K_BASE      	(0x08010000U)
#define EMBED_FLASH_SECTOR_128K_BASE     	(0x08020000U)

#define EMBED_FLASH_SECTOR_SIZE_16K     	(1024*16)
#define EMBED_FLASH_SECTOR_SIZE_64K     	(1024*64)
#define EMBED_FLASH_SECTOR_SIZE_128K    	(1024*128)


int32_t embed_flash_unlock(void)
{
    return HAL_FLASH_Unlock();
}

int32_t embed_flash_lock(void)
{
    return HAL_FLASH_Lock();
}

int32_t embed_flash_erase(uint32_t addr, size_t size)
{
    FLASH_EraseInitTypeDef  eraseInit;
    uint32_t                sectorError;
    uint32_t                offset;
    uint32_t                sector;
    uint32_t                endSector;
    uint32_t                endAddr;

    memset(&eraseInit, 0, sizeof(FLASH_EraseInitTypeDef));    
    
    if (addr < EMBED_FLASH_SECTOR_64K_BASE) {
        offset = addr - EMBED_FLASH_SECTOR_16K_BASE;
        sector = offset / EMBED_FLASH_SECTOR_SIZE_16K;
    } else if(addr < EMBED_FLASH_SECTOR_128K_BASE) {
        sector = 4;
    } else {
        offset = addr - EMBED_FLASH_SECTOR_128K_BASE;
        sector = 5 + offset / EMBED_FLASH_SECTOR_SIZE_128K;
    }
    
    endAddr = addr + size;
    if (endAddr < EMBED_FLASH_SECTOR_64K_BASE) {
        endAddr   += EMBED_FLASH_SECTOR_SIZE_16K;
        offset    = endAddr - EMBED_FLASH_SECTOR_16K_BASE;
        endSector = offset / EMBED_FLASH_SECTOR_SIZE_16K;
    } else if(endAddr < EMBED_FLASH_SECTOR_128K_BASE) {
        endAddr   += EMBED_FLASH_SECTOR_SIZE_64K;
        offset    = endAddr - EMBED_FLASH_SECTOR_64K_BASE;
        endSector = 4 + offset / EMBED_FLASH_SECTOR_SIZE_64K;
    } else {
        endAddr   += EMBED_FLASH_SECTOR_SIZE_128K;
        offset    = endAddr - EMBED_FLASH_SECTOR_128K_BASE;
        endSector = 5 + offset / EMBED_FLASH_SECTOR_SIZE_128K;
    }
    
    eraseInit.Sector        = sector;
    eraseInit.NbSectors     = endSector - sector;
    eraseInit.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
    
    return HAL_FLASHEx_Erase(&eraseInit, &sectorError);
}

int32_t embed_flash_program(uint32_t addr, const uint8_t* data, size_t size)
{
    int32_t  ret;
    uint32_t align;
    uint32_t residue;
    const uint32_t* word;
    
    align = addr & 0x7;
    
    /* 1 byte alignment */
    while (align > 0) {
        ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, *data);
        if (ret) {
            return -1;
        }
        
        addr++;
        data++;
        align--;            
        size--;
    }   

    align   = size / 4;
    residue = size % 4;
    word    = (const uint32_t*)data;
    while (align > 0) {
        ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, *word);
        if (ret) {
            return -1;
        }
        addr += 4;
        word++;
        align--;
    }
    
    data = (const uint8_t*)word;
    while (residue > 0) {
        ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, *data);
        if (ret) {
            return -1;
        }
        addr++;
        data++;
        residue--;
    }

    return 0;
}

