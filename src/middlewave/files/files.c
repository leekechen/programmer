#include <string.h>

#include <rl_fs.h>
//#include "Driver_Flash.h"
#include "files.h"
#include "file_dev.h"
#include "file_hex.h"
#include "file_cfg.h"
#include "file_ic.h"
#include "file_firmware.h"
#include "error.h"

#include "crc32_soft.h"

#define MODULE_DEBUG                            0
#define MODULE_ERROR                            1

#include "dbg_print.h"

#define FILE_SYSTEM_DRIVER                       "F0:"

struct file_operation {
    int32_t     (*init)     (void);
    void*       (*open)     (const char* mode);
    int32_t     (*read)     (uint8_t* buf, size_t size, void* fp);
    int32_t     (*write)    (const uint8_t* data, size_t size, void* fp);
    int32_t     (*seek)     (void* fp, int32_t offset, int32_t whence);
    uint32_t    (*get_size) (void);
    uint32_t    (*get_pos)  (void);
    int32_t     (*close)    (void* fp);
    int32_t     (*verify)   (void);
    int32_t     (*upgrade)  (void);
} ;

struct file_ext {
    void*                   fp;
    const struct file_operation*  ops;
} ;

static const char* file_name[] = {
    FILE_DEV,      
    FILE_FIRMWARE, 
    FILE_HEX,      
    FILE_IC,       
    FILE_LOG,      
    FILE_CFG,          
} ;


static const struct file_operation file_firmware_ops = {
    .open       = file_firmware_open,
    .read       = file_firmware_read,
    .write      = file_firmware_write,
    .close      = file_firmware_close,
    .upgrade    = file_firmware_upgrade,
} ;

static struct file_ext file_firmware = {
    .ops = &file_firmware_ops,
};

#ifndef BOOTLOADER

static struct file_operation file_dev_ops = {
    .init     = file_dev_init,
    .open     = file_dev_open,
    .read     = file_dev_read,
    .write    = file_dev_write,
    .close    = file_dev_close,
    .upgrade  = file_dev_upgrade,
};

static struct file_ext file_dev = {
    .ops = &file_dev_ops,
};



static struct file_operation file_hex_ops = {
    .init       = file_hex_init,
    .open       = file_hex_open,
    .read       = file_hex_read,
    .write      = file_hex_write,
    .close      = file_hex_close,
    .upgrade    = file_hex_upgrade,
};

static struct file_ext file_hex = {
    .ops = &file_hex_ops,
};

static struct file_operation file_ic_ops= {
    .open       = file_ic_open,
    .read       = file_ic_read,
    //.write      = file_ic_write,
    .close      = file_ic_close,
    .get_size   = file_ic_get_size,
};

static struct file_ext file_ic = {
    .ops = &file_ic_ops,
};

static struct file_operation file_log_ops= {
};

static struct file_ext file_log = {
    .ops = &file_log_ops,
};

static struct file_operation file_cfg_ops= {
    .open       = file_cfg_open, 
    .read       = file_cfg_read,
    .write      = file_cfg_write,
    .close      = file_cfg_close,
    .upgrade    = file_cfg_upgrade,
};

static struct file_ext file_cfg = {
    .ops = &file_cfg_ops,
};





static struct file_ext* file_list[] = {
    &file_dev,
    &file_firmware,
    &file_hex,
    &file_ic,
    &file_log,
    &file_cfg,
};

#else

static struct file_ext* file_list[] = {
    NULL,
    &file_firmware,
    NULL
};
#endif

#define FILE_OPS_NUM             (sizeof(file_list)/4)


