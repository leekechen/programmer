#include <stdint.h>
#include <stm32f4xx_hal.h>
#include "embed_flash.h"
#include "cswrite_cfg.h"
#include "config.h"
#include "error.h"
#include "files.h"
#include "key.h"
#include "crc32_soft.h"
#include "fram_map.h"
#include "file_log.h"
#include "program.h"
#include "cswrite_log.h"

#define MODULE_DEBUG       1 
#define MODULE_ERROR       1

#include "dbg_print.h"

struct cswrite_cfg {
    struct   tlv_date       active_day;
    uint8_t                 language_id;
    uint8_t                 buzzer_state;
    uint8_t                 firmware_bak;
    uint8_t                 upgrade_flag;
    uint32_t                total_program_count;
    struct chip_operation*  ops;
    struct tlv_rolling_code roll;
    uint32_t                crc32;
} ;

extern const struct chip_operation csurp3228_ops;


#ifdef CSU8RP3228
#define OPS_CSU8RP3228         &csurp3228_ops,
#else    

#define OPS_CSU8RP3228         
#endif

#define CSWRITE_CFG_CHIP_NUM       (sizeof(cswrite_chip_list)/sizeof(char*))


static const char* cswrite_chip_list[] = {
    /* CSU8RT30213 */
    STR_CSU8RP3212
    STR_CSU8RP3215
    STR_CSU8RP3216
    STR_CSU8RP3217
    STR_CSU8RP3218
    STR_CSU8RP3219
    STR_CSU8RP2215
    STR_CSU8RP2218

    /* CSU8RP3228 */ 
    STR_CSU8RP3234 
    STR_CSU8RP3235 
    STR_CSU8RP3236 
    STR_CSU8RP3225 
    STR_CSU8RP3226
    STR_CSU8RP3418 
    STR_CSU8RP3424 
    STR_CSU8RP3425 
    STR_CSU8RP3426
    STR_CSU8RP3429 
    NULL
} ;


#define CSWRITE_CHIP_OPS_NUM        ((sizeof(cswrite_chip_ops_list))/(sizeof(struct chip_operation*)))

static const struct chip_operation* cswrite_chip_ops_list[] = {
    OPS_CSU8RP3228
} ;

static const char* ver_str[] = {
    "Hardware Version",
    "Firmware Version",
    "Bootloader Version",
    "PC Software Version",
} ;



static struct cswrite_cfg cfg;

//static int32_t firmware_upgrade_notify(void)
//{
//    struct key_event key;
//    
//    key.code  = KEY_CODE_FIRMWARE;
//    key.state = KEY_STATE_PRESS;
//    return key_set(&key);
//}

//static int32_t hex_upgrade_notify(void)
//{
//    struct key_event key;
//    
//    key.code  = KEY_CODE_HEX;
//    key.state = KEY_STATE_PRESS;
//    return key_set(&key);
//}

static uint32_t calc_crc32(const uint8_t* data, size_t size)
{
    return crc32_sw_calc(0xFFFFFFFF, data, size) ^ 0xFFFFFFFF;
}


static int32_t cswrite_cfg_save(void)
{
    int32_t ret;
    int32_t size;
    uint32_t addr;
    uint8_t* buf;

    buf  = (uint8_t*)&cfg;
    size = sizeof(struct cswrite_cfg);
    addr = 0;

    cfg.crc32 = calc_crc32(buf, size - 4);
    dbg_dump("\t CSWrite Cfg", buf, sizeof(cfg));
    ret = fram_map_cfg_write(addr, buf, size);
    if (size != ret) {
        err_print("Error, fram_map_cfg_write() failed\n");
        return ERR_DRV_FRAM;
    }

    return ERR_OK;
}

static const char* get_chip_name(int32_t index)
{
    if (index >= (sizeof(cswrite_chip_list)/sizeof(char*))) {
        err_print("Invalid argument, index=%d\n", index);;
        return NULL;
    }

    return (const char*)cswrite_chip_list[index];
}

