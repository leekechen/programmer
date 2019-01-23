#include <stdint.h>
#include "cmsis_os2.h"
#include "error.h"
#include "firmware.h"
#include "lcd.h"
#include "hid.h"
#include "key.h"
#include "buzzer.h"
#include "files.h"
#include "mb85rcxx.h"
#include "led.h"
#include "power.h"
#include "program.h"
#include "protcol.h"
#include "file_hex.h"
#include "flash_map.h"
#include "fram_map.h"
#include "ui.h"
#include "cswrite_cfg.h"
#include "delay.h"
#include "st7789v.h"


#define MODULE_DEBUG       1 
#define MODULE_ERROR       1

#include "dbg_print.h"

static int32_t bsp_init(void)
{
    int32_t err;

    dbg_print("[Firmware] BSP initialize ...");
    
    delay_init();
    err = st7789v_init(); 
    if (err) {
        err_print("lcd initialize failed, errno=%d\n", err);
        return err;
    }
    
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

void thread_main(void *arg)
{  
    int32_t err;
    struct  key_event key;
    
    dbg_init();
    
    dbg_print("\n\n[CSWrite3.0] Firmware built at %s %s\n", __DATE__, __TIME__);

    dbg_print("[Firmware] CSWrite3.0 Initializing\n");

    dbg_print("[Firmware] BSP device Initializing ...\n");
    err = bsp_init();
    if (err) {        
        dbg_print(" failed!\n");
        return;
    }

    err = file_init();
    if (err) {
        return; 
    }

    err_init();
    
    dbg_print("[Firmware] Loading the configuration ...\n");
    err = cswrite_cfg_load();
    if (err) {        
        dbg_print(" failed!\n");
        dbg_print("\t Reset configuration\n");
        cswrite_cfg_reset();
    }
    
    gui_init();

    err = protcol_init();
    if (err) {
        err_print("usb protocol, errno=%d\n", err);
        return;
    } 

    file_firmware_backup();
   
    while (1) {
        err = key_waiting(&key, osWaitForever);
        if (err) {
        }
        ui_key_process(&key);
    }
}


void osStart(void)
{    
    osThreadId_t thread_id_main;

    thread_id_main = osThreadNew(thread_main, NULL, NULL);
    if (thread_id_main == NULL) {
        return;
    }
    
    if(osKernelGetState() == osKernelInactive) {
        osKernelInitialize();
    }

    if (osKernelGetState() == osKernelReady) {
        osKernelStart();
    }
}
