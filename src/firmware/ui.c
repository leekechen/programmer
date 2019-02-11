#include <stm32f4xx.h>
#include <cmsis_os2.h>

#include <rl_fs.h>
#include "ui.h"
#include "key.h"
#include "error.h"
#include "program.h"
#include "file_hex.h"
#include "file_cfg.h"
#include "power.h"
#include "cswrite_cfg.h"
#include "cswrite_log.h"
#include "gui_main.h"
#include "power.h"

#define MODULE_DEBUG       1 
#define MODULE_ERROR       1

#include "dbg_print.h"

static int32_t power_setup(void)
{
    int32_t            err;
    struct tlv_voltage* vdd;
    struct tlv_voltage* vpp;
    struct tlv_file*    hex;
        
    dbg_print("[Program] Setup Program power\n");
    vdd = file_hex_get_voltage(TLV_DESC_VOLTAGE_TYPE_VDD);
    if (vdd == NULL) {
        err_print("file_hex_get_voltage() failed\n");
        return ERR_VDD;
    }
    dbg_print("\t Vdd : %d mv, %d mv\n", vdd->value, vdd->offset);
    
    vpp = file_hex_get_voltage(TLV_DESC_VOLTAGE_TYPE_VPP);
    if (vpp == NULL) {
        err_print("file_hex_get_voltage() failed\n");
        return ERR_VDD;
    }
    dbg_print("\t Vdd : %d mv, %d mv\n", vpp->value, vpp->offset);

    err = power_voltage_set(vdd);
    if (err) {
        err_print("power_voltage_set(VDD) failed\n");
        return ERR_VDD;
    }

    err = power_voltage_set(vpp);
    if (err) {
        err_print("power_voltage_set(VPP) failed\n");
        return ERR_VDD;
    }

    return ERR_OK;
}

int32_t ui_key_process(struct key_event* key)
{
    int32_t ret;
    struct key_event key_new;
    struct tlv_chip*         chip;
    struct tlv_rolling_code* roll;
    
    key_new.state = KEY_STATE_PRESS;    
    
    dbg_print("key Code=%d\n", key->code);
    gui_key_process(key);

    switch (key->code) {
    case KEY_CODE_FIRMWARE:
        /* This 500 ms delay was made the last USB-HID packet which had been send to host. */        
        dbg_print("[UI] Got a new file firmware ...\n");
        key->code = KEY_CODE_REBOOT;
        key_set(key);
        ret = ERR_OK;
        break;
    case KEY_CODE_REBOOT:
        fdelete("file_dev.bin", NULL);
        osDelay(1000);
        NVIC_SystemReset();
        break;
    case KEY_CODE_HEX:
        dbg_print("[UI] Got a new file_hex ...\n");
        ret = file_hex_free();
        if (ret) {
            goto ERR_UI_KEY_PROCESS;
        }
        ret = file_hex_load();
        if (ret) {
            goto ERR_UI_KEY_PROCESS;
        }

        roll = file_hex_get_rolling_code(0);
        if (roll) {
        ret = cswrite_cfg_set_rolling_code(roll);
            if (ret) {
                goto ERR_UI_KEY_PROCESS;
            }
        }

        chip = file_hex_get_chip(0);
        if (chip) {
            goto ERR_UI_KEY_PROCESS;
        }

        ret = cswrite_cfg_set_chip((const char*)chip->name, chip->id, chip->id_length);
        if (ret) {
            goto ERR_UI_KEY_PROCESS;
        }

        ret = file_ic_set_code();
        /* 
        * The value of Vdd and Vpp had been setup when system initialize or file_hex updated.
        * So we should avoid the charging process at here.
        */
        ret = power_setup(); 
        if (ret) {
            err_print("power_setup() failed\n");
            return ret;
        }

        key_new.code = KEY_CODE_GUI_HEX_UPGRADED;
        key_set(&key_new);
        break;
    case KEY_CODE_CFG: {
            int32_t bitmap;
            struct tlv_file file;

            ret = file_cfg_load(&file);
            if (ret) {
                goto ERR_UI_KEY_PROCESS;
            }

            bitmap = file_cfg_get_program(&file);
            if (bitmap & TLV_PROGRAM_ACTIVE) {
                struct tlv_date* day;
                day = file_cfg_get_date(&file);
                ret = cswrite_cfg_set_active_date(day);
                break;
            }

            if (bitmap & TLV_PROGRAM_UPGRADE_HEX) {
                struct tlv_date* day;
                struct tlv_time* tm;
                day = file_cfg_get_date(&file);
                tm = file_cfg_get_time(&file);
                ret = cswrite_log_set_hex_time(day, tm);
                break;
            }
            ret = chip_program(bitmap);
            if (ret) {
                goto ERR_UI_KEY_PROCESS;
            }
        }
        break;
    case KEY_CODE_READ_IC: {            
            int32_t bitmap;
            bitmap = TLV_PROGRAM_READ;
            ret = chip_program(bitmap);
            if (ret) {
                goto ERR_UI_KEY_PROCESS;
            }
        }
        break;
    case KEY_CODE_PROGRAM:
        if ( key->state == KEY_STATE_PRESS) {        
            dbg_print("[UI] Got Program Key Pressed\n");
            ret = chip_program(0);
        }
        break;
    
    case KEY_CODE_FUNC:
        if (key->state == KEY_STATE_PRESS) {         
            dbg_print("[UI] Got Menu Key Pressed\n");
        }
        break;
    default:
        break;
    }
    
ERR_UI_KEY_PROCESS: 
    dbg_print("%s(),ret=0x%x\n", __func__, ret);
    err_handler(ret);   
    return ret;
}


