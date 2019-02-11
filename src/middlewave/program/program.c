#include "error.h"
#include "program.h"
#include "file_hex.h"
#include "power.h"
#include "cswrite_cfg.h"
#include "cswrite_log.h"
#include "delay.h"
#include "files.h"

#define MODULE_DEBUG       1 
#define MODULE_ERROR       1

#include "dbg_print.h"

#define CHIP_PROGRAM_UNINIT     0
#define CHIP_PROGRAM_INIT       1

struct chip_program {
    int32_t                flag;
    uint8_t                bitmap;
    struct tlv_file        ic;
} ;

static struct chip_program program;

static int32_t power_is_valid(int32_t type)
{
    int32_t             err;
    uint32_t            mv;
    uint32_t            mv_upper_limit;
    uint32_t            mv_lower_limit;
    struct tlv_voltage* voltage;
    struct tlv_file*    hex;
    
    voltage = file_hex_get_voltage(type);
    if (voltage == NULL) {
        return err;
    }
        
    err = power_voltage_is_valid(voltage);
    if (!err) {
        return err;
    }

    return ERR_OK;

VOLTAGE_ERROR:    
    if (voltage->type == TLV_DESC_VOLTAGE_TYPE_VDD) {
        return ERR_VDD;
    } 

    return ERR_VPP;
}



int32_t chip_program_power_on(uint32_t vpp_delay_us, uint32_t stable_delay_ms)
{
    int32_t err;
    
    err = power_voltage_switch(TLV_DESC_VOLTAGE_TYPE_VDD, POWER_FLAG_ON);
    if (err) {
        err_print("power_voltage_switch(VDD, ON) failed\n");
        return ERR_VDD;
    }

    delay_us(vpp_delay_us);

    err = power_voltage_switch(TLV_DESC_VOLTAGE_TYPE_VPP, POWER_FLAG_ON);
    if (err) {
        err_print("power_voltage_switch(VPP, ON) failed\n");
        return ERR_VPP;
    }

    delay_ms(stable_delay_ms);

    return ERR_OK;

}

int32_t chip_program_power_off(void)
{
    int32_t err;
    
    err = power_voltage_switch(TLV_DESC_VOLTAGE_TYPE_VDD, POWER_FLAG_OFF);
    if (err) {
        err_print("power_voltage_switch(VDD, OFF) failed\n");
        return ERR_VDD;
    }

    err = power_voltage_switch(TLV_DESC_VOLTAGE_TYPE_VPP, POWER_FLAG_OFF);
    if (err) {
        err_print("power_voltage_switch(VPP, OFF) failed\n");
        return ERR_VPP;
    }

    return ERR_OK;
}

int32_t chip_program_is_power_ready(void)
{
    int32_t err;
    uint32_t mv;
    struct tlv_voltage* voltage;
    struct tlv_file* hex;
            
    dbg_print("[Program] Vdd Checking ...\n");
    voltage = file_hex_get_voltage(TLV_DESC_VOLTAGE_TYPE_VDD);
    if (voltage == NULL) {
        err_print("Got Vdd voltage from file_hex failed\n");
        return ERR_VDD;
    }
    dbg_print("\t file_hex config: Rated=%d mv, Error=%d mv\n", voltage->value, voltage->offset);

    err = power_voltage_is_valid(voltage);
    if (!err) {
        err_print("Invalid Vdd\n");
        return ERR_VDD;
    }
    dbg_print("\t Vdd is OK\n");    
        
    dbg_print("[Program] Vpp Checking ...\n");
    voltage = file_hex_get_voltage(TLV_DESC_VOLTAGE_TYPE_VPP);
    if (voltage == NULL) {
        err_print("Got Vpp voltage from file_hex failed\n");
        return ERR_VPP;
    }
    dbg_print("\t file_hex config: Rated=%d mv, error=%d mv\n", voltage->value, voltage->offset);
    
    err = power_voltage_is_valid(voltage);
    if (!err) {
        err_print("Invalid Vpp\n");
        return ERR_VPP;
    }
    dbg_print("\t Vpp is OK\n");

    return ERR_OK;
}



