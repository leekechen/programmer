#include <stdint.h>
#include <stdio.h>
#include <stm32f4xx_hal.h>
#include <cmsis_os2.h>
#include "files.h"
#include "crc32.h"
#include "embed_flash.h"
#include "config.h"
#include "bootloader.h"
#include "cswrite_cfg.h"
#include "lcd.h"
#include "key.h"
#include "flash_map.h"
#include "fram_map.h"
#include "error.h"
#include "delay.h"
#include "buzzer.h"
#include "file_firmware.h"

#define MODULE_DEBUG       1 
#define MODULE_ERROR       1

#include "dbg_print.h"

#define FIRMWARE_PC_BASE_ADDR           (FIRMWARE_ADDR_BASE + 4)

static uint32_t msp;
static void (*pc)(void);

static const char* prompt_ver[] = {
    "version of hardware",
    "version of firmware",
    "version of bootloader",
    "version of PCSoftware",
} ;

//static const char* promt_firmware_flag[] = {
//    "New firmware invalid ",    
//    "New firmware upgraded",    
//    "Old firmware backuped",
//    "New firmware received"
//} ;


static void resetClk(void)
{
    IRQn_Type irq;
    
    /* Reset all perherial device of STM32F4 */
    RCC->AHB1RSTR = 0xFFFFFFFF;
    RCC->AHB2RSTR = 0xFFFFFFFF;
    RCC->AHB3RSTR = 0xFFFFFFFF;
    
    RCC->APB1RSTR = 0xFFFFFFFF;
    RCC->APB2RSTR = 0xFFFFFFFF;
    
    /* Release reset of perherial device of STM32F4 */
    RCC->AHB1RSTR = 0x0;
    RCC->AHB2RSTR = 0x0;
    RCC->AHB3RSTR = 0x0;
    RCC->APB1RSTR = 0x0;
    RCC->APB2RSTR = 0x0;

    /* Clear EXTI pending and Disable EXTI */
    EXTI->IMR     = 0x0;
    EXTI->PR      = 0xFFFFFFFF;
    
    /* Disable system Tick */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;
    for (irq = SVCall_IRQn; irq < FPU_IRQn; irq++) {
        NVIC_ClearPendingIRQ(irq);
        NVIC_DisableIRQ(irq);
    }
}

static void booting(void)
{
    osKernelLock();
    resetClk();
    __set_CONTROL(0x0);

    msp = *((uint32_t*)FIRMWARE_ADDR_BASE);
    pc = (void(*)(void))*((uint32_t*)FIRMWARE_PC_BASE_ADDR); 
    __set_MSP(msp);
    pc();    
}

static int32_t upgrade(struct tlv_file* tlv)
{   
    size_t      len;
    int32_t     size;
    uint32_t    addr;
    uint32_t    crc32;  
    uint8_t     buf[256];
    void*                 fp;
    struct tlv_code*      code;    

    dbg_print("[Bootloader] Upgrade to new firmware...\n");

    code = file_firmware_get_code(tlv);
    if (code == NULL) {
        err_print("file_firmware_get_code(%p) failed\n", (void*)tlv);
        return -1;
    }
    
    dbg_print("\t start=0x%x, end=0x%x\n", code->start, code->end);
    size = code->end - code->start;    
    if (size <= 0) {
        err_print("Invalid size=%d\n", size);
        return -1;
    }

    /* Start Programming to embed flash */
    addr = FIRMWARE_ADDR_BASE;//code->addr;
   
    fp = file_code_open(TLV_FILE_ID_FIRMWARE, code);
    if (fp == NULL) {
        err_print("file_firmware_open_code(%p) failed\n", tlv);
        return -1;
    }
    
    embed_flash_unlock();
    embed_flash_erase(addr, (size_t)size);
    
    for (len = sizeof(buf); len == sizeof(buf); addr += len) {        
        len = file_read(buf, sizeof(buf), fp);
        if (len > 0) {
            embed_flash_program(addr, buf, len);
        }
    }
    
    embed_flash_lock();

    dbg_print("\t checksum_type=%d\n", code->checksum_type);
    if (code->checksum_type == TLV_DESC_CHECKSUM_TYPE_CRC32) {
        crc32 = crc32_hw_calc((const uint8_t *)FIRMWARE_ADDR_BASE, (size_t)size); 
        if (crc32 != code->checksum) {
            err_print("Invalid checksum, crc32=0x%x,checksum=0x%x\n", crc32, code->checksum);
            return -1;
        }
        dbg_print("[Bootloader] Verify Updated firmware OK\n");
    } 
    
    return 0;
}