int32_t cswrite_get_version(struct tlv_version* ver)
{
    uint8_t* version;
    
    if (ver == NULL) {
        err_print("Error, Invalid argument, type=%d, version=%p\n", ver->type, ver);
        return ERR_SYS_ARGUMENT;
    }

    version = (uint8_t*)&ver->value;

    memset(&ver->value, 0, sizeof(ver->value));
    switch (ver->type) {
    case TLV_DESC_VERSION_TYPE_HW:
        version[1] = HW_VERSION_MAJOR;
        version[2] = HW_VERSION_MINI;
        version[3] = HW_VERSION_REVERSION;
        break; 
    case TLV_DESC_VERSION_TYPE_FM:
        version[1] = FIRMWARE_VERSION_MAJOR;
        version[2] = FIRMWARE_VERSION_MINI;
        version[3] = FIRMWARE_VERSION_REVERSION;
        break;   
    case TLV_DESC_VERSION_TYPE_BOOT:
        version[1] = BOOTLOADER_VERSION_MAJOR;
        version[2] = BOOTLOADER_VERSION_MIN;
        version[3] = BOOTLOADER_VERSION_REVERSION;
        break; 
    case TLV_DESC_VERSION_TYPE_PC:
        break;
    default:
        err_print("Error, Invalid argument, type=%d, version=%p\n", ver->type, ver);        
        return ERR_SYS_ARGUMENT; 
    }
    
    return ERR_OK;
}



int32_t cswrite_cfg_load(void)
{
    int32_t ret;
    int32_t size;
    uint32_t addr;
    uint8_t* buf;
    uint32_t crc32;

    buf  = (uint8_t*)&cfg;
    size = sizeof(struct cswrite_cfg);
    addr = 0;
    
    ret = fram_map_cfg_read(addr, buf, size);
    if (size != ret) {
        err_print("fram_map_cfg_write() failed\n");
        return ERR_DRV_FRAM;
    }

    crc32 = calc_crc32(buf, size - 4);
    if (crc32 != cfg.crc32) {
        return ERR_DRV_FRAM;     
    }
    
    dbg_dump("\t CSWrite Cfg", buf, sizeof(cfg));


    return ERR_OK;
}

int32_t cswrite_cfg_reset(void)
{
    memset(&cfg, 0, sizeof(cfg));

    return cswrite_cfg_save();
}


uint32_t cswrite_cfg_get_chip_size(void)
{
    return ERR_UPDATE_SEQ;
}

int32_t cswrite_cfg_get_serial_id(int32_t type, uint8_t* id)
{
    if (id == NULL || type > TLV_DESC_SERIAL_ID_TYPE_32_BITS || type < TLV_DESC_SERIAL_ID_TYPE_96_BITS) {
        err_print("Error, Invalid argument, type=%d, id=%p\n", type, id);
        return ERR_SYS_ARGUMENT;
    }

    if (type == TLV_DESC_SERIAL_ID_TYPE_96_BITS) {
        HAL_GetUID((uint32_t*)id);
    }
    else if (type == TLV_DESC_SERIAL_ID_TYPE_32_BITS) {
        memcpy(id, (uint8_t*)CSWRITER_UNIQUE_ID_ADDR_BASE, CSWRITER_UNIQUE_ID_MAX_SIZE);
    } 
    
    return ERR_OK;
}

int32_t cswrite_cfg_get_language(uint8_t* lang_id)
{
    *lang_id = cfg.language_id;
    return ERR_OK;
}

int32_t cswrite_cfg_set_language(uint8_t lang_id)
{
    cfg.language_id = lang_id;
    return cswrite_cfg_save();    
}

int32_t cswrite_cfg_get_buzzer_state(uint8_t* state)
{
    *state = cfg.buzzer_state;
    return ERR_OK;
}

int32_t cswrite_cfg_set_buzzer_state(uint8_t state)
{
    cfg.buzzer_state= state;
    return cswrite_cfg_save();  
}

int32_t cswrite_cfg_get_active_day(struct tlv_date* day)
{
    memcpy(day, &cfg.active_day, sizeof(struct tlv_date));
    return ERR_OK;  
}