int32_t chip_program_is_valid(void)
{
    int32_t err;

    err = chip_program_is_power_ready();

    return err;
}

int32_t chip_program_setup(uint8_t bitmap)
{
    return ERR_OK;
}

int32_t chip_program_get_size(void)
{
}

int32_t chip_program(uint32_t bitmap)
{
    int32_t err; 
    int32_t  i;
    uint32_t addr;
    size_t   size;
    uint32_t ok;
    struct tlv_file* hex;
    struct chip_operation* ops;
    struct tlv_limit* limit;


    ops = cswrite_cfg_get_chip_ops();
    if (ops == NULL) {
        return ERR_UPDATE_SEQ;
    }
    
    if (bitmap == 0) {
        bitmap = file_hex_get_program();
    }
    
    dbg_print("[Program] checking program enviroment ...\n");
    err = chip_program_is_valid();
    if (err) {
        err_print("Invalid Program Enviroment \n");
        goto PROGRAM_ERROR;
    }
    
    if (err == ERR_OK 
        && ops->hande_shake) {
        dbg_print("[Program] hande_shake() ...\n");
        err = ops->hande_shake(hex);
        if (err) {
            goto PROGRAM_ERROR;
        }
    }
    
    if (err == ERR_OK && ops->get_id) {
        dbg_print("[Program] get_id() ...\n");
        err = ops->get_id(hex);
        if (err) {
            goto PROGRAM_ERROR;
        }
    }
    
    if (err == ERR_OK && ops->osc_cali) {
        dbg_print("[Program] osc_cali() ...\n");
        err = ops->osc_cali(hex);
        if (err) {
            goto PROGRAM_ERROR;
        }
    }
    
    if (err == ERR_OK && ops->wdt_cali) {
        dbg_print("[Program] wdt_cali() ...\n");
        err = ops->wdt_cali(hex);
        if (err) {
            goto PROGRAM_ERROR;
        }
    }
    
    if ((bitmap & TLV_PROGRAM_BLANK_CHECK) 
       && err == ERR_OK 
       && ops->blank_check) {
        dbg_print("[Program] blank_check() ...\n");
        err = ops->blank_check(hex);
        if (err) {
            goto PROGRAM_ERROR;
        }
    }
    
    if ((bitmap & TLV_PROGRAM_ERASE) 
       && (err == ERR_OK) 
       && ops->erase) {
        dbg_print("[Program] erase() ...\n");
        err = ops->erase(hex);
        if (err) {
            goto PROGRAM_ERROR;
        }
    }
    
    if ((bitmap & TLV_PROGRAM_PROGRAM) 
       && (err == ERR_OK) 
       && ops->program) {
        dbg_print("[Program] program() ...\n");
        limit = file_hex_get_limit(0);
        if (limit != NULL && limit->count != PROGRAM_NO_LIMIT) {
            ok = cswrite_log_get_num_ok();

            if (ok >= limit->count) {
                
                dbg_print("[Program] program count limit=%d, current=%d\n", limit->count, ok);
                return ERR_PRG_LIMIT;
            }
        }
        
        err = ops->program(hex);
        if (err) {
            goto PROGRAM_ERROR;
        }       
    }
    
    if ((bitmap & TLV_PROGRAM_VERIFY)
        && err == ERR_OK 
        && ops->verify) {
        dbg_print("[Program] verify() ...\n");
        err = ops->verify(hex);
        if (err) {
            goto PROGRAM_ERROR;
        }
    }
    
    if ((bitmap & TLV_PROGRAM_READ) 
        && err == ERR_OK 
        && ops->readback) {
        dbg_print("[Program] readback() ...\n");
        err = ops->readback(hex);
        if (err) {
            goto PROGRAM_ERROR;
        }
    }  
    osDelay(100);
    chip_program_power_off();

    return ERR_OK;
    
PROGRAM_ERROR: 
    
    err_print("failed, error=%x\n", err);
    osDelay(100);
    chip_program_power_off();
    
    return err;
}   




