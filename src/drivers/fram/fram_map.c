#include "error.h"
#include "mb85rcxx.h"
#include "fram_map.h"

#define MODULE_DEBUG       1 
#define MODULE_ERROR       1

#include "dbg_print.h"

#define CSWRITE_CFG_BASE_ADDR     0x0000
#define CSWRITE_LOG_BASE_ADDR     0x0100

int32_t fram_map_init(void)
{
    return fram_init();
}

int32_t fram_map_log_read(uint32_t addr, uint8_t* buf, size_t size)
{
    int32_t err;

    err = fram_read(CSWRITE_LOG_BASE_ADDR + addr, buf, size);
    if (err) {
        err_print("fram_read() failed, err=%d\n", err);
        return 0;
    }

    return size;
}

int32_t fram_map_log_write(int32_t addr, const uint8_t* buf, size_t size)
{
    int32_t err;
    
    err =  fram_write(CSWRITE_LOG_BASE_ADDR + addr, (uint8_t*)buf, size);
    if (err) {
        err_print("fram_write() failed, err=%d\n", err);
        return 0;
    }
    
    return size;
}

int32_t fram_map_cfg_read(int32_t addr, uint8_t* buf, size_t size)
{
    int32_t err;

    dbg_print("[Fram] fram_map_cfg_read(0x%x,%d)\n", addr, size);
    err = fram_read(CSWRITE_CFG_BASE_ADDR + addr, buf, size);
    if (err) {
        err_print("fram_read() failed, err=%d\n", err);
        return 0;
    }

    return size;
}

int32_t fram_map_cfg_write(int32_t addr, const uint8_t* buf, size_t size)
{
    int32_t err;
  
    dbg_print("[Fram] fram_map_cfg_write(0x%x,%d)\n", addr, size);  
    err =  fram_write(CSWRITE_CFG_BASE_ADDR + addr, (uint8_t*)buf, size);
    if (err) {
        err_print("fram_write() failed, err=%d\n", err);
        return 0;
    }
    
    return size;
}