int32_t cswrite_cfg_set_active_date(struct tlv_date* day)
{
    memcpy(&cfg.active_day, day, sizeof(struct tlv_date));
    return cswrite_cfg_save();  
}

int32_t cswrite_cfg_set_firmware_state(uint32_t flag)
{

    return ERR_OK;  
}

int32_t cswrite_cfg_set_hex_state(uint32_t flag)
{


    return ERR_OK;
}

int32_t cswrite_cfg_get_firmware_state(uint32_t* flag)
{
    *flag = cfg.upgrade_flag;
    return ERR_OK;
}


int32_t cswrite_cfg_get_id(struct list_head* list)
{
    int32_t                  err;
    uint8_t*                 buf;
    uint32_t                 num_ok;
    uint32_t                 num_failed;
    struct tlv_desc_id*      desc_id;
    struct tlv_node_id*      node_id = NULL;

    dbg_print("[File-Dev] create ID Descriptor ...\n");
    
    node_id = calloc(1, sizeof(struct tlv_node_id));
    if (node_id == NULL) {        
        err_print("Not enough memory, calloc(1, %d)\n", sizeof(struct tlv_node_id));
        err = ERR_SYS_NOT_ENOUGH_MEMORY;        
        goto ERR_CREATE_DESC_ID;
    }
    desc_id = &node_id->desc;
    
    desc_id->head.tag    = TLV_DESC_TAG_ID;
    desc_id->head.length = TLV_DESC_ID_BODY_LENGTH;
    
    /* Read the CSWrite3.0 Hardware Unique ID */
    desc_id->body.type = TLV_DESC_SERIAL_ID_TYPE_32_BITS;
    buf  = (uint8_t*)&desc_id->body.serial_id;
    err  = cswrite_cfg_get_serial_id(TLV_DESC_SERIAL_ID_TYPE_32_BITS, buf);
    if (err) {
        err_print("cswrite_get_serial_id() failed, err=0x%x\n", err);        
        goto ERR_CREATE_DESC_ID;
    }
    dbg_print("\t serail_id:%02x%02x%02x%02x\n", buf[0], buf[1], buf[2], buf[3]);
    
    /* Read the CSWrite3.0 Active time */   
    err = cswrite_cfg_get_active_day(&desc_id->body.day);    
    if (err) {
        err_print("swrite_get_active_date() failed, err=0x%x\n", err);        
        goto ERR_CREATE_DESC_ID;
    }
    dbg_print("\t Active Time(YY//MM//DD):%02d/%02d/%02d\n", desc_id->body.day.year, desc_id->body.day.month, desc_id->body.day.date);
    
    /* Read the CSWrite3.0 Total program count in life cycles */
    num_ok = cswrite_log_get_num_ok();    

    dbg_print("\t Total Program OK num:%d\n", num_ok);
    
    num_failed = cswrite_log_get_num_failed();    

    dbg_print("\t Total Program Failed num:%d\n", num_failed);

    desc_id->body.count = num_ok + num_failed;
    
    list_add(&node_id->node, list);
    
    return ERR_OK;
    
ERR_CREATE_DESC_ID:
    if (node_id) {
        free(node_id);
    }
    return err;
}

int32_t cswrite_cfg_set_chip(const char* name, uint8_t* id, size_t id_len)
{
    int32_t i;
    int32_t err;
    struct chip_operation* ops;

    for (i = 0; i < CSWRITE_CHIP_OPS_NUM; i++) {
        ops = (struct chip_operation*)cswrite_chip_ops_list[i];
        if (!ops->is_matched) {
            continue;
        }
        
        err = ops->is_matched(name, id, id_len);
        if (err) {
            cfg.ops = ops;
            return ERR_OK;
        }
    }

    return ERR_UPDATE_SEQ;
}

struct chip_operation* cswrite_cfg_get_chip_ops(void)
{
    return cfg.ops;
}

