#include "files.h"
#include "error.h"
#include "tlv.h"
#include "key.h"
#include "file_cfg.h"
#include "crc32_soft.h"

#define MODULE_DEBUG       1
#define MODULE_ERROR       1

#include "dbg_print.h"


void* file_cfg_open(const char* mode)
{    
    return fopen(FILE_CFG, mode);
}

int32_t file_cfg_write(const uint8_t* data, size_t size, void* fp)
{
    if (data == NULL || fp == NULL) {
        err_print("erro, data=0x%p,fp=0x%p\n", data, fp);
        return 0;
    }

    return fwrite(data, 1, size, fp);
}

int32_t file_cfg_read(uint8_t* buf, size_t size, void* fp)
{
    if (buf == NULL || fp == NULL) {
        err_print("erro, buf=0x%p,fp=0x%p\n", buf, fp);
        return 0;
    }

    return fread(buf, 1, size, fp);
}


int32_t file_cfg_close(void* fp)
{    
    return fclose(fp);
}

int32_t file_cfg_upgrade(void)
{
    int32_t ret;
    struct key_event evt;

    evt.code  = KEY_CODE_CFG;
    evt.state = KEY_STATE_PRESS;

    ret = key_set(&evt);
    if (ret) {
        err_print("key_set(%p) failed, ret=%d\n", &evt, ret);
        return ret;
    }
    
    return ERR_OK; 
}

int32_t file_cfg_load(struct tlv_file* tlv)
{
    int32_t ret;
    
    ret = tlv_file_init(tlv);
    if (ret) {
    }

    ret = tlv_file_unpack(TLV_FILE_ID_CFG, tlv);
    if (ret) {
    }    

    return ret;
}

int32_t file_cfg_get_program(struct tlv_file* tlv)
{
    struct list_head* pos;
    struct tlv_node_program* node_program;

    list_for_each(pos, &tlv->list_program) {
        node_program = list_entry(pos, struct tlv_node_program, node);
        return node_program->desc.body.bitmap;
    }
    
    return 0;
}

struct tlv_date* file_cfg_get_date(struct tlv_file* tlv)
{
    struct list_head* pos;
    struct tlv_node_time* node_time;

    list_for_each(pos, &tlv->list_program) {
        node_time = list_entry(pos, struct tlv_node_time, node);
        return &node_time->desc.day;
    }
    
    return NULL;    
}

struct tlv_time* file_cfg_get_time(struct tlv_file* tlv)
{
    struct list_head* pos;
    struct tlv_node_time* node_time;

    list_for_each(pos, &tlv->list_program) {
        node_time = list_entry(pos, struct tlv_node_time, node);
        return &node_time->desc.time;
    }
    
    return NULL;    
}


