#include "flash_map.h"
#include "w25qxx.h"
#include "error.h"

#define MODULE_DEBUG       0
#define MODULE_ERROR       1

#include "dbg_print.h"

#define FLASH_MAP_FRIMWARE_SIZE         (512 * 1024)


#define FLASH_MAP_FRIMWARE_A_ADDR       (0x0000)
#define FLASH_MAP_FIRMWARE_B_ADDR       (FLASH_MAP_FRIMWARE_A_ADDR + FLASH_MAP_FRIMWARE_SIZE)

#define FLASH_MAP_HEX_ADDR              (FLASH_MAP_FIRMWARE_B_ADDR + FLASH_MAP_FRIMWARE_SIZE)
#define FLASH_MAP_HEX_SIZE              (512 * 1024)




#define FLASH_DEV                       DEV_SPI1

static int32_t dev = 0;

static int32_t flash_map_get_addr(int32_t type)
{
    int32_t addr;
    
    switch (type) {
    case FLASH_MAP_TYPE_FRIMWARE_A:
        addr = FLASH_MAP_FRIMWARE_A_ADDR;
        break;
    case FLASH_MAP_TYPE_FRIMWARE_B:
        addr = FLASH_MAP_FIRMWARE_B_ADDR;
        break;
    case FLASH_MAP_TYPE_HEX:
        addr = FLASH_MAP_HEX_ADDR;
        break;
    default:
        addr =  -1;
    }
    
    return addr;    
}

static int32_t flash_map_get_size(int32_t type)
{
    int32_t size;
    switch (type) {
    case FLASH_MAP_TYPE_FRIMWARE_A:
        size = FLASH_MAP_FRIMWARE_SIZE;
        break;
    case FLASH_MAP_TYPE_FRIMWARE_B:
        size = FLASH_MAP_FRIMWARE_SIZE;
        break;
    case FLASH_MAP_TYPE_HEX:
        size = FLASH_MAP_HEX_SIZE;
        break;
    default:
        size =  -1;
    }
    return size;    
}

int32_t flash_map_init(void)
{
    int32_t err;
    
    w25qxx_init();
    dev = w25qxx_open(FLASH_DEV, FLASH_8M_SIZE);
    if (dev < 0) {
        err_print("w25qxxOpen(%s) failed!,ret=%d\n", FLASH_DEV, dev);
        return ERR_DRV_FLASH;
    }
    
    err = w25qxx_power_control(dev, ARM_POWER_FULL, 42000000);
    if (err) {
        err_print("w25qxx_power_control() failed!,ret=%d\n", err);
        return err;
    }
   
    return ERR_OK;
}

int32_t flash_map_erase(int32_t type)
{
    int32_t err;
    int32_t addr;
    int32_t size;

    addr = flash_map_get_addr(type);
    if (addr < 0) {
        err_print("flash_map_erase() Invalid type, type=%d\n", type);
        return ERR_DRV_FLASH;
    }

    size = flash_map_get_size(type);
    if (size < 0) {
        err_print("flash_map_get_size() Invalid type, type=%d\n", type);
        return ERR_DRV_FLASH;
    }
    
    err = w25qxx_erase(dev, addr, size);
    if (err) {
        err_print("w25qxx_erase() failed, dev=0x%x, addr=0x%x, size=0x%x\n", dev, addr, size);
        return ERR_DRV_FLASH;
    }

    return err;
}

int32_t flash_map_write(int32_t type, uint32_t offset, const uint8_t* data, size_t size)
{
    int32_t err;
    int32_t addr;
    int32_t max_size;    
    
    dbg_print("[Flash-Map] flash_map_write(), type=%d, offset=0x%x, buf=%p, size=%d\n", type, offset, data, size);
    dbg_dump("flash_map_write()", data, size);    
    
    if (data == NULL) {    
        err_print("flash_map_write() Invalid data==%p\n", data);
        return ERR_DRV_FLASH;
    }
    
    addr = flash_map_get_addr(type);
    if (addr < 0) {
        err_print("flash_map_erase() Invalid type, type=%d\n", type);
        return ERR_DRV_FLASH;
    }

    max_size = flash_map_get_size(type);
    if (max_size < 0) {
        err_print("flash_map_get_size() Invalid type, type=%d\n", type);
        return ERR_DRV_FLASH;
    }
    
    if ((offset + size) > max_size) {
        err_print("flash_map_get_size() Invalid size, (offset + size)=%d > max_size=%d\n", (offset + size), max_size);
        return ERR_DRV_FLASH;        
    }

    addr += offset;
    
    err= w25qxx_program(dev, addr, data, size);
    if (err != size) {
        err_print("w25qxx_program() failed,  dev=0x%x, addr=0x%x, data=%p, size=0x%x\n", dev, addr, data, size);
        return ERR_DRV_FLASH;
    }

    return err;
}


int32_t flash_map_read(int32_t type, uint32_t offset, uint8_t* buf, size_t size)
{
    int32_t err;
    int32_t addr;
    int32_t max_size;
    
    dbg_print("[Flash-Map] flash_map_read(), type=%d, offset=0x%x, buf=%p, size=%d\n", type, offset, buf, size);
    
    
    if (buf == NULL) {    
        err_print("flash_map_write() Invalid buf==%p\n", buf);
        return ERR_DRV_FLASH;
    }
    
    addr = flash_map_get_addr(type);
    if (addr < 0) {
        err_print("flash_map_get_addr() Invalid type, type=%d\n", type);
        return ERR_DRV_FLASH;
    }

    max_size = flash_map_get_size(type);
    if (max_size < 0) {
        err_print("flash_map_get_size() Invalid type, type=%d\n", type);
        return ERR_DRV_FLASH;
    }
    
    if ((offset + size) > max_size) {
        err_print("flash_map_get_size() Invalid size, (offset + size)=%d > max_size=%d\n", (offset + size), max_size);
        return ERR_DRV_FLASH;        
    }

    addr += offset;
    err= w25qxx_read(dev, addr, buf, size);
    if (err != size) {
        err_print("w25qxx_program() failed,  dev=0x%x, addr=0x%x, buf=%p, size=0x%x\n", dev, addr, buf, size);
        return ERR_DRV_FLASH;
    }

    dbg_dump("flash_map_read()", buf, size);

    return err;
}