static void show_bytes(const char* msg, const uint8_t* data, size_t size)
{
    int32_t i;
    
    dbg_print("%s", msg);
    for (i = 0; i < size; i++) {
        dbg_print("%02x", data[i]);
    }
    dbg_print("\n");
}

static int32_t index2type(int32_t index)
{
    return index + 1;
}

static int32_t bsp_init(void)
{
    int32_t err;

    dbg_print("[Bootloader] BSP initialize ...\n");
    
    delay_init();
    
    err = st7789v_init(); 
    if (err) {
        err_print("lcd initialize failed, errno=%d\n", err);
        return err;
    }
    
//    err = gui_init(); 
//    if (err) {
//        err_print("lcd initialize failed, errno=%d\n", err);
//        return err;
//    }
    
    err = buzzer_init();
    if (err) {
        err_print("buzzer initialize failed, errno=%d\n", err);
        return err;
    }
    
    err = fram_map_init();
    if (err) {
        err_print("fram initialize failed, errno=%d\n", err);
        return err;
    } 
    
    err = key_init();
    if (err) {
        err_print("keyboard initialize failed, errno=%d\n", err);
        return err;
    }
    
    return ERR_OK;
}

static int32_t cfg_init(void)
{
    int32_t  ret;
    int32_t  i;
    int32_t  type;
    uint8_t  unique_id[4];
    uint8_t  stm32_id[12];
    uint8_t  version[TLV_DESC_VERSION_TYPE_PC][4];
    
    dbg_print("[Bootloader] Loading the configuration ...\n");
    
    file_init();
    
    ret = cswrite_cfg_load();
    if (ret) {        
        dbg_print(" failed!\n");
        dbg_print("\t Reset configuration\n");
        cswrite_cfg_reset();
    }
    dbg_print(" ok!\n");
    
    ret = cswrite_cfg_get_serial_id(TLV_DESC_SERIAL_ID_TYPE_32_BITS, unique_id);
    if (ret) {
        err_print("\t Error, get 32 bits Unique ID failed\n");
    }
    dbg_print("[Bootloader] CSWrite Information list:\n");
    dbg_print("\t CSWrite UnqiueID:\n");
    show_bytes("\t\t 32 Bits UnqiueID:", unique_id, sizeof(unique_id));
    
    ret = cswrite_cfg_get_serial_id(TLV_DESC_SERIAL_ID_TYPE_96_BITS, stm32_id);
    if (ret) {
        err_print("\t Error, get 96 bits Unique ID failed\n");
    }
    show_bytes("\t\t 96 Bits UnqiueID:", stm32_id, sizeof(stm32_id));
    
    return ERR_OK;
}


void bootloader_entry(void* arg)
{
    int32_t ret;
    struct tlv_file tlv;
    
    (void)arg;
    
    dbg_init();
    
    dbg_print("\n\n[CSwrite3.0] Bootloader built at %s %s\n", __DATE__, __TIME__);

    ret = bsp_init();
    if (ret) {        
        dbg_print(" failed!\n");
        return;
    }
    dbg_print(" ok!\n");
    
    cfg_init();
    
    dbg_print("[Bootloader] Booting ...\n");
    
    /* LCD show the log of ChipSea */


    dbg_print("[Bootloader] Searching the firmware ...\n");
    
    ret = file_firmware_load(&tlv);
    if (ret) {
        dbg_print("[Bootloader] Loading new firmware failed\n");
        ret = file_firmware_borken();
        if (ret) {
            goto RECOVER_FIRMWARE;
        }
        
        goto BOOTING_FIRMWARE;    
    }

    ret = upgrade(&tlv);
    if (ret) {        
        goto RECOVER_FIRMWARE;
    }

    file_firmware_updated();   


BOOTING_FIRMWARE:
    dbg_print("[Bootloader] Booting to firmware...\n");

    osDelay(5000);

    booting();
    return;
    
RECOVER_FIRMWARE:
    dbg_print("[Bootloader] Upgrade firmware failed, recovery firmware\n");
    file_firmware_recovery();
    dbg_print("[Bootloader] recovery finished, reboot to finished\n");
    NVIC_SystemReset();
    return;
}

void thread_main(void *arg)
{  
    bootloader_entry(NULL);
}

void osStart(void)
{
    osThreadId_t threadId;

    threadId = osThreadNew(thread_main, NULL, NULL);
    if (threadId == NULL) {
        return;
    }

    if(osKernelGetState() == osKernelInactive) {
        osKernelInitialize();
    }

    if (osKernelGetState() == osKernelReady) {
        osKernelStart();
    }
}