static void dot_format (uint64_t val, char *sp) 
{
    if (val >= (uint64_t)1e12) {
        sp += sprintf (sp,"%d.",(uint32_t)(val/(uint64_t)1e12));
        val %= (uint64_t)1e12;
        sp += sprintf (sp,"%03d.",(uint32_t)(val/(uint64_t)1e9));
        val %= (uint64_t)1e9;
        sp += sprintf (sp,"%03d.",(uint32_t)(val/(uint64_t)1e6));
        val %= (uint64_t)1e6;
        sprintf (sp,"%03d.%03d",(uint32_t)(val/1000),(uint32_t)(val%1000));
        return;
    }
    if (val >= (uint64_t)1e9) {
        sp += sprintf (sp,"%d.",(uint32_t)(val/(uint64_t)1e9));
        val %= (uint64_t)1e9;
        sp += sprintf (sp,"%03d.",(uint32_t)(val/(uint64_t)1e6));
        val %= (uint64_t)1e6;
        sprintf (sp,"%03d.%03d",(uint32_t)(val/1000),(uint32_t)(val%1000));
        return;
    }
    if (val >= (uint64_t)1e6) {
        sp += sprintf (sp,"%d.",(uint32_t)(val/(uint64_t)1e6));
        val %= (uint64_t)1e6;
        sprintf (sp,"%03d.%03d",(uint32_t)(val/1000),(uint32_t)(val%1000));
        return;
    }
    if (val >= 1000) {
        sprintf (sp,"%d.%03d",(uint32_t)(val/1000),(uint32_t)(val%1000));
        return;
    }
    sprintf (sp,"%d",(uint32_t)(val));
}

 int32_t file_system_dir(void) 
{
    uint64_t fsize;
    uint32_t files;
    uint32_t dirs;
    uint32_t i;
    char temp[32];
    char *mask;
    char ch;
    fsFileInfo info;

    mask = "*.*";     

    dbg_print("  File System Directory...\n");
    files = 0;
    dirs  = 0;
    fsize = 0;
    info.fileID  = 0;
    while (ffind (mask,&info) == fsOK) {
        if (info.attrib & FS_FAT_ATTR_DIRECTORY) {
            i = 0;
            while (strlen((const char *)info.name+i) > 41) {
                ch = info.name[i+41];
                info.name[i+41] = 0;
                dbg_print("\n%-41s", &info.name[i]);
                info.name[i+41] = ch;
                i += 41;
            }
            dbg_print("\t %-41s    <DIR>       ", &info.name[i]);
            dbg_print("  %02d-%02d-%04d  %02d:%02d\n",
            info.time.day, info.time.mon, info.time.year,
            info.time.hr, info.time.min);
            dirs++;
        } else {
            dot_format (info.size, &temp[0]);
            i = 0;
            while (strlen((const char *)info.name+i) > 41) {
                ch = info.name[i+41];
                info.name[i+41] = 0;
                dbg_print("\t%-41s", &info.name[i]);
                info.name[i+41] = ch;
                i += 41;
            }
            dbg_print("\t %-41s %14s ", &info.name[i], temp);
            dbg_print("  %02d-%02d-%04d  %02d:%02d\n",
                        info.time.day, info.time.mon, info.time.year,
                        info.time.hr, info.time.min);
            fsize += info.size;
            files++;
        }
    }
    if (info.fileID == 0) {
        dbg_print ("\t No files...\n");
    } else {
        dot_format (fsize, &temp[0]);
        dbg_print("              %9d File(s)    %21s bytes\n", files, temp);
    }
    dot_format (ffree(mask), &temp[0]);
    if (dirs) {
        dbg_print("\n              %9d Dir(s)     %21s bytes free.\n", dirs, temp);
    } else {
        dbg_print("%56s bytes free.\n", temp);
    }
    return ERR_OK;
}

static int32_t file_system_format(void)
{
    int32_t ret;
    
    dbg_print("  fformat()... ");
    ret = fformat(FILE_SYSTEM_DRIVER, NULL);
    if (ret){ 
        err_print("fformat(%s, NULL) failed!,ret=%d\n", FILE_SYSTEM_DRIVER, ret);        
        return -1;
    } 
    dbg_print("ok!\n");
    
    return ERR_OK;
}

static int32_t file_system_defrag(void)
{
    int32_t ret;

    dbg_print("  fanalyse()...");
    ret = fanalyse(FILE_SYSTEM_DRIVER);
    dbg_print("fragment =%d\n", ret);
    if (ret > 50) {
        dbg_print("  fdefrag()...");
        ret = fdefrag (FILE_SYSTEM_DRIVER);
        if (ret){ 
            err_print("fdefrag(%s) failed!,ret=%d\n", FILE_SYSTEM_DRIVER, ret);            
            return -1;
        } 
        dbg_print("ok!\n");
    }
    
    return ERR_OK;
}

