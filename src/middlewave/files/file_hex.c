#include <stdint.h>
#include <string.h>
#include "error.h"
#include "files.h"
#include "common.h"
#include "file_hex.h"
#include "flash_map.h"
#include "tlv.h"
#include "list.h"
#include "key.h"
#include "cswrite_cfg.h"

#define MODULE_DEBUG       1
#define MODULE_ERROR       1

#include "dbg_print.h"

#define FILE_HEX_EXT_OFFSET                 0
#define FILE_HEX_EXT_SIZE                  (0)
#define FILE_HEX_OFFSET                    (FILE_HEX_EXT_OFFSET + FILE_HEX_EXT_SIZE)

static struct tlv_file file_hex;

int32_t file_hex_init(void)
{
    tlv_file_init(&file_hex);
    return file_hex_load();
}

void* file_hex_open(const char* mode)
{
    return fopen(FILE_HEX, mode);
}

int32_t file_hex_read(uint8_t* buf, size_t size, void* fp)
{
    return fread(buf, 1, size, fp);
}

int32_t file_hex_write(const uint8_t* data, size_t size, void* fp)
{
    return fwrite(data, 1, size, fp);
}

int32_t file_hex_close(void* fp)
{    
    return fclose(fp);
}

int32_t file_hex_upgrade(void)
{
    int32_t ret;
    struct key_event evt;

    evt.code  = KEY_CODE_HEX;
    evt.state = KEY_STATE_PRESS;

    ret = key_set(&evt);
    if (ret) {
        err_print("key_set(%p) failed, ret=%d\n", &evt, ret);
        return ret;
    }
    
    return ERR_OK;
}

int32_t file_hex_load(void)
{
    int32_t ret;
    struct tlv_file* hex;

    hex = &file_hex;

    dbg_print("[File-Hex] file_hex load ...\n");
    ret = file_verify(TLV_FILE_ID_HEX);
    if (!ret) {
        err_print("hex_is_valid() failed\n");
        return ERR_FILE;
    }

    return tlv_file_unpack(TLV_FILE_ID_HEX , hex);
}

int32_t file_hex_free(void)
{
    int32_t ret;
    struct tlv_file* hex;

    hex = &file_hex;
    
    return tlv_file_destroy(hex);
}

struct tlv_chip* file_hex_get_chip(int32_t index)
{
    int32_t i = 0;
    struct list_head*     pos;
    struct list_head*     list;
    struct tlv_desc_chip* desc;
    struct tlv_node_chip* node;
    struct tlv_file*      hex;

    list = &file_hex.list_chip;

    /* There is only one chip descriptor in one hex. */
    if (list == NULL) {
        err_print("Invalid arguments chip == %p\n", list);
        return NULL;
    }

    list_for_each(pos, list) {
        node = list_entry(pos,  struct tlv_node_chip, node);
        desc = &node->desc;
        if (i++ == index) {
            return &desc->body;  
        }            
    }

    return NULL;
}

struct tlv_voltage* file_hex_get_voltage(int32_t type)
{
    struct list_head*        pos;
    struct list_head*     list;
    struct tlv_desc_voltage* desc;
    struct tlv_node_voltage* node_voltage;
    
    list = &file_hex.list_voltage;    

    if (list == NULL || type > TLV_DESC_VOLTAGE_TYPE_MAX || type < 0) {
        err_print("Error, Invalid arguments list=%p,type=%d\n", list, type);
        return NULL;
    }

    list_for_each(pos, list) {
        node_voltage = list_entry(pos,  struct tlv_node_voltage, node);
        desc = &node_voltage->desc;
        if (desc->body.type == type) {
            return &desc->body;
        }
    }

    return NULL;
}

struct tlv_osc* file_hex_get_osc(int32_t type)
{
    struct list_head*    pos;
    struct list_head* list;
    struct tlv_desc_osc* desc;
    struct tlv_node_osc* node_osc;
    
    list = &file_hex.list_osc;    