struct tlv_rolling_code* cswrite_cfg_get_rolling_code(int32_t flag)
{    
    if (cfg.roll.width == 0) {
        return NULL;
    }
    if (flag) {
        switch (cfg.roll.width) {
        case 8: {
                uint8_t* byte;
                byte = (uint8_t*)&cfg.roll.value;
                *byte = (*byte) + cfg.roll.step;
                goto SAVE_ROLLING_CODE;
            }
        case 16: {
                uint16_t* half;
                half = (uint16_t*)&cfg.roll.value;
                *half = (*half) + cfg.roll.step;
                goto SAVE_ROLLING_CODE;
            }
        case 32:{
                uint32_t* word;
                word = (uint32_t*)&cfg.roll.value;
                *word = (*word) + cfg.roll.step;
                goto SAVE_ROLLING_CODE;
            }
        default:
            break;
        }
SAVE_ROLLING_CODE:
        cswrite_cfg_save();
    }
    return &cfg.roll;
}

int32_t cswrite_cfg_set_rolling_code(struct tlv_rolling_code* roll)
{
    memcpy(&cfg.roll, roll, sizeof(struct tlv_rolling_code)); 
    return cswrite_cfg_save();   
}


int32_t cswrite_cfg_get_version(struct list_head* list)
{
    int32_t                  err;
    int32_t                  type;
    struct tlv_desc_version* desc_version;
    struct tlv_node_version* node_version = NULL;
    
    dbg_print("[File-Dev] Create Version Descriptor ...\n");
    
    /* Read the CSWrite3.0's version list */
    for (type = TLV_DESC_VERSION_TYPE_HW; type <= TLV_DESC_VERSION_TYPE_PC; type++) {
        node_version = calloc(1, sizeof(struct tlv_node_version));
        if (node_version == NULL) {        
            err_print("Not enough memory, calloc(1, %d)\n", sizeof(struct tlv_node_version));
            err = ERR_SYS_NOT_ENOUGH_MEMORY;
            goto ERR_CREATE_DESC_VERSION;
        }
        desc_version = &node_version->desc;

        desc_version->head.tag    = TLV_DESC_TAG_VERSION;
        desc_version->head.length = TLV_DESC_VERSION_BODY_LENGTH;
        
        desc_version->body.type = type;
        err = cswrite_get_version(&desc_version->body);
        if (err) {
            err_print("cswrite_get_version() failed\n");
            goto ERR_CREATE_DESC_VERSION;
        }
        dbg_print("\t %s:%x\n", ver_str[type - 1], desc_version->body.value);
        
        list_add(&node_version->node, list);        
    }
    
    return ERR_OK;
    
ERR_CREATE_DESC_VERSION:
    if (node_version) {
        free(node_version);
    }
    
    return err;

}

int32_t cswrite_cfg_get_chip(struct list_head* list)
{
    uint32_t                 i;
    size_t                   size;
    const char*              name;
    struct tlv_desc_chip*    desc_chip;
    struct tlv_node_chip*    node_chip = NULL;
    
    for (i = 0; i < CSWRITE_CFG_CHIP_NUM; i++) {
        name = get_chip_name(i);
        if (name == NULL) {
            break;
        }
        size = sizeof(struct tlv_node_chip);
        node_chip = tlv_desc_new(size);
        if (node_chip == NULL) {        
            err_print("Not enough memory, calloc(1, %d) failed\n", size);
            return ERR_SYS_NOT_ENOUGH_MEMORY;
        }    
        desc_chip = &node_chip->desc; 
        
        desc_chip->body.name        = (uint8_t*)name;
        desc_chip->body.name_length = strlen(name);
        
        /* 3 bytes include 'name_length', 'id_length', 'type_length' */
        size = 3; 
        size += desc_chip->body.name_length;
        size += desc_chip->body.id_length;
        size += desc_chip->body.type_length;

        TLV_DESC_HEAD_INIT(&desc_chip->head, TLV_DESC_TAG_CHIP, size);

        list_add(&node_chip->node, list);
    }
    
    return ERR_OK;
}