static int32_t file_system_init(void)
{
    int32_t ret;

    dbg_print("fileSystemInit..\n  finit()... ");
    ret = finit (FILE_SYSTEM_DRIVER);
    if (ret) {
        err_print("finit (%s) failed!,ret=%d\n", FILE_SYSTEM_DRIVER, ret);
        goto FILE_SYSTEM_INIT_ERROR;
    }
    dbg_print("ok!\n  fmount()... ");
    ret = fmount (FILE_SYSTEM_DRIVER);
    if (ret) { 
        err_print("fmount (%s) failed!,ret=%d\n", FILE_SYSTEM_DRIVER, ret);
        goto FILE_SYSTEM_INIT_ERROR;
    }
    
    dbg_print("ok!\n  fcheck()...");
    ret = fcheck(FILE_SYSTEM_DRIVER);
    if (ret) {
        err_print("fcheck(%s) failed!,ret=%d\n", FILE_SYSTEM_DRIVER, ret);
        goto FILE_SYSTEM_INIT_ERROR;  
    }
    dbg_print("ok!\n");

//    file_system_format();

    ret = file_system_defrag();
    if (ret) {
        err_print("file_system_defrag() failed!,ret=%d\n", ret);
        goto FILE_SYSTEM_INIT_ERROR;  
    }

    file_system_dir();

    return ret;

FILE_SYSTEM_INIT_ERROR:
    return file_system_format();
}

static struct file_ext* get_file_ext_by_id(int32_t id)
{
    if (id <= 0 || id > TLV_FILE_ID_MAX) {
        err_print("Invalid argument, id=%d\n", id);
        return NULL;
    }    
    return file_list[--id];
}

static struct file_ext* get_file_ext_by_fp(void* fp)
{
    int32_t i;
    
    if (fp == NULL) {
        err_print("Invalid argument, fp=NULL\n");
        return NULL;
    }

    for (i = 0; i < FILE_OPS_NUM; i++) {
        if ((void*)file_list[i] == fp) {
            return file_list[i];
        }
    }    
    
    return NULL;
}

static const char* get_file_name(int32_t id)
{
    if (id <= 0 || id > TLV_FILE_ID_MAX) {
        err_print("Invalid argument, id=%d\n", id);
        return NULL;
    }    
    
    return file_name[--id];
}

static uint32_t default_get_size(int32_t id)
{
    uint32_t    ret;
    const char* name;
    fsFileInfo  info;

    info.fileID = 0;
    name = get_file_name(id);
    if (name == NULL) {
        err_print("Invalid argument, id=%d\n", id);
        return 0;
    }
    
    dbg_print("[File] get %s size\n", name);

    ret = ffind(name, &info);
    if (ret) {
        err_print("ffind(%s,%p) failed, ret=%d\n", name, &info, ret);
        return 0;
    }
    return info.size;

}

int32_t file_init(void)
{
    int32_t i;
    int32_t err = ERR_OK;

    file_system_init();

    for (i = 0; i < FILE_OPS_NUM; i++) {
        if (file_list[i]) {
            if (file_list[i]->ops->init) {
                file_list[i]->ops->init();
            }
        }
    }
    
    return err;
}

int32_t file_reset(void)
{
    return file_system_format();
}

void* file_open(int32_t id, const char* mode)
{    
    struct file_ext*  file;

    if (mode == NULL) {
        err_print("Invalid argument,mode==NULL\n");
        return NULL;
    }

    file = get_file_ext_by_id(id);
    if (file == NULL) {
        err_print("get_file_ext(%d) failed\n", id);
        return NULL;
    }

    if (file->ops->open) {
        file->fp = file->ops->open(mode);
        if (file->fp == NULL) {
            return NULL;
        }
    }
    
    return file;
}

void* file_code_open(int32_t id, struct tlv_code* code)
{
    int32_t ret;
    struct file_ext*  file;
    const char* name;

    file = get_file_ext_by_id(id);
    if (file == NULL) {
        err_print("get_file_ext(%d) failed\n", id);
        return NULL;
    }    

    if (code == NULL) {        
        err_print("Find Code Desciptor failed\n");
        return NULL;
    }

    name = get_file_name(id);
    if (name == NULL) {        
        err_print("get_file_name(%d) failed\n", id);
        return NULL;
    }

    file->fp = fopen(name, "r");
    if (file->fp == NULL) {
        err_print("fopen(%s,\"r\") failed\n", name);
        return NULL;
    }

    ret = fseek(file->fp, code->start, SEEK_SET);
    if (ret) {
        err_print("fseek(%p,%d,SEEK_SET) failed\n", file->fp, code->start);
        return NULL;
    }    
    
    return file;
}

