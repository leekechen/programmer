#include <rl_fs.h>
#include "files.h"
#include "error.h"
#include "common.h"
#include "file_firmware.h"
#include "crc32_soft.h"
#include "flash_map.h"
#include "tlv.h"
#include "key.h"
#include "cswrite_cfg.h"

#define MODULE_DEBUG                1
#define MODULE_ERROR                1

#include "dbg_print.h"

#define FILE_FIRMWARE_HELPER        1


#define FILE_FIRMWARE_UPADATED      "file_firmware.tmp"

#define FILE_FIRMWARE_BACKUP        "file_firmware.bak"

int32_t file_firmware_backup(void)
{
    int32_t ret;
    fsFileInfo  info;

    info.fileID = 0;
    ret = ffind(FILE_FIRMWARE_UPADATED, &info);
    if (ret) {
        err_print("ffind(%s,%p) failed, ret=%d\n", FILE_FIRMWARE_UPADATED, &info, ret);
        return 0;
    }

    dbg_print("file_firmware_backup()\n");

    ret = ffind(FILE_FIRMWARE_BACKUP, &info);
    if (ret == fsOK) {
        ret = fdelete(FILE_FIRMWARE_BACKUP, NULL);
        if (ret) {
            err_print("fdelte(%s,NULL) failed, ret=%d\n", 
                    FILE_FIRMWARE_BACKUP, ret); 
            return 0;        
        }
    }
    
    ret = frename(FILE_FIRMWARE_UPADATED, FILE_FIRMWARE_BACKUP);
    if (ret) {
        err_print("frename(%s,%s) failed, ret=%d\n", 
                    FILE_FIRMWARE_UPADATED, FILE_FIRMWARE_BACKUP, ret);          
    }

    //ret = file_upgrade(TLV_FILE_ID_DEV);

    return ret;
}


int32_t file_firmware_init(void)
{
    return file_firmware_backup();
}

void* file_firmware_open(const char* mode)
{
    return fopen(FILE_FIRMWARE, mode);    
}

int32_t file_firmware_read(uint8_t* buf, size_t size, void* fp)
{
    return fread(buf, 1, size, fp);
}

int32_t file_firmware_write(const uint8_t* data, size_t size, void* fp)
{
    return fwrite(data, 1, size, fp);
}

int32_t file_firmware_close(void* fp)
{
    return fclose(fp);
}

int32_t file_firmware_upgrade(void)
{
    int32_t ret;
    struct key_event evt;

    evt.code  = KEY_CODE_FIRMWARE;
    evt.state = KEY_STATE_PRESS;

    ret = key_set(&evt);
    if (ret) {
        err_print("key_set(%p) failed, ret=%d\n", &evt, ret);
        return ret;
    }
    
    return ERR_OK;
}

#ifdef BOOTLOADER

int32_t file_firmware_load(struct tlv_file* tlv)
{
    int32_t ret;

    ret = file_get_size(TLV_FILE_ID_FIRMWARE);
    if (ret <= 0) {
        err_print("file_get_size(%d) ret=%d\n", TLV_FILE_ID_FIRMWARE, ret);
        return -1;
    }

    ret = file_verify(TLV_FILE_ID_FIRMWARE);
    if (!ret) {
        err_print("file_verify(%d) ret=%d\n", TLV_FILE_ID_FIRMWARE, ret);
        return -1;
    }

    tlv_file_init(tlv);

    ret = tlv_file_unpack(TLV_FILE_ID_FIRMWARE, tlv);
    if (ret) {
        err_print("tlv_file_unpack(%d) ret=%d\n", TLV_FILE_ID_FIRMWARE, ret);
        return -1;
    }
    
    return ERR_OK;
}



int32_t file_firmware_updated(void)
{
    int32_t ret;
    fsFileInfo  info;
    
    dbg_print("file_firmware_updated()\n");

    info.fileID = 0;
    ret = ffind(FILE_FIRMWARE_UPADATED, &info);
    if (ret == fsOK) {
        ret = fdelete(FILE_FIRMWARE_UPADATED, NULL);
        if (ret) {
            err_print("fdelte(%s,NULL) failed, ret=%d\n", 
                    FILE_FIRMWARE_UPADATED, ret); 
            return 0;        
        }
    } 
    
    ret = frename(FILE_FIRMWARE, FILE_FIRMWARE_UPADATED);
    if (ret) {
        err_print("frename(%s,%s) failed, ret=%d\n", 
                    FILE_FIRMWARE_BACKUP, FILE_FIRMWARE, ret);          
    }

    return ret;
}

int32_t file_firmware_borken(void)
{    
    int32_t ret;
    fsFileInfo  info;
    
    dbg_print("file_firmware_borken()\n");

    info.fileID = 0;
    ret = ffind(FILE_FIRMWARE_UPADATED, &info);
    if (ret == fsOK) {
        return 1;
    } 
    return 0;
}

int32_t copy(const char* dst, const char* src)
{
    int32_t ret;
    int32_t len_r;
    int32_t len_w;
    uint8_t buf[256];
    FILE*   fp_bak;
    FILE*   fp_new;
    
    dbg_print("copy()\n");

    fp_bak = fopen(src, "r");
    if (fp_bak == NULL) {
        err_print("fopen(%s,\"r\") failed, ret=%d\n", 
                    src, ret);
        return -1;
    }
    
    fp_new = fopen(dst, "w");
    if (fp_new == NULL) {
        err_print("fopen(%s,\"w\") failed, ret=%d\n", 
                    dst, ret);
        return -1;
    }  

    for (len_r = sizeof(buf); len_r == sizeof(buf);) {
        len_r = fread(buf, 1, sizeof(buf), fp_bak);

        len_w = fwrite(buf, 1, len_r, fp_new);
        if (len_w != len_r) {
            err_print("fwrite(%p, 1, %d, %p) failed\n", buf, len_r, fp_new);
            fclose(fp_new);
            fclose(fp_bak);
            return -1;
        }
    }              
    fclose(fp_new);
    fclose(fp_bak); 

    return ERR_OK;
}

struct tlv_code* file_firmware_get_code(struct tlv_file* tlv)
{
    struct list_head* pos;
    struct tlv_node_code* node_code;
    

    list_for_each (pos, &tlv->list_code) {
        node_code = list_entry(pos, struct tlv_node_code, node);
        return &node_code->desc.body;
    }
    
    return NULL;
}

int32_t file_firmware_recovery(void)
{   
    int32_t ret;

    ret = fdelete(FILE_FIRMWARE, NULL);
    if (ret) {
        err_print("fdelete(%s, NULL) failed\n", FILE_FIRMWARE);
    }
    ret = copy(FILE_FIRMWARE, FILE_FIRMWARE_BACKUP);
    return ret;
}

#endif

