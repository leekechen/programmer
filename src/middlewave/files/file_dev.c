#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "tlv.h"
#include "error.h"
//#include "version.h"
#include "crc32.h"
#include "crc32_soft.h"
#include "files.h"
#include "common.h"
#include "file_dev.h"
#include "cswrite_cfg.h"
#include "cswrite_log.h"

#define MODULE_DEBUG        1
#define MODULE_ERROR        1

#include "dbg_print.h"



void* file_dev_open(const char* mode)
{
    return fopen(FILE_DEV, mode);
}

int32_t file_dev_read(uint8_t* buf, size_t size, void* fp)
{
    return fread(buf, 1, size, fp);
}

int32_t file_dev_write(const uint8_t* data, size_t size, void* fp)
{
    return fwrite(data, 1, size, fp);
}

int32_t file_dev_close(void* fp)
{
    return fclose(fp);
}

int32_t file_dev_init(void)
{
    int32_t ret;

    ret = file_get_size(TLV_FILE_ID_DEV);
    if (ret <= 0) {
        return file_dev_upgrade();
    }
    dbg_print("[File-Dev] file size=%d\n", ret);
    ret = file_verify(TLV_FILE_ID_DEV);
    if (!ret) {
        return file_dev_upgrade();
    }
    
    return ERR_OK;        
}

int32_t cswrite_get_id(struct tlv_id* id)
{
    int32_t                  err;
    uint8_t*                 buf;
    uint32_t                 num_ok;
    uint32_t                 num_failed;
    struct tlv_desc_id*      desc_id;
    struct tlv_node_id*      node_id = NULL;

    dbg_print("[File-Dev] Get ID Descriptor ...\n");
    
    if (id == NULL) {
        err_print("Invalid argument, id=%p\n", id);
        return ERR_SYS_ARGUMENT;
    }    
    
    /* Read the CSWrite3.0 Hardware Unique ID */
    id->type = TLV_DESC_SERIAL_ID_TYPE_32_BITS;
    buf  = (uint8_t*)&id->serial_id;
    err  = cswrite_cfg_get_serial_id(TLV_DESC_SERIAL_ID_TYPE_32_BITS, buf);
    if (err) {
        err_print("cswrite_get_serial_id() failed, err=0x%x\n", err);        
        goto ERR_CREATE_DESC_ID;
    }
    dbg_print("\t serail_id:%02x%02x%02x%02x\n", buf[0], buf[1], buf[2], buf[3]);
    
    /* Read the CSWrite3.0 Active time */   
    err = cswrite_cfg_get_active_day(&id->day);    
    if (err) {
        err_print("swrite_get_active_date() failed, err=0x%x\n", err);        
        goto ERR_CREATE_DESC_ID;
    }
    dbg_print("\t Active Time(YY//MM//DD):%02d/%02d/%02d\n", id->day.year, id->day.month, id->day.date);
    
    /* Read the CSWrite3.0 Total program count in life cycles */
    num_ok = cswrite_log_get_num_ok();    

    dbg_print("\t Total Program OK num:%d\n", num_ok);
    
    num_failed = cswrite_log_get_num_failed();    

    dbg_print("\t Total Program Failed num:%d\n", num_failed);

    id->count = num_ok + num_failed;
    
    return ERR_OK;
    
ERR_CREATE_DESC_ID:
    if (node_id) {
        free(node_id);
    }
    return err;
}


int32_t file_dev_upgrade(void)
{
    int32_t         ret;
    uint32_t        len;
    struct tlv_file dev;
    struct tlv_node_chip* chip;

    tlv_file_init(&dev);
    
    dev.head.file_id = TLV_FILE_ID_DEV;
    ret = cswrite_cfg_get_id(&dev.list_id);
    if (ret) {
        err_print("create_desc_id(%p) failed\n", &dev.list_id);
    }

    ret = cswrite_cfg_get_version(&dev.list_version);
    if (ret) {
        err_print("create_desc_id(%p) failed\n", &dev.list_id);
    }
    
    ret = cswrite_cfg_get_chip(&dev.list_chip);
    if (ret) {
        err_print("create_desc_id(%p) failed\n", &dev.list_id);
    }

    return tlv_file_pack(&dev);
}

