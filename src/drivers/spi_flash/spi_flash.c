#include <string.h>
#include "Driver_Flash.h"
#include "spi_flash.h"
#include "w25qxx.h"
#include "error.h"

#define MODULE_DEBUG            0
#define MODULE_ERROR            1

#include "dbg_print.h"


#define FLASH_DEV                               DEV_SPI1

#define FLASH_SPI_SPEED                         (42000000)
#define FLASH_SECTOR_SIZE                       (SECTOR_SIZE)

#define ARM_FLASH_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,00) /* driver version */


#ifndef DRIVER_FLASH_NUM
#define DRIVER_FLASH_NUM        0       /* Default driver number */
#endif


/* Sector Information */
#ifdef FLASH_SECTORS
static ARM_FLASH_SECTOR FLASH_SECTOR_INFO[FLASH_SECTOR_COUNT] = {
    FLASH_SECTORS
};
#else
#define FLASH_SECTOR_INFO NULL
#endif

/* Flash Information */
static ARM_FLASH_INFO FlashInfo = {
    FLASH_SECTOR_INFO,
    FLASH_SECTOR_COUNT,
    FLASH_SECTOR_SIZE,
    FLASH_PAGE_SIZE,
    FLASH_PROGRAM_UNIT,
    FLASH_ERASED_VALUE
};

/* Flash Status */
static ARM_FLASH_STATUS FlashStatus;

static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_FLASH_API_VERSION,
    ARM_FLASH_DRV_VERSION
};

static const ARM_FLASH_CAPABILITIES DriverCapabilities = 
{
    .event_ready = 0,    /* event_ready */
    .data_width  = 0,    /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    .erase_chip  = 1,    /* erase_chip */
    .reserved    = 0
};

static int32_t spiFlashHandle;

static ARM_DRIVER_VERSION GetVersion (void) 
{
    return DriverVersion;
}

static ARM_FLASH_CAPABILITIES GetCapabilities (void) 
{
    return DriverCapabilities;
}

static int32_t Initialize (ARM_Flash_SignalEvent_t cb_event) 
{
    (void)cb_event;
    
    w25qxx_init();
    spiFlashHandle = w25qxx_open(FLASH_DEV, FLASH_8M_SIZE);

    if (spiFlashHandle < 0) {
        err_print("w25qxxOpen(%s) failed!,ret=%d\n", FLASH_DEV, spiFlashHandle);
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    return ARM_DRIVER_OK;
}

static int32_t Uninitialize (void) 
{
    /* ... */
    return ARM_DRIVER_OK;
} 

static int32_t PowerControl (ARM_POWER_STATE state) 
{
    FlashStatus.busy = 0;
    FlashStatus.error = 0;
    return w25qxx_power_control(spiFlashHandle, state, FLASH_SPI_SPEED);
}

static int32_t ReadData (uint32_t addr, void *data, uint32_t cnt)
{
    int32_t len;
    
    dbg_print("%s(),%d,addr=0x%x,size=%d\n", __func__, __LINE__, addr, cnt);
    len = w25qxx_read(spiFlashHandle, addr, data, cnt);
    dbg_dump("ReadData() Dump:", (uint8_t*)data, len);
    
    return len;
}

static int32_t ProgramData (uint32_t addr, const void *data, uint32_t cnt) 
{
    int32_t len;
    
    dbg_print("%s(),%d,addr=0x%x,size=%d\n", __func__, __LINE__, addr, cnt);
    len = w25qxx_program(spiFlashHandle, addr, data, cnt);
    dbg_dump("ProgramData() Dump:", (uint8_t*)data, len);
    
    return len;
}

static int32_t EraseSector (uint32_t addr) 
{
    return w25qxx_erase_sector(spiFlashHandle, addr);
}

static int32_t EraseChip (void) 
{    
    return w25qxx_chip_erase(spiFlashHandle);
}

static ARM_FLASH_STATUS GetStatus (void) 
{
    return FlashStatus;
}

/**
\fn          int32_t ARM_Flash_GetInfo (ARM_FLASH_INFO *info)
\brief       Get Flash information.
\param[out]  info  Pointer to Flash information \ref ARM_FLASH_INFO retrieved.
\return      \ref execution_status
*/
static ARM_FLASH_INFO* GetInfo(void)
{
    return &FlashInfo;
}

/* Flash Driver Control Block */
ARM_DRIVER_FLASH ARM_Driver_Flash_(DRIVER_FLASH_NUM) = {
    GetVersion,
    GetCapabilities,
    Initialize,
    Uninitialize,
    PowerControl,
    ReadData,
    ProgramData,
    EraseSector,
    EraseChip,
    GetStatus,
    GetInfo
};