int32_t file_read(uint8_t* buf, size_t size, void* fp)
{
    size_t            ret = 0;
    struct file_ext*  file;

    if (buf == NULL || size == 0 || fp == NULL) {        
        err_print("Invalid argument,buf=%p,size=%d,fp=%p\n", buf, size, fp);
        return ret;
    }

    file = get_file_ext_by_fp(fp);
    if (file == NULL) {
        err_print("get_file_ext_by_fp(0x%p) failed\n", fp);
        return ret;
    }

    if (file->ops->read) {
        ret = file->ops->read(buf, size, file->fp);
    } 
    
    return ret;
}

int32_t file_write(const uint8_t* data, size_t size, void* fp)
{
    int32_t          ret = 0;
    struct file_ext* file;

    if (data == NULL || size == 0 || fp == NULL) {        
        err_print("Invalid argument,buf=%p,size=%d,fp=%p\n", data, size, fp);
        return ret;
    }

    file = get_file_ext_by_fp(fp);
    if (file == NULL) {
        err_print("get_file_ext_by_fp(0x%p) failed\n", fp);
        return ret;
    }
    
    if (file->ops->write) {    
        ret = file->ops->write(data, size, file->fp);
    }
    
    return ret;
}

int32_t file_seek(void* fp, int32_t offset, int32_t whence)
{
    int32_t          ret = 0;
    struct file_ext* file;

    file = get_file_ext_by_fp(fp);
    if (file == NULL) {
        err_print("get_file_ext_by_fp(0x%p) failed\n", fp);
        return ret;
    }
    
    if (file->ops->seek) {    
        ret = file->ops->seek(file->fp, offset, whence);
    } else {
        ret = fseek(file->fp, offset, whence);
    }
    
    return ret;
}

int32_t file_get_pos(void* fp)
{
    int32_t ret = -1;
    struct file_ext* file;

    file = get_file_ext_by_fp(fp);
    if (file == NULL) {
        err_print("get_file_ext_by_fp(0x%p) failed\n", fp);
        return ret;
    }

    if (file->ops->get_pos) {
        ret = file->ops->get_pos();
    } else {
        fpos_t  pos;        
        ret = fgetpos(file->fp, &pos);
        if (ret) {
            err_print("fgetpos(0x%p, %p) failed\n", fp, &pos);
            return 0;
        }
        ret = (int32_t)pos.__pos;
    }
    return ret;
}

int32_t file_close(void* fp)
{   
    int32_t          ret = 0; 
    struct file_ext* file;

    file = get_file_ext_by_fp(fp);
    if (file == NULL) {
        err_print("get_file_ext_by_fp(0x%p) failed\n", fp);
        return NULL;
    }

    if (file->ops->close) {
        ret = file->ops->close(file->fp);
        file->fp = NULL;
    }    
    
    return ret;
}

int32_t file_get_size(int32_t id)
{
    int32_t ret;
    struct file_ext*  file;

    file = get_file_ext_by_id(id);
    if (file == NULL) {
        err_print("get_file_ext(%d) failed\n", id);
        return 0;
    }

    if (file->ops->get_size) {
        ret = file->ops->get_size();
    } else {
        ret = default_get_size(id);
    }  
    
    return ret;
}

int32_t file_verify(int32_t id)
{
    int32_t ret;
    struct file_ext*  file;

    file = get_file_ext_by_id(id);
    if (file == NULL) {
        err_print("get_file_ext(%d) failed\n", id);
        return NULL;
    }

    if (file->ops->get_size) {
        ret = file->ops->verify();
    } else {
        ret = tlv_file_verify(id);
    }    
    return ret;
}

int32_t file_upgrade(int32_t id)
{
    int32_t ret;
    struct file_ext*  file;

    file = get_file_ext_by_id(id);
    if (file == NULL) {
        err_print("get_file_ext(%d) failed\n", id);
        return NULL;
    }

    if (file->ops->upgrade) {
        ret = file->ops->upgrade();
    } 
    
    return ret;
}