    if (list == NULL) {
        err_print("[Error, Invalid arguments type == %d, list=%p\n", type, list);
        return NULL;
    }

    list_for_each(pos, list) {
        node_osc = list_entry(pos,  struct tlv_node_osc, node);
        desc = &node_osc->desc;
        if (desc->body.type == type) {
            return &desc->body;
        }
    }

    return NULL;
}


struct tlv_option* file_hex_get_option(int32_t index)
{
    int32_t i = 0;
    struct list_head*       pos;
    struct list_head*       list;
    struct tlv_desc_option* desc;
    struct tlv_node_option* node_option;

    list = &file_hex.list_option;

    if (list == NULL) {
        err_print("[Error, Invalid arguments option=%p\n", list);
        return NULL;
    }

    list_for_each(pos, list) {
        node_option = list_entry(pos,  struct tlv_node_option, node);
        desc = &node_option->desc;
        if (i++ == index) {
            return &desc->body;      
        }
    }

    return NULL;
}

struct tlv_memory* file_hex_get_memory(int32_t index)
{
    int32_t                 i = 0;
    struct list_head*       pos;
    struct list_head* list;
    struct tlv_desc_memory* desc;
    struct tlv_node_memory* node_mem;
    
    list = &file_hex.list_memory;

    if (list == NULL) {
        err_print("[Error, Invalid arguments num=%p\n", list);
        return NULL;
    }

    list_for_each(pos, list) {
        node_mem = list_entry(pos,  struct tlv_node_memory, node);
        desc = &node_mem->desc;
        if (i++ == index) {
            return &desc->body;
        }
    }

    return NULL;
}


struct tlv_rolling_code* file_hex_get_rolling_code(int32_t index)
{
    int32_t i = 0;
    struct list_head* list;
    struct list_head*             pos;
    struct tlv_desc_rolling_code* desc;
    struct tlv_node_rolling_code* node_rolling_code;
    
    list = &file_hex.list_rolling_code;

    if (list == NULL) {
        err_print("Error, Invalid arguments rol=%p\n", list);
        return NULL;
    }

    list_for_each(pos, list) {
        node_rolling_code = list_entry(pos,  struct tlv_node_rolling_code, node);
        desc = &node_rolling_code->desc;
        if (i++ == index) {
            return &desc->body;
        }
    }

    return ERR_OK;
}

struct tlv_limit* file_hex_get_limit(int32_t index)
{
    int32_t i = 0;
    struct list_head* pos;
    struct list_head* list;
    struct tlv_desc_limit* desc;
    struct tlv_node_limit* node_code;
    
    list = &file_hex.list_limit;

    list_for_each(pos, list) {
        node_code = list_entry(pos, struct tlv_node_limit, node);
        desc = &node_code->desc;
        if (i++ == index) {
            return &desc->body;   
        }            
    }

    return NULL;
}

struct tlv_code* file_hex_get_code(int32_t index)
{
    int32_t i = 0;
    struct list_head* pos;
    struct list_head* list;
    struct tlv_desc_code* desc;
    struct tlv_node_code* node_code;

    list = &file_hex.list_code;

    list_for_each(pos, list) {
        node_code = list_entry(pos, struct tlv_node_code, node);
        desc = &node_code->desc;
        if (i++ == index) {
            return &desc->body;  
        }
    }

    return NULL;
}

int32_t file_hex_get_program(void)
{
    int32_t i = 0;
    struct list_head* pos;
    struct list_head* list;
    struct tlv_desc_program* desc;
    struct tlv_node_program* node_program;

    
    list = &file_hex.list_program;

    if (list == NULL) {
        err_print("[File Hex] Error, Invalid arguments list=%p\n", list);
        return NULL;
    }

    list_for_each(pos, list) {
        node_program = list_entry(pos, struct tlv_node_program, node);
        desc = &node_program->desc;
        return desc->body.bitmap;

    }

    return 0;
}

uint32_t file_hex_get_crc32(void)
{    
    return file_hex.crc32;
}
