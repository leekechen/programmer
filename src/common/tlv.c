#include <stdlib.h>
#include "tlv.h"
#include "error.h"
#include "files.h"
#include "crc32_soft.h"

#define MODULE_DEBUG       1
#define MODULE_ERROR       1

#include "dbg_print.h"

#define DBG_PACK           1
#define DBG_UNPACK         1
#define DBG_FILE_DUMP      0


#if (DBG_PACK)
#define dbg_pack_print(...)     dbg_print(__VA_ARGS__)
#else
#define dbg_pack_print(...)  
#endif

#if (DBG_UNPACK)
#define dbg_unpack_print(...)     dbg_print(__VA_ARGS__)
#else
#define dbg_unpack_print(...)    
#endif

#if (DBG_FILE_DUMP)
#define dbg_file_dump(...)        dbg_dump(__VA_ARGS__)
#else
#define dbg_file_dump(...)  
#endif


#define MAX_BUF_SIZE                        256



static int32_t tlv_unpack_desc_crypto(uint32_t tag_length, struct list_head* list_desc, void* fp)
{
    int32_t                 err;
    int32_t                 len;
    size_t                  size;
    uint8_t*                buf;
    struct tlv_desc_crypto* desc;
    struct tlv_node_crypto* node_crypto;

    node_crypto = tlv_desc_new(sizeof(struct tlv_node_crypto)); 
    if (node_crypto == NULL) {
        err_print("tlv_desc_new(%p,%d) failed!\n", list_desc, sizeof(struct tlv_node_crypto));
        err = ERR_FILE | ERR_SYS_NOT_ENOUGH_MEMORY;
        goto ERR_UNPACK_CRYPTO;
    }
    desc = &node_crypto->desc;

    size = sizeof(desc->body.algorithm);
    buf  = (uint8_t*)&desc->body.algorithm;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_CRYPTO;
    }
    tag_length -= size;
    dbg_unpack_print("\t\t algorithm=%d\n", desc->body.algorithm);

    if (tag_length <= 0) {
        err_print("tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_CRYPTO;
    }

    size = sizeof(desc->body.key);
    buf  = (uint8_t*)&desc->body.key;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_CRYPTO;
    }
    dbg_unpack_print("\t\t key=%d\n", desc->body.key);

    tlv_desc_add(&node_crypto->node, list_desc);

    return ERR_OK;

ERR_UNPACK_CRYPTO:
    if (node_crypto) {
        free(node_crypto);
    }

    return err;
}

static int32_t tlv_unpack_desc_version(uint32_t tag_length, struct list_head* list_desc, void* fp)
{
    int32_t  err;
    int32_t  len;
    size_t   size;
    uint8_t* buf;
    struct tlv_desc_version* desc;
    struct tlv_node_version* node_version;

    node_version = tlv_desc_new(sizeof(struct tlv_node_version));
    if (node_version == NULL) {
        err_print("tlv_desc_new(%p,%d) failed!\n", list_desc, sizeof(struct tlv_node_version));
        err = ERR_FILE | ERR_SYS_NOT_ENOUGH_MEMORY;
        goto ERR_UNPACK_VERSION;
    }
    desc = &node_version->desc;

    size = sizeof(desc->body.type);
    buf  = (uint8_t*)&desc->body.type;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_VERSION;
    }
    dbg_unpack_print("\t\t type=%d\n", desc->body.type);

    if (desc->body.type > TLV_DESC_VERSION_TYPE_PC || desc->body.type <= 0) {
        err_print("Invalid Version Descriptor type=%d\n", desc->body.type);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_VERSION;
    }

    if (tag_length <= 0) {
        err_print("tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_VERSION;
    }

    size = sizeof(desc->body.value);
    buf  = (uint8_t*)&desc->body.value;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_VERSION;
    }
    dbg_unpack_print("\t\t version=%x\n", desc->body.value);

    list_add(&node_version->node, list_desc);

    return ERR_OK;

ERR_UNPACK_VERSION:
    if (node_version) {
        free(node_version);
    }

    return err;
}

static int32_t tlv_unpack_desc_code(uint32_t tag_length,  struct list_head* list_desc, void* fp)
{
    int32_t  err;
    int32_t  ret;
    size_t   size;
    uint8_t* buf;
    uint32_t addr;
    struct tlv_desc_code* desc;
    struct tlv_node_code* node_code;

    node_code = calloc(1, sizeof(struct tlv_node_code));
    if (node_code == NULL) {
        err_print("Not enough memory!,size=%d\n", sizeof(struct tlv_node_code));
        err = ERR_FILE | ERR_SYS_NOT_ENOUGH_MEMORY;
        goto ERR_UNPACK_CODE;
    }
    desc = &node_code->desc;

    size = sizeof(desc->body.addr);
    buf  = (uint8_t*)&desc->body.addr;
    ret  = file_read(buf, size, fp);
    if (ret != size) {
        err_print("file_read() failed, ret=%d!\n", ret);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_CODE;
    }
    addr       += size;
    tag_length -= size;
    desc->body.addr = __REV32(desc->body.addr);
    dbg_unpack_print("\t\t addr=0x%x\n", desc->body.addr);

    if (tag_length <= 0) {
        err_print("tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_CODE;
    }

    /* 4: address, 5: 1 byte checksum_type, 4 bytes checksum */
    size = tag_length - 5;
    ret = file_get_pos(fp);
    if (ret < 0) {
        err_print("file_get_pos(%p) ret=%d!\n", fp, ret);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_CODE;        
    }
    
    desc->body.start = ret;
    desc->body.end   = ret + size;
    addr       += size;
    tag_length -= size;
    dbg_unpack_print("\t\t hex start=0x%x, end=0x%x\n", desc->body.start, desc->body.end);

    if (tag_length <= 0) {
        err_print("Invalid tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_CODE;
    }

    ret = file_seek(fp, size, SEEK_CUR);
    if (ret) {
        err_print("file_seek(%p,%d,SEEK_CUR) failed),ret=%d\n", fp, size, ret);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_CODE;
    }

    size = sizeof(desc->body.checksum_type);
    buf  = (uint8_t*)&desc->body.checksum_type;
    ret  = file_read(buf, size, fp);
    if (ret != size) {
        err_print("Error, file_read() failed, ret=%d!\n", ret);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_CODE;
    }
    addr        += size;
    tag_length  -= size;
    dbg_unpack_print("\t\t checksum_type=%d\n", desc->body.checksum_type);

    if (tag_length <= 0) {
        err_print("Error, tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_CODE;
    }

    size = sizeof(desc->body.checksum);
    buf  = (uint8_t*)&desc->body.checksum;
    ret  = file_read(buf, size, fp);
    if (ret != size) {
        err_print("file_read() failed, ret=%d!\n", ret);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_CODE;
    }
    desc->body.checksum = __REV32(desc->body.checksum);
    dbg_unpack_print("\t\t checksum=0x%x\n", desc->body.checksum);

    list_add(&node_code->node, list_desc);

    return ERR_OK;

ERR_UNPACK_CODE:
    if (node_code) {
        free(node_code);
    }

    return err;
}

static int32_t tlv_unpack_desc_chip(size_t tag_length, struct list_head* list_desc, void* fp)
{
    int32_t  err;
    int32_t  len;
    int32_t  size;
    uint8_t* buf;
    //struct tlv_chip*      chip;
    struct tlv_desc_chip* desc;
    struct tlv_node_chip* node_chip;

    node_chip = calloc(1, sizeof(struct tlv_node_chip));
    if (node_chip == NULL) {
        err_print("Not enough memory!\n");
        err = ERR_FILE | ERR_SYS_NOT_ENOUGH_MEMORY;
        goto ERR_UNPACK_CHIP;
    }
    desc = &node_chip->desc;

    /* Got the name_length */
    size = sizeof(desc->body.name_length);
    buf  = &desc->body.name_length;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE;
        goto ERR_UNPACK_CHIP;
    }
    tag_length  -= size;
    dbg_unpack_print("\t\t name_length=0x%x\n", desc->body.name_length);

    if (tag_length <= 0) {
        err_print("tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_CHIP;
    }

    if (desc->body.name_length > 0) {
        /* Got the name string, add 1 byte for '\0' */
        desc->body.name = calloc(1, desc->body.name_length + 1);
        if (desc->body.name == NULL) {
            err_print("calloc(1,%d) failed\n", desc->body.name_length);
            err = ERR_SYS_NOT_ENOUGH_MEMORY;
            goto ERR_UNPACK_CHIP;
        }

        size = desc->body.name_length;
        buf  = desc->body.name;
        len  = file_read(buf, size, fp);
        if (len != size) {
            err_print("file_read() failed, ret=%d!\n", len);
            err = ERR_FILE;
            goto ERR_UNPACK_CHIP;
        }
        tag_length -= size;
        dbg_unpack_print("\t\t name=%s\n", desc->body.name);
    }

    if (tag_length <= 0) {
        return ERR_OK;
    }

    /* Got the id_length */
    size = sizeof(desc->body.id_length);
    buf  = &desc->body.id_length;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE;
        goto ERR_UNPACK_CHIP;
    }
    tag_length -= size;
    dbg_unpack_print("\t\t id_length=0x%x\n", desc->body.id_length);

    if (desc->body.id_length > 0) {
        /* Got the ID */
        desc->body.id = calloc(1, desc->body.id_length);
        if (desc->body.id == NULL) {
            err = ERR_SYS_NOT_ENOUGH_MEMORY;
            goto ERR_UNPACK_CHIP;
        }

        size = desc->body.id_length;
        buf  = desc->body.id;
        len  = file_read(buf, size, fp);
        if (len != size) {
            err_print("file_read() failed, ret=%d!\n", len);
            err = ERR_FILE;
            goto ERR_UNPACK_CHIP;
        }
        tag_length  -= size;
        dbg_dump("\t\t id", buf, size);
    }

    if (tag_length <= 0) {
        return ERR_OK;
    }

    /* Got the type_length */
    size = sizeof(desc->body.type_length);
    buf  = &desc->body.type_length;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE;
        goto ERR_UNPACK_CHIP;
    }
    tag_length  -= size;
    dbg_unpack_print("\t\t type_length=0x%x\n", desc->body.type_length);

    if (desc->body.type_length > 0) {
        desc->body.type  = calloc(1, desc->body.type_length);
        if (desc->body.type == NULL) {
            err = ERR_SYS_NOT_ENOUGH_MEMORY;
            goto ERR_UNPACK_CHIP;
        }

        size = desc->body.type_length;
        buf  = desc->body.type;
        len  = file_read(buf, size, fp);
        if (len != size) {
            err_print("file_read() failed, ret=%d!\n", len);
            err = ERR_FILE;
            goto ERR_UNPACK_CHIP;
        }
        tag_length  -= size;
        dbg_dump("\t\t type", buf, size);
    }

    list_add(&node_chip->node, list_desc);

    return ERR_OK;

ERR_UNPACK_CHIP:
    if (desc->body.name) {
        free(desc->body.name);
    }

    if (desc->body.id) {
        free(desc->body.id);
    }

    if (desc->body.type) {
        free(desc->body.type);
    }

    if (node_chip) {
        free(node_chip);
    }

    return err;

}

static int32_t tlv_unpack_desc_voltage(size_t tag_length, struct list_head* list_desc, void* fp)
{
    int32_t  err;
    int32_t  len;
    int32_t  size;
    uint8_t* buf;
    struct tlv_desc_voltage* desc;
    struct tlv_node_voltage* node_voltage;

    node_voltage = calloc(1, sizeof(struct tlv_node_voltage));
    if (node_voltage == NULL) {
        err_print("Not enough memory!\n");
        err = ERR_FILE | ERR_SYS_NOT_ENOUGH_MEMORY;
        goto ERR_UNPACK_VOLTAGE;
    }
    desc = &node_voltage->desc;

    size = sizeof(desc->body.type);
    buf  = &desc->body.type;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_VOLTAGE;
    }
    tag_length -= size;
    dbg_unpack_print("\t\t type=0x%x\n", desc->body.type);

    if (desc->body.type > TLV_DESC_VOLTAGE_TYPE_VPP || desc->body.type <= 0) {
        err_print("Invalid Voltage type! type=%d\n", desc->body.type);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_VOLTAGE;
    }

    if (tag_length <= 0) {
        err_print("Invalid tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_VOLTAGE;
    }

    size = sizeof(desc->body.value);
    buf  = (uint8_t*)&desc->body.value;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_VOLTAGE;
    }
    desc->body.value = __REV32(desc->body.value);
    tag_length -= size;
    dbg_unpack_print("\t\t value=%d (in mV)\n", desc->body.value);

    if (tag_length <= 0) {
        err_print("Invalid tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_VOLTAGE;
    }

    size = sizeof(desc->body.offset);
    buf  = (uint8_t*)&desc->body.offset;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_VOLTAGE;
    }
    desc->body.offset = __REV32(desc->body.offset);
    dbg_unpack_print("\t\t offset=%d (in mV)\n", desc->body.offset);

    list_add(&node_voltage->node, list_desc);

    return ERR_OK;

ERR_UNPACK_VOLTAGE:
    if (node_voltage) {
        free(node_voltage);
    }

    return err;
}

static int32_t tlv_unpack_desc_memory(size_t tag_length, struct list_head* list_desc, void* fp)
{
    int32_t  err;
    int32_t  len;
    int32_t  size;
    uint8_t* buf;
    struct tlv_desc_memory* desc;
    struct tlv_node_memory* node_memory;

    node_memory = calloc(1, sizeof(struct tlv_node_memory));
    if (node_memory == NULL) {
        err_print("Not enough memory!\n");
        err = ERR_FILE | ERR_SYS_NOT_ENOUGH_MEMORY;
        goto ERR_UNPACK_MEMORY;
    }
    desc = &node_memory->desc;

    size = sizeof(desc->body.type);
    buf  = &desc->body.type;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_MEMORY;
    }
    tag_length -= size;
    dbg_unpack_print("\t\t type=0x%x\n", desc->body.type);

    if ((desc->body.type > TLV_DESC_MEMORY_TYPE_EEPROM) || (desc->body.type <= 0)) {
        err_print("Invalid memory type=%d\n", desc->body.type);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_MEMORY;
    }

    if (tag_length <= 0) {
        err_print("Invalid tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_MEMORY;
    }

    buf  = &desc->body.width;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
    }
    tag_length -= size;
    dbg_unpack_print("\t\t width=%d (in bit)\n", desc->body.width);

    if (tag_length <= 0) {
        err_print("Invalid tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_MEMORY;
    }

    size = sizeof(desc->body.addr);
    buf  = (uint8_t*)&desc->body.addr;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("Invalidfile_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_MEMORY;
    }
    desc->body.addr = __REV32(desc->body.addr);
    tag_length -= size;
    dbg_unpack_print("\t\t address=%d (word addressing)\n", desc->body.addr);

    if (tag_length <= 0) {
        err_print("Error, tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_MEMORY;
    }

    buf  = (uint8_t*)&desc->body.size;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_MEMORY;
    }
    desc->body.size = __REV32(desc->body.size);
    dbg_unpack_print("\t\t size=%d (in byte)\n", desc->body.size);

    list_add(&node_memory->node, list_desc);

    return ERR_OK;

ERR_UNPACK_MEMORY:
    if (node_memory) {
        free(node_memory);
    }

    return err;
}

static int32_t tlv_unpack_desc_osc(size_t tag_length, struct list_head* list_desc, void* fp)
{
    int32_t  err;
    int32_t  len;
    int32_t  size;
    uint8_t* buf;
    struct tlv_desc_osc* desc;
    struct tlv_node_osc* node_osc;

    node_osc = calloc(1, sizeof(struct tlv_node_osc));
    if (node_osc == NULL) {
        err_print("Not enough memory!\n");
        err = ERR_FILE | ERR_SYS_NOT_ENOUGH_MEMORY;
        goto ERR_UNPACK_OSC;
    }
    desc = &node_osc->desc;

    size = sizeof(desc->body.type);
    buf  = &desc->body.type;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
    }

    tag_length -= size;
    dbg_unpack_print("\t\t type=0x%x\n", desc->body.type);

    if (desc->body.type > TLV_DESC_OSC_TYPE_WDT || desc->body.type <= 0) {
        err_print("Invalid OSC type! type=%d\n", desc->body.type);
        err = ERR_FILE |ERR_TLV_DESC;
        goto ERR_UNPACK_OSC;
    }

    if (tag_length <= 0) {
        err_print("Error, tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_OSC;
    }

    size = sizeof(desc->body.value);
    buf  = (uint8_t*)&desc->body.value;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_OSC;
    }
    desc->body.value = __REV32(desc->body.value);
    tag_length -= size;
    dbg_unpack_print("\t\t value=%d (in Hz)\n", desc->body.value);

    if (tag_length <= 0) {
        err_print("Invalid tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_OSC;
    }

    size = sizeof(desc->body.offset);
    buf  = (uint8_t*)&desc->body.offset;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
    }
    desc->body.offset = __REV32(desc->body.offset);
    tag_length -= size;
    dbg_unpack_print("\t\t offset=%d (in percent)\n", desc->body.offset);

    if (tag_length <= 0) {
        err_print("Invalid tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_OSC;
    }

    size = sizeof(desc->body.addr);
    buf  = (uint8_t*)&desc->body.addr;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
    }
    desc->body.addr = __REV32(desc->body.addr);
    dbg_unpack_print("\t\t address=0x%x\n", desc->body.addr);

    list_add(&node_osc->node, list_desc);

    return ERR_OK;

ERR_UNPACK_OSC:
    if (node_osc) {
        free(node_osc);
    }

    return err;

}

static int32_t tlv_unpack_desc_option(size_t tag_length, struct list_head* list_desc, void* fp)
{
    int32_t  err;
    int32_t  len;
    int32_t  size;
    uint8_t* buf;
    struct tlv_desc_option* desc;
    struct tlv_node_option* node_option;

    node_option = calloc(1, sizeof(struct tlv_node_option));
    if (node_option == NULL) {
        err_print("Not enough memory!\n");
        err = ERR_FILE | ERR_SYS_NOT_ENOUGH_MEMORY;
        goto ERR_UNPACK_OPTION;
    }
    desc = &node_option->desc;

    size = sizeof(desc->body.type);
    buf  = (uint8_t*)&desc->body.type;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_OPTION;
    }
    tag_length -= size;
    dbg_unpack_print("\t\t type=0x%x\n", desc->body.type);

    if (tag_length <= 0) {
        err_print("Invalid tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_OPTION;
    }

    if (desc->body.type == TLV_DESC_OPTION_TYPE_ADDR) {
        size = sizeof(desc->body.addr);
        buf  = (uint8_t*)&desc->body.addr;
        len  = file_read(buf, size, fp);
        if (len != size) {
            err_print("file_read() failed, ret=%d!\n", len);
            err = ERR_FILE | ERR_DRV_FLASH;
            goto ERR_UNPACK_OPTION;
        }
        desc->body.addr = __REV32(desc->body.addr);
        tag_length -= size;
        dbg_unpack_print("\t\t address=0x%x\n", desc->body.addr);

        if (tag_length <= 0) {
            err_print("Invalid tag_length=%d!\n", tag_length);
            err = ERR_FILE | ERR_TLV_DESC;
            goto ERR_UNPACK_OPTION;
        }
    }

    if (tag_length <= 0) {
        return ERR_OK;
    }
    desc->body.option_len = tag_length;
    size                  = tag_length;
    desc->body.bytes = calloc(1, size);
    if (desc->body.bytes == NULL) {
        err = ERR_SYS_NOT_ENOUGH_MEMORY;
        err_print("Not enought memory\n");
        goto ERR_UNPACK_OPTION;
    }
    
    buf  = desc->body.bytes;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_OPTION;
    }
    tag_length -= size;
    dbg_dump("\t\t bytes", buf, size);

    list_add(&node_option->node, list_desc);

    return ERR_OK;

ERR_UNPACK_OPTION:
    if (desc->body.bytes) {
        free(desc->body.bytes);
    }

    if (node_option) {
        free(node_option);
    }

    return err;

}

static int32_t tlv_unpack_desc_rolling_code(size_t tag_length, struct list_head* list_desc, void* fp)
{
    int32_t  err;
    int32_t  len;
    int32_t  size;
    uint8_t* buf;
    struct tlv_desc_rolling_code* desc;
    struct tlv_node_rolling_code* node_rolling_code;

    node_rolling_code = calloc(1, sizeof(struct tlv_node_rolling_code));
    if (node_rolling_code == NULL) {
        err_print("Not enough memory!\n");
        err = ERR_FILE | ERR_SYS_NOT_ENOUGH_MEMORY;
        goto ERR_UNPACK_ROLLING_CODE;
    }
    desc = &node_rolling_code->desc;

    size = sizeof(desc->body.step);
    buf  = &desc->body.step;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_ROLLING_CODE;
    }
    tag_length -= size;
    dbg_unpack_print("\t\t step=%d\n", desc->body.step);

    if (tag_length <= 0) {
        err_print("Invalid tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_ROLLING_CODE;
    }

    size = sizeof(desc->body.addr);
    buf  = (uint8_t*)&desc->body.addr;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_ROLLING_CODE;
    }
    desc->body.addr = __REV32(desc->body.addr);
    tag_length -= size;
    dbg_unpack_print("\t\t addr=0x%x (word addressing)\n", desc->body.addr);

    if (tag_length <= 0) {
        err_print("Invalid tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_ROLLING_CODE;
    }

    size = sizeof(desc->body.width);
    buf  = (uint8_t*)&desc->body.width;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_ROLLING_CODE;
    }
    tag_length -= size;
    dbg_unpack_print("\t\t width=%d (in bit)\n", desc->body.width);

    if (tag_length <= 0) {
        err_print("Invalid tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_ROLLING_CODE;
    }

    size = sizeof(desc->body.value);
    buf  = (uint8_t*)&desc->body.value;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_ROLLING_CODE;
    }
    dbg_unpack_print("\t\t value=%d \n", desc->body.value);

    list_add(&node_rolling_code->node, list_desc);

    return ERR_OK;

ERR_UNPACK_ROLLING_CODE:
    if (node_rolling_code) {
        free(node_rolling_code);
    }

    return err;
}


static int32_t tlv_unpack_desc_verify(size_t tag_length, struct list_head* list_desc, void* fp)
{
    int32_t err;
    size_t  size;
    size_t   len;
    uint8_t* buf;
    struct tlv_desc_verify* desc;
    struct tlv_node_verify* node_verify;

    node_verify = calloc(1, sizeof(struct tlv_node_verify));
    if (node_verify == NULL) {
        err_print("Not enough memory!\n");
        err = ERR_FILE | ERR_SYS_NOT_ENOUGH_MEMORY;
        goto ERR_UNPACK_VERIFY;
    }
    desc = &node_verify->desc;

    size = sizeof(desc->body.addr);
    buf  = (uint8_t*)&desc->body.addr;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_VERIFY;
    }
    
    tag_length -= size;
    dbg_unpack_print("\t\t addr=%d\n", desc->body.addr);

    if (tag_length <= 0) {
        err_print("Invalid tag_length=%d!\n", tag_length);
        err = ERR_FILE | ERR_TLV_DESC;
        goto ERR_UNPACK_VERIFY;
    }

    size = sizeof(desc->body.size);
    buf  = (uint8_t*)&desc->body.size;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_VERIFY;
    }
    desc->body.size = __REV32(desc->body.size);
    dbg_unpack_print("\t\t size=0x%x\n", desc->body.size);

    list_add(&node_verify->node, list_desc);

    return ERR_OK;

ERR_UNPACK_VERIFY:
    if (node_verify) {
        free(node_verify);
    }

    return err;

}

static int32_t tlv_unpack_desc_time(size_t tag_length, struct list_head* list_desc, void* fp)
{
    int32_t err;
    int32_t  size;
    uint8_t* buf;
    int32_t  len;
    struct tlv_desc_time* desc;
    struct tlv_node_time* node_time;

    node_time = calloc(1, sizeof(struct tlv_node_time));
    if (node_time == NULL) {
        err_print("Not enough memory!\n");
        err = ERR_FILE | ERR_SYS_NOT_ENOUGH_MEMORY;
        goto ERR_UNPACK_PROGRAM;
    }
    desc = &node_time->desc;

    buf  = (uint8_t*)&desc->type;
    size = sizeof(desc->type);
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_PROGRAM;
    }

    if (desc->type == TLV_DESC_TIME_TYPE_YYMMDDhhmm) {
        buf  = (uint8_t*)&desc->day.year;
        size = sizeof(desc->day.year);
        len  = file_read(buf, size, fp);
        if (len != size) {
            err_print("file_read() failed, ret=%d!\n", len);
            err = ERR_FILE | ERR_DRV_FLASH;
            goto ERR_UNPACK_PROGRAM;
        }   
        
        buf  = (uint8_t*)&desc->day.month;
        size = sizeof(desc->day.month);
        len  = file_read(buf, size, fp);
        if (len != size) {
            err_print("file_read() failed, ret=%d!\n", len);
            err = ERR_FILE | ERR_DRV_FLASH;
            goto ERR_UNPACK_PROGRAM;
        } 
        
        buf  = (uint8_t*)&desc->day.date;
        size = sizeof(desc->day.date);
        len  = file_read(buf, size, fp);
        if (len != size) {
            err_print("file_read() failed, ret=%d!\n", len);
            err = ERR_FILE | ERR_DRV_FLASH;
            goto ERR_UNPACK_PROGRAM;
        } 
        
        buf  = (uint8_t*)&desc->time.hour;
        size = sizeof(desc->time.hour);
        len  = file_read(buf, size, fp);
        if (len != size) {
            err_print("file_read() failed, ret=%d!\n", len);
            err = ERR_FILE | ERR_DRV_FLASH;
            goto ERR_UNPACK_PROGRAM;
        }  
        
        buf  = (uint8_t*)&desc->time.minutes;
        size = sizeof(desc->time.minutes);
        len  = file_read(buf, size, fp);
        if (len != size) {
            err_print("file_read() failed, ret=%d!\n", len);
            err = ERR_FILE | ERR_DRV_FLASH;
            goto ERR_UNPACK_PROGRAM;
        }   
        dbg_unpack_print("\t\t type(YYMMDD HH:MM)=20%d-%02d-%02d %02d:%02d\n", 
            desc->day.year, desc->day.month, desc->day.date, desc->time.hour, desc->time.minutes);
    }
    

    list_add(&node_time->node, list_desc);

    return ERR_OK;

ERR_UNPACK_PROGRAM:
    if (node_time) {
        free(node_time);
    }

    return err;

}



static int32_t tlv_unpack_desc_program(size_t tag_length, struct list_head* list_desc, void* fp)
{
    int32_t err;
    int32_t  size;
    uint8_t* buf;
    int32_t  len;
    struct tlv_desc_program* desc;
    struct tlv_node_program* node_program;

    node_program = calloc(1, sizeof(struct tlv_node_program));
    if (node_program == NULL) {
        err_print("Not enough memory!\n");
        err = ERR_FILE | ERR_SYS_NOT_ENOUGH_MEMORY;
        goto ERR_UNPACK_PROGRAM;
    }
    desc = &node_program->desc;

    buf  = (uint8_t*)&desc->body.bitmap;
    size = sizeof(desc->body.bitmap);
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_PROGRAM;
    }
    dbg_unpack_print("\t\t program=0x%x\n", desc->body.bitmap);

    list_add(&node_program->node, list_desc);

    return ERR_OK;

ERR_UNPACK_PROGRAM:
    if (node_program) {
        free(node_program);
    }

    return err;

}

static int32_t tlv_unpack_desc_limit(size_t tag_length, struct list_head* list_desc, void* fp)
{
    int32_t err;
    int32_t  size;
    uint8_t* buf;
    int32_t  len;
    struct tlv_desc_limit* desc;
    struct tlv_node_limit* node_limit;

    node_limit = calloc(1, sizeof(struct tlv_node_limit));
    if (node_limit == NULL) {
        err_print("Not enough memory!\n");
        err = ERR_FILE | ERR_SYS_NOT_ENOUGH_MEMORY;
        goto ERR_UNPACK_LIMIT;
    }
    desc = &node_limit->desc;

    buf  = (uint8_t*)&desc->body.count;
    size = 4;
    len  = file_read(buf, size, fp);
    if (len != size) {
        err_print("file_read() failed, ret=%d!\n", len);
        err = ERR_FILE | ERR_DRV_FLASH;
        goto ERR_UNPACK_LIMIT;
    }
    dbg_unpack_print("\t\t limit=0x%x\n", desc->body.count);

    list_add(&node_limit->node, list_desc);

    return ERR_OK;

ERR_UNPACK_LIMIT:
    if (node_limit) {
        free(node_limit);
    }

    return err;
}

static int32_t tlv_pack_file_head(struct tlv_file_head* head, void* fp)
{
    int32_t   ret;
    int32_t   len;
    uint8_t*  data;
    size_t    size;
    
    dbg_pack_print("[TLV] file_id=%d,file_length=%d\n", head->file_id, head->file_length);

    data  = (uint8_t*)&head->file_id;
    size  = sizeof(head->file_id);
    ret   = file_write(data, size, fp);
    if (ret != size) {
        return -1;
    }

    len   = __REV32(head->file_length);
    data  = (uint8_t*)&len;
    size  = sizeof(len);
    ret   = file_write(data, size, fp);
    if (ret != size) {
        return -1;
    }

    return TLV_FILE_HEAD_LENGTH;
}


static int32_t tlv_pack_desc_head(struct tlv_desc_head* head, void* fp)
{
    int32_t   ret;
    int32_t   len;
    uint8_t*  data;
    size_t    size;

    dbg_pack_print("\t\t tag=%d,length=%d\n", head->tag, head->length);

    data  = (uint8_t*)&head->tag;
    size  = sizeof(head->tag);
    ret   = file_write(data, size, fp);
    if (ret != size) {
        return -1;
    }

    len   = __REV32(head->length);
    data  = (uint8_t*)&len;
    size  = sizeof(len);
    ret   = file_write(data, size, fp);
    if (ret != size) {
        return -1;
    }

    return TLV_DESC_HEAD_LENGTH;
}

static int32_t tlv_pack_desc_id(struct list_head* list, void* fp)
{
    int32_t                 ret;
    uint8_t*                data;
    size_t                  size;
    uint32_t                tmp;
    struct list_head*       pos;
    struct tlv_desc_id*     desc;
    struct tlv_node_id*     entry;

    list_for_each(pos, list) {
    
        dbg_pack_print("\t ID Descriptor\n");
        
        entry = list_entry(pos, struct tlv_node_id, node);
        desc  = &entry->desc;
        ret   = tlv_pack_desc_head(&desc->head, fp);
        if (ret == 0) {
            err_print("tlv_pack_desc_head(%p,%p) failed\n", &desc->head, fp);
            return -1;
        }

        data = &desc->body.type;
        size = sizeof(desc->body.type);
        ret  = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }
        
        dbg_pack_print("\t\t type=%d\n", desc->body.type);
        dbg_pack_print("\t\t serial_id=0x%x\n", desc->body.serial_id);
        
        tmp  = __REV32(desc->body.serial_id);
        data = (uint8_t*)&tmp;
        size = sizeof(tmp);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }   

        data = &desc->body.day.year;
        size = sizeof(desc->body.day.year);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }    

        data = &desc->body.day.month;
        size = sizeof(desc->body.day.month);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }    

        data = &desc->body.day.date;
        size = sizeof(desc->body.day.date);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }  

        
        dbg_pack_print("\t\t 20%02d-%02d-%02d\n", 
                desc->body.day.year, desc->body.day.month, desc->body.day.date);

        tmp = __REV32(desc->body.count);
        data = (uint8_t*)&tmp;
        size = sizeof(tmp);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        } 
        
        dbg_pack_print("\t\t program count=0x%x\n", desc->body.count);
    }

    return ERR_OK;
}

static int32_t tlv_pack_desc_version(struct list_head* list, void* fp)
{
    int32_t                     ret;
    uint8_t*                    data;
    size_t                      size;
    uint32_t                    tmp;
    struct list_head*           pos;
    struct tlv_desc_version*    desc;
    struct tlv_node_version*    entry;

    list_for_each(pos, list) {        
        
        dbg_pack_print("\t Version Descriptor\n");
        
        entry = list_entry(pos, struct tlv_node_version, node);
        desc  = &entry->desc;
        ret   = tlv_pack_desc_head(&desc->head, fp);
        if (ret == 0) {
            err_print("tlv_pack_desc_head(%p,%p) failed\n", &desc->head, fp);
            return -1;
        }
        
        dbg_pack_print("\t\t type=%d\n", desc->body.type);
        dbg_pack_print("\t\t value=0x%x\n", desc->body.value);

        data = &desc->body.type;
        size = sizeof(desc->body.type);
        ret  = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }        
        
        //tmp  = __REV32(desc->body.value);
        tmp  = desc->body.value;
        data = (uint8_t*)&tmp;
        size = sizeof(tmp);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }  
    }

    return ERR_OK;
}




static int32_t tlv_pack_desc_crypto(struct list_head* list, void* fp)
{
    int32_t                 ret;
    uint8_t*                data;
    size_t                  size;
    struct list_head*       pos;
    struct tlv_desc_crypto* desc;
    struct tlv_node_crypto* entry;

    list_for_each(pos, list) {
        
        dbg_pack_print("\t Crypto Desciptor\n");
        
        entry = list_entry(pos, struct tlv_node_crypto, node);
        desc  = &entry->desc;
        ret = tlv_pack_desc_head(&desc->head, fp);
        if (ret == 0) {
            err_print("tlv_pack_desc_head(%p,%p) failed\n", &desc->head, fp);
            return -1;
        }

        dbg_pack_print("\t\t algorithm=%d\n", desc->body.algorithm);
        dbg_pack_print("\t\t key=%d\n", desc->body.key);

        data = &desc->body.algorithm;
        size = sizeof(desc->body.algorithm);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }

        data = &desc->body.key;
        size = sizeof(desc->body.key);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }      
    }

    return ERR_OK;
}

static int32_t tlv_pack_desc_voltage(struct list_head* list, void* fp)
{
    int32_t                  ret;
    uint8_t*                 data;
    uint32_t                 tmp;
    size_t                   size;
    struct list_head*        pos;
    struct tlv_desc_voltage* desc;
    struct tlv_node_voltage* entry;
    
    list_for_each(pos, list) {
        
        dbg_pack_print("\t Voltage Descriptor\n");
        
        entry = list_entry(pos, struct tlv_node_voltage, node);
        desc  = &entry->desc;
        ret = tlv_pack_desc_head(&desc->head, fp);
        if (ret == 0) {
            err_print("tlv_pack_desc_head(%p,%p) failed\n", &desc->head, fp);
            return -1;
        }
        
        dbg_pack_print("\t\t type=%d\n", desc->body.type);
        dbg_pack_print("\t\t value=%d\n", desc->body.value);
        dbg_pack_print("\t\t offset=%d\n", desc->body.offset);

        data = &desc->body.type;
        size = sizeof(desc->body.type);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }

        tmp = __REV32(desc->body.value);
        data = (uint8_t*)&tmp;
        size = sizeof(tmp);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }

        tmp = __REV32(desc->body.offset);
        data = (uint8_t*)&tmp;
        size = sizeof(tmp);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }      
    }

    return ERR_OK;
}

static int32_t tlv_pack_desc_osc(struct list_head* list, void* fp)
{
    int32_t                  ret;
    uint8_t*                 data;
    uint32_t                 tmp;
    size_t                   size;
    struct list_head*        pos;
    struct tlv_desc_osc*     desc;
    struct tlv_node_osc*     entry;

    list_for_each(pos, list) {        
        
        dbg_pack_print("\t OSC Descriptor\n");
        
        entry = list_entry(pos, struct tlv_node_osc, node);
        desc  = &entry->desc;
        ret   = tlv_pack_desc_head(&desc->head, fp);
        if (ret == 0) {
            err_print("tlv_pack_desc_head(%p,%p) failed\n", &desc->head, fp);
            return -1;
        }
        
        dbg_pack_print("\t\t type=%d\n", desc->body.type);
        dbg_pack_print("\t\t value=%d\n", desc->body.value);
        dbg_pack_print("\t\t offset=%d\n", desc->body.offset);
        dbg_pack_print("\t\t addr=0x%x\n", desc->body.addr);

        data = &desc->body.type;
        size = sizeof(desc->body.type);
        ret  = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }

        tmp = __REV32(desc->body.value);
        data = (uint8_t*)&tmp;
        size = sizeof(tmp);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        } 

        tmp = __REV32(desc->body.offset);
        data = (uint8_t*)&tmp;
        size = sizeof(tmp);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }

        tmp = __REV32(desc->body.addr);
        data = (uint8_t*)&tmp;
        size = sizeof(tmp);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }     
    }

    return ret;
}

static int32_t tlv_pack_desc_chip(struct list_head* list, void* fp)
{
    int32_t                  ret;
    uint8_t*                 data;
    size_t                   size;
    struct list_head*        pos;
    struct tlv_desc_chip*    desc;
    struct tlv_node_chip*    entry;

    list_for_each(pos, list) {
    
        dbg_pack_print("\t Chip Descriptor\n");
    
        entry = list_entry(pos, struct tlv_node_chip, node);
        desc  = &entry->desc;
        ret   = tlv_pack_desc_head(&desc->head, fp);
        if (ret == 0) {
            err_print("tlv_pack_desc_head(%p,%p) failed\n", &desc->head, fp);
            return -1;
        }

        dbg_pack_print("\t\t name_length=%d\n", desc->body.name_length);
        dbg_pack_print("\t\t name=%s\n", desc->body.name);

        data = &desc->body.name_length;
        size = sizeof(desc->body.name_length);
        ret  = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }

        if (desc->body.name_length > 0) {
            data = desc->body.name;
            size = desc->body.name_length;
            ret  = file_write(data, size, fp);
            if (ret != size) {
                err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
                return -1;
            }
        }
        
        dbg_pack_print("\t\t id_length=%d\n", desc->body.id_length);

        data = &desc->body.id_length;
        size = sizeof(desc->body.id_length);
        ret  = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }
        
        if (desc->body.id_length > 0) {
            data = desc->body.id;
            size = desc->body.id_length;
            ret  = file_write(data, size, fp);
            if (ret != size) {
                err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
                return -1;
            }
        } 
        
        dbg_pack_print("\t\t type_length=%d\n", desc->body.type_length);

        data = &desc->body.type_length;
        size = sizeof(desc->body.type_length);
        ret  = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }
    
        if (desc->body.type_length > 0) {
            data = desc->body.type;
            size = desc->body.type_length;
            ret  = file_write(data, size, fp);
            if (ret != size) {
                err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
                return -1;
            }
        }         
    }

    return ret;
}

static int32_t tlv_pack_desc_memory(struct list_head* list, void* fp)
{
    int32_t                  ret;
    uint8_t*                 data;
    uint32_t                 tmp;
    size_t                   size;
    struct list_head*        pos;
    struct tlv_desc_memory*  desc;
    struct tlv_node_memory*  entry;

    list_for_each(pos, list) {        
        
        dbg_pack_print("\t Memory Descriptor\n");
        
        entry = list_entry(pos, struct tlv_node_memory, node);
        desc  = &entry->desc;
        ret   = tlv_pack_desc_head(&desc->head, fp);
        if (ret == 0) {
            err_print("tlv_pack_desc_head(%p,%p) failed\n", &desc->head, fp);
            return -1;
        }

        dbg_pack_print("\t\t type=%d\n", desc->body.type);
        dbg_pack_print("\t\t width=%d\n", desc->body.width);
        dbg_pack_print("\t\t addr=%d\n", desc->body.addr);
        dbg_pack_print("\t\t size=%d\n", desc->body.size);

        data = &desc->body.type;
        size = sizeof(desc->body.type);
        ret  = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }

        data = &desc->body.width;
        size = sizeof(desc->body.width);
        ret  = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }

        tmp = __REV32(desc->body.addr);
        data = (uint8_t*)&tmp;
        size = sizeof(tmp);
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }

        tmp  = __REV32(desc->body.size);
        data = (uint8_t*)&tmp;
        size = sizeof(tmp);
        ret  = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }  
    }

    return ERR_OK;
}

static int32_t tlv_pack_desc_option(struct list_head* list, void* fp)
{
    int32_t                  ret;
    uint8_t*                 data;
    uint32_t                 tmp;
    size_t                   size;
    struct list_head*        pos;
    struct tlv_desc_option*  desc;
    struct tlv_node_option*  entry;

    list_for_each(pos, list) {
        
        dbg_pack_print("\t Option Descriptor\n");
        
        entry = list_entry(pos, struct tlv_node_option, node);
        desc  = &entry->desc;
        ret   = tlv_pack_desc_head(&desc->head, fp);
        if (ret == 0) {
            err_print("tlv_pack_desc_head(%p,%p) failed\n", &desc->head, fp);
            return -1;
        }

        data = &desc->body.type;
        size = sizeof(desc->body.type);
        ret  = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }
        
        if (desc->body.type == TLV_DESC_OPTION_TYPE_ADDR) {
            tmp  = __REV32(desc->body.addr);
            data = (uint8_t*)&tmp;
            size = sizeof(tmp);
            ret  = file_write(data, size, fp);
            if (ret != size) {
                err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
                return -1;
            }
        }

        data = desc->body.bytes;
        size = desc->body.option_len;
        ret = file_write(data, size, fp);
        if (ret != size) {
            err_print("file_write(%p,%d,%p) failed\n", data, size, fp);
            return -1;
        }   
    }

    return ret;
}


static int32_t tlv_file_calc_len(struct tlv_file* tlv)
{
    int32_t           len;
    struct list_head* pos;

    len = 0;    
    list_for_each(pos, &tlv->list_id) {
        len += TLV_DESC_ID_LENGTH;
        dbg_pack_print("ID Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_version) {
        len += TLV_DESC_VERSION_LENGTH;
        dbg_pack_print("Version Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_crypto) {
        len += TLV_DESC_CRYPTO_LENGTH;
        dbg_pack_print("Crypto Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_code) {
//        len += TLV_DESC_CODE_LENGTH;
        dbg_pack_print("Code Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_voltage) {
        len += TLV_DESC_VOLTAGE_LENGTH;
        dbg_pack_print("Voltage Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_osc) {
        len += TLV_DESC_OSC_LENGTH;
        dbg_pack_print("OSC Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_chip) {
        struct tlv_node_chip* entry;
        struct tlv_desc_chip* desc;

        entry = list_entry(pos, struct tlv_node_chip, node);
        desc  = &entry->desc;
        len   += desc->head.length;
        
        dbg_pack_print("Chip Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_memory) {
        len += TLV_DESC_MEMORY_LENGTH;
        
        dbg_pack_print("Memory Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_option) {
        struct tlv_node_option* entry;
        struct tlv_desc_option* desc;

        entry = list_entry(pos, struct tlv_node_option, node);
        desc  = &entry->desc;
        len += TLV_DESC_HEAD_LENGTH + desc->head.length;
        dbg_pack_print("Option Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_rolling_code) {
        struct tlv_node_rolling_code* entry;
        struct tlv_desc_rolling_code* desc;

        entry = list_entry(pos, struct tlv_node_rolling_code, node);
        desc  = &entry->desc;
        len += TLV_DESC_HEAD_LENGTH + desc->head.length;
        dbg_pack_print("Rolling Code Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_verify) {        
        len += TLV_DESC_VERIFY_LENGTH;
        dbg_pack_print("Verify Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_time) {
        len += TLV_DESC_TIME_LENGTH;
        dbg_pack_print("Time Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_log) {
        len += TLV_DESC_PROGRAM_LENGTH;
        dbg_pack_print("Log Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_program) {
        len += TLV_DESC_PROGRAM_LENGTH;
        dbg_pack_print("Program Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_limit) {
        len += TLV_DESC_LIMIT_LENGTH;
        dbg_pack_print("Limit Desc, len=%d\n", len);
    }
    list_for_each(pos, &tlv->list_program_status) {        
        len += TLV_DESC_PROGRAM_STATUS_LENGTH;
        dbg_pack_print("Program status Desc, len=%d\n", len);
    }  
    return len;
}


static uint32_t tlv_file_calc_crc32(int32_t file_id)
{
    uint32_t                crc32;
    int32_t                 len;
    int32_t                 size;
    int32_t                 offset;
    uint8_t*                ptr;
    void*                   fp;
    int32_t                 file_length;
    uint8_t                 data[MAX_BUF_SIZE];
    struct tlv_file_head    file_head;

    fp = file_open(file_id, "r");
    if (fp == NULL) {
        err_print("Invalid argument, fp=%p\n", fp);
        return 0;
    }

    /* Got the file_firmware length */
    size = sizeof(file_head.file_id);
    ptr  = (uint8_t*)&file_head.file_id;
    len  = file_read(ptr, size, fp);
    if (len != size) {
        err_print("file_read(%p,%d,%p) failed\n", ptr, size, fp);
        goto ERR_TLV_FILE_CALC_CRC32;
    }
    
    size = sizeof(file_head.file_length);
    ptr  = (uint8_t*)&file_head.file_length;
    len  = file_read(ptr, size, fp);
    if (len != size) {
        err_print("file_read(%p,%d,%p) failed\n", ptr, size, fp);
        goto ERR_TLV_FILE_CALC_CRC32;
    }
    file_length = __REV32(file_head.file_length);

    dbg_print("[TLV] file_id=%d,file_length=%d\n", file_head.file_id, file_length);

    /* Calculate the CRC32 of file_hex, skip file_head and last CRC32 */
    file_length -= 4;
    for (offset = 0, crc32 = CRC32_INIT_VALUE; offset < file_length; offset += len) {
        len  = file_length - offset;
        size = len > MAX_BUF_SIZE ? MAX_BUF_SIZE : len;
        len  = file_read(data, size, fp);
        if (len != size) {
            err_print("file_read(%p,%d,%p) failed, ret=%d\n",data, size, fp, len);            

            goto ERR_TLV_FILE_CALC_CRC32;
        }
        
        crc32 = crc32_sw_calc(crc32, data, len);
    }

    crc32 ^= CRC32_XOR_VALUE;
    crc32 = __REV32(crc32);
   
    
    dbg_print("[TLV] CRC32=0x%x\n", crc32);
    file_close(fp);
    
    return crc32;
    
ERR_TLV_FILE_CALC_CRC32:
   
    file_close(fp);
    return 0;
}

static int32_t tlv_file_pack_desc(struct tlv_file* tlv)
{
    int32_t ret;
    int32_t len;
    void*   fp;

    len = tlv_file_calc_len(tlv);
    if (len == 0) {
        err_print("tlv_file_calc_len(%p) failed\n", tlv);
        return ERR_FILE;
    }
    
    fp = file_open(tlv->head.file_id, "w");
    if (fp == NULL) {
        err_print("file_open(%d,\"w\") failed\n", tlv->head.file_id);
        return ERR_FILE;
    }
    
    tlv->head.file_length = len + TLV_FILE_CRC32_LENGTH;
    ret = tlv_pack_file_head(&tlv->head, fp);
    if (ret < 0) {
        err_print("tlv_pack_desc_crypto(%p,%p) failed\n", &tlv->list_chip, fp);
        ret = ERR_FILE;
        goto ERR_TLV_FILE_PACK_DESC;
    }

    ret = tlv_pack_desc_id(&tlv->list_id, fp);
    if (ret < 0) {
        err_print("tlv_pack_desc_id(%p,%p) failed\n", &tlv->list_id, fp);
        ret = ERR_FILE;
        goto ERR_TLV_FILE_PACK_DESC;
    }
    
    ret = tlv_pack_desc_version(&tlv->list_version, fp);
    if (ret < 0) {
        err_print("tlv_pack_desc_version(%p,%p) failed\n", &tlv->list_version, fp);
        ret = ERR_FILE;
        goto ERR_TLV_FILE_PACK_DESC;
    }
  
    ret = tlv_pack_desc_crypto(&tlv->list_crypto, fp);
    if (ret < 0) {
        err_print("tlv_pack_desc_crypto(%p,%p) failed\n", &tlv->list_crypto, fp);
        ret = ERR_FILE;
        goto ERR_TLV_FILE_PACK_DESC;
    }
    
//    ret = tlv_pack_desc_code(&tlv->list_code, fp);
//    if (ret) {
//        err_print("tlv_pack_desc_code(%p,%p) failed\n", &tlv->list_code, fp);
//        ret = ERR_FILE;
//        goto ERR_TLV_FILE_PACK_DESC;
//    } 

    ret = tlv_pack_desc_voltage(&tlv->list_voltage, fp);
    if (ret < 0) {
        err_print("tlv_pack_desc_voltage(%p,%p) failed\n", &tlv->list_voltage, fp);
        ret = ERR_FILE;
        goto ERR_TLV_FILE_PACK_DESC;
    }
    
    ret = tlv_pack_desc_osc(&tlv->list_osc, fp);
    if (ret < 0) {
        err_print("tlv_pack_desc_osc(%p,%p) failed\n", &tlv->list_osc, fp);
        ret = ERR_FILE;
        goto ERR_TLV_FILE_PACK_DESC;
    }
    
    ret = tlv_pack_desc_chip(&tlv->list_chip, fp);
    if (ret < 0) {
        err_print("tlv_pack_desc_chip(%p,%p) failed\n", &tlv->list_chip, fp);
        ret = ERR_FILE;
        goto ERR_TLV_FILE_PACK_DESC;
    }
    
    ret = tlv_pack_desc_memory(&tlv->list_memory, fp);
    if (ret < 0) {
        err_print("tlv_pack_desc_memory(%p,%p) failed\n", &tlv->list_memory, fp);
        ret = ERR_FILE;
        goto ERR_TLV_FILE_PACK_DESC;
    }
    
    ret = tlv_pack_desc_option(&tlv->list_option, fp);
    if (ret < 0) {
        err_print("tlv_pack_desc_option(%p,%p) failed\n", &tlv->list_option, fp);
        ret = ERR_FILE;
        goto ERR_TLV_FILE_PACK_DESC;
    }
    
//    ret = tlv_pack_desc_rolling_code(&tlv->list_rolling_code, fp);
//    if (ret) {
//        err_print("tlv_pack_desc_rolling_code(%p,%p) failed\n", &tlv->list_rolling_code, fp);
//        ret = ERR_FILE;
//        goto ERR_TLV_FILE_PACK_DESC;
//    }
    
//    ret = tlv_pack_desc_verify(&tlv->list_verify, fp);
//    if (ret < 0) {
//        err_print("tlv_pack_desc_verify(%p,%p) failed\n", &tlv->list_verify, fp);
//        ret = ERR_FILE;
//        goto ERR_TLV_FILE_PACK_DESC;
//    }

//    ret = tlv_pack_desc_time(&tlv->list_time, fp);
//    if (ret < 0) {
//        err_print("tlv_pack_desc_time(%p,%p) failed\n", &tlv->list_time, fp);
//        ret = ERR_FILE;
//        goto ERR_TLV_FILE_PACK_DESC;
//    }

//    ret = tlv_pack_desc_log(&tlv->list_log, fp);
//    if (ret < 0) {
//        err_print("tlv_pack_desc_log(%p,%p) failed\n", &tlv->list_log, fp);
//        ret = ERR_FILE;
//        goto ERR_TLV_FILE_PACK_DESC;
//    }

//    ret = tlv_pack_desc_program(&tlv->list_program, fp);
//    if (ret) {
//        err_print("tlv_pack_desc_program(%p,%p) failed\n", &tlv->list_program, fp);
//        ret = ERR_FILE;
//        goto ERR_TLV_FILE_PACK_DESC;
//    }

//    ret = tlv_pack_desc_limit(&tlv->list_limit, fp);
//    if (ret < 0) {
//        err_print("tlv_pack_desc_limit(%p,%p) failed\n", &tlv->list_limit, fp);
//        ret = ERR_FILE;
//        goto ERR_TLV_FILE_PACK_DESC;
//    }
 
    return file_close(fp);

ERR_TLV_FILE_PACK_DESC:
    file_close(fp);
    return ret;
}




static int32_t tlv_file_append_crc32(int32_t file_id, uint32_t crc32)
{ 
    int32_t  ret;
    uint8_t* buf;
    void*    fp;
    size_t   size;
    
    dbg_pack_print("[TLV] tlv_file_append_crc32()\n");

    fp = file_open(file_id, "a");
    if (fp == NULL) {
        err_print("file_open(%d,\"a\") failed\n", file_id);
        return -1;
    }

    //crc32 = __REV32(crc32);
    buf = (uint8_t*)&crc32;
    size = sizeof(crc32);
    ret  = file_write(buf, size, fp);
    if (ret != size) {
        err_print("file_write(%p,%d,%p) failed\n", buf, size, fp);
        file_close(fp);
        return -1;
    }

    return file_close(fp);
}

static void tlv_file_dump(int32_t file_id)
{
    int32_t ret;
    uint8_t buf[256];

    void*   fp;

    fp = file_open(file_id, "r");
    if (fp == NULL) {
        return;
    }

    ret = sizeof(buf);
    while (ret == sizeof(buf)) {
        ret = file_read(buf, sizeof(buf), fp);
        dbg_file_dump("\t TLV file dump", buf, ret);
    }

    file_close(fp);
}

int32_t tlv_file_init(struct tlv_file* tlv)
{

    INIT_LIST_HEAD(&tlv->list_id);
    INIT_LIST_HEAD(&tlv->list_version);
    INIT_LIST_HEAD(&tlv->list_crypto);
    INIT_LIST_HEAD(&tlv->list_code);
    INIT_LIST_HEAD(&tlv->list_voltage);
    INIT_LIST_HEAD(&tlv->list_osc);
    INIT_LIST_HEAD(&tlv->list_chip);
    INIT_LIST_HEAD(&tlv->list_memory);
    INIT_LIST_HEAD(&tlv->list_option);
    INIT_LIST_HEAD(&tlv->list_rolling_code);
    INIT_LIST_HEAD(&tlv->list_verify);
    INIT_LIST_HEAD(&tlv->list_time);
    INIT_LIST_HEAD(&tlv->list_log);
    INIT_LIST_HEAD(&tlv->list_program);
    INIT_LIST_HEAD(&tlv->list_limit);
    INIT_LIST_HEAD(&tlv->list_program_status);    
    
    return ERR_OK;
}

int32_t tlv_file_destroy(struct tlv_file* tlv)
{
    struct list_head* pos;
    struct list_head* tmp;

    list_for_each_safe(pos, tmp, &tlv->list_chip) {
        struct tlv_node_chip* chip;
        printf("%s(),%d\n", __func__, __LINE__);
        list_del(pos);
        printf("%s(),%d\n", __func__, __LINE__);

        chip = list_entry(pos, struct tlv_node_chip, node);
        if (chip->desc.body.name) {
            printf("%s(),%d\n", __func__, __LINE__);
            free(chip->desc.body.name);
        }
        if (chip->desc.body.id) {
        printf("%s(),%d\n", __func__, __LINE__);
            free(chip->desc.body.id);
        }
        if (chip->desc.body.type) {
        printf("%s(),%d\n", __func__, __LINE__);
            free(chip->desc.body.type);
        }
        printf("%s(),%d\n", __func__, __LINE__);

        free(pos);
    }

    list_for_each_safe(pos, tmp, &tlv->list_code) {
        printf("%s(),%d\n", __func__, __LINE__);
        list_del(pos);
        printf("%s(),%d\n", __func__, __LINE__);
        free(pos);
    }

    list_for_each_safe(pos, tmp, &tlv->list_limit) {
        printf("%s(),%d\n", __func__, __LINE__);
        list_del(pos);
        printf("%s(),%d\n", __func__, __LINE__);
        free(pos);
    }

    list_for_each_safe(pos, tmp, &tlv->list_memory) {
        printf("%s(),%d\n", __func__, __LINE__);
        list_del(pos);
        printf("%s(),%d\n", __func__, __LINE__);
        free(pos);
    }

    list_for_each_safe(pos, tmp, &tlv->list_option) {
        struct tlv_node_option* option;

        printf("%s(),%d\n", __func__, __LINE__);

        list_del(pos);
        printf("%s(),%d\n", __func__, __LINE__);
        option = list_entry(pos, struct tlv_node_option, node);
        
        if (option->desc.body.bytes) {
        printf("%s(),%d\n", __func__, __LINE__);
            free(option->desc.body.bytes);
        }

        printf("%s(),%d\n", __func__, __LINE__);

        free(pos);
    }

    list_for_each_safe(pos, tmp, &tlv->list_osc) {
        list_del(pos);
        printf("%s(),%d\n", __func__, __LINE__);
        free(pos);
        printf("%s(),%d\n", __func__, __LINE__);
    }

    list_for_each_safe(pos, tmp, &tlv->list_program) {
        printf("%s(),%d\n", __func__, __LINE__);
        list_del(pos);
        printf("%s(),%d\n", __func__, __LINE__);
        free(pos);
        printf("%s(),%d\n", __func__, __LINE__);
    }

    list_for_each_safe(pos, tmp, &tlv->list_rolling_code) {
        printf("%s(),%d\n", __func__, __LINE__);
        list_del(pos);
        printf("%s(),%d\n", __func__, __LINE__);
        free(pos);
        printf("%s(),%d\n", __func__, __LINE__);
    }

    list_for_each_safe(pos, tmp, &tlv->list_verify) {
        printf("%s(),%d\n", __func__, __LINE__);
        list_del(pos);
        printf("%s(),%d\n", __func__, __LINE__);
        free(pos);
    }

    list_for_each_safe(pos, tmp, &tlv->list_voltage) {
        printf("%s(),%d\n", __func__, __LINE__);
        list_del(pos);
        printf("%s(),%d\n", __func__, __LINE__);
        free(pos);
    }

    return ERR_OK;
}

int32_t tlv_file_unpack(int32_t file_id, struct tlv_file* tlv)
{
    int32_t                 ret;
    int32_t                 len;
    int32_t                 file_length;
    int32_t                 tag_length;
    uint8_t*                buf;
    uint32_t                size;
    struct tlv_file_head    file_head;
    struct tlv_desc_head    tlv_head;
    void*                   fp;
    
    dbg_unpack_print("[TLV] tlv_unpack ...\n");

    fp = file_open(file_id, "r");
    if (fp == NULL) {
        err_print("file_open(%d, \"r\") failed\n", file_id);
        return ERR_FILE;
    }

    size = sizeof(tlv->head.file_id);
    buf  = (uint8_t*)&tlv->head.file_id;
    ret  = file_read(buf, size, fp);
    if (ret != size) {
        err_print("file_read(%p,%d,%p) failed\n", buf, size, fp);
        ret = ERR_FILE;
        goto ERR_TLV_UNPACK;
    }

    size = sizeof(file_length);
    buf  = (uint8_t*)&file_length;
    ret  = file_read(buf, size, fp);
    if (ret != size) {
        ret = ERR_FILE; 
        err_print("file_read(%p,%d,%p) failed\n", buf, size, fp);       
        goto ERR_TLV_UNPACK;
    }

    file_length = __REV32(file_length);
    tlv->head.file_length = file_length;

    dbg_unpack_print("\t file_id=%d\n", file_head.file_id);
    dbg_unpack_print("\t file_length=%d\n", file_length);
    
    file_length -= 4; /* 4 bytes CRC32 */

    while (file_length > 0) {
        /* Read the head of TLV */
        size = sizeof(tlv_head.tag);
        buf  = (uint8_t*)&tlv_head.tag;
        ret  = file_read(buf, size, fp);
        if (ret != size) {
            ret = ERR_FILE | ERR_TLV_DESC; 
            err_print("file_read(%p,%d,%p) failed\n", buf, size, fp); 
            goto ERR_TLV_UNPACK;
        }
        file_length -= size;
        
        size = sizeof(tlv_head.length);
        buf  = (uint8_t*)&tlv_head.length;
        ret  = file_read(buf, size, fp);
        if (ret != size) {            
            ret = ERR_FILE | ERR_TLV_DESC;
            err_print("file_read(%p,%d,%p) failed\n", buf, size, fp); 
            goto ERR_TLV_UNPACK;
        }
        file_length -= size;
        
        tag_length  = __REV32(tlv_head.length);

        dbg_unpack_print("\t tag=%d\n", tlv_head.tag);
        dbg_unpack_print("\t tag_length=%d\n", tag_length);

        switch (tlv_head.tag) {
        case TLV_DESC_TAG_CHIP:
            dbg_unpack_print("\t Chip Descriptor, len=%d\n", tag_length);
            ret = tlv_unpack_desc_chip(tag_length, &tlv->list_chip, fp);
            break;
        case TLV_DESC_TAG_VOLTAGE:
            dbg_unpack_print("\t Voltage Descriptor, len=%d\n", tag_length);
            ret = tlv_unpack_desc_voltage(tag_length, &tlv->list_voltage, fp);
            break;
        case TLV_DESC_TAG_OSC:
            dbg_unpack_print("\t OSC Descriptor, len=%d\n", tag_length);
            ret = tlv_unpack_desc_osc(tag_length, &tlv->list_osc, fp);
            break;
        case TLV_DESC_TAG_MEMORY:
            dbg_unpack_print("\t Memory Descriptor, len=%d\n", tag_length);
            ret = tlv_unpack_desc_memory(tag_length, &tlv->list_memory, fp);
            break;
        case TLV_DESC_TAG_OPTION:
            dbg_unpack_print("\t Option Descriptor, len=%d\n", tag_length);
            ret = tlv_unpack_desc_option(tag_length, &tlv->list_option, fp);
            break;
        case TLV_DESC_TAG_ROLLING_CODE:
            dbg_unpack_print("\t Rolling Code Descriptor, len=%d\n", tag_length);             
            ret = tlv_unpack_desc_rolling_code(tag_length, &tlv->list_rolling_code, fp);                    
            break;
        case TLV_DESC_TAG_VERIFY:
            dbg_unpack_print("\t Verify Descriptor, len=%d\n", tag_length);
            ret = tlv_unpack_desc_verify(tag_length, &tlv->list_verify, fp);
            break;
        case TLV_DESC_TAG_TIME:
            dbg_unpack_print("\t Time Descriptor, len=%d\n", tag_length);
            ret = tlv_unpack_desc_time(tag_length, &tlv->list_time, fp);
            break;
        case TLV_DESC_TAG_PROGRAM:
            dbg_unpack_print("\t Program Descriptor, len=%d\n", tag_length);
            ret = tlv_unpack_desc_program(tag_length, &tlv->list_program, fp);  
            break;
        case TLV_DESC_TAG_CODE:
            dbg_unpack_print("\t Code Descriptor, len=%d\n", tag_length);
            ret = tlv_unpack_desc_code(tag_length, &tlv->list_code, fp);
            break;
        case TLV_DESC_TAG_PROGRAM_LIMIT:
            dbg_unpack_print("\t Program Limit Descriptor, len=%d\n", tag_length);
            ret = tlv_unpack_desc_limit(tag_length, &tlv->list_limit, fp);
            break;        
        case TLV_DESC_TAG_CRYPTO:
            dbg_unpack_print("\t Crypto Descriptor, len=%d\n", tag_length);
            ret = tlv_unpack_desc_crypto(tag_length, &tlv->list_crypto, fp);  
            break;
        case TLV_DESC_TAG_VERSION:
            dbg_unpack_print("\t Version Descriptor, len=%d\n", tag_length);
            ret = tlv_unpack_desc_version(tag_length, &tlv->list_version, fp);
            break;
        default:
            dbg_unpack_print("\t Unknow Descriptor, tag = %d, len=%d\n", tlv_head.tag, tag_length);
            ret = ERR_FILE | ERR_TLV_DESC;            
            break;
        }
        file_length -= tag_length;
        if (ret) {
            ret = ERR_FILE | ERR_TLV_DESC;
            goto ERR_TLV_UNPACK;
        }
    }
    buf = (uint8_t*)&tlv->crc32;
    size = 4;
    len = file_read(buf, size, fp);
    if (len != size) {        
        ret = ERR_FILE | ERR_TLV_DESC;
        err_print("file_read(%p,%d,%p) failed\n", buf, size, fp); 
        goto ERR_TLV_UNPACK;
    }
    
    dbg_unpack_print("\t CRC32=0x%x\n", tlv->crc32);
    
ERR_TLV_UNPACK:
    file_close(fp);

    return ret;
}


int32_t tlv_file_verify(int32_t file_id)
{
    uint32_t                crc32;
    uint32_t                crc32_recv;
    int32_t                 ret;
    int32_t                 size;
    FILE*                   fp;
    uint8_t*                buf;
    
    tlv_file_dump(file_id);

    crc32 = tlv_file_calc_crc32(file_id);
    
    fp = file_open(file_id, "r");
    if (fp == NULL) {
        err_print("file_open(%d,\"r\")\n", file_id);
        return 0;
    }  
    ret = file_seek(fp, -4, SEEK_END);
    if (ret) {
        err_print("file_seek(%p,4,SEEK_END) failed,ret=%d\n", fp, ret);
        goto ERR_TLV_FILE_VERIFY;
    }
    
    buf  = (uint8_t*)&crc32_recv;
    size = sizeof(crc32_recv);
    ret  = file_read(buf, size, fp);
    if (ret != size) {
        err_print("file_read(%p,%d,%p) failed,ret=%d\n", buf, size, fp, ret);
        goto ERR_TLV_FILE_VERIFY;
    }
    file_close(fp);
//    crc32_recv = __REV32(crc32_recv);
    dbg_unpack_print("[File] CRC32=0x%x,file CRC32=0x%x\n", crc32, crc32_recv);
    
    return crc32 == crc32_recv;
    
ERR_TLV_FILE_VERIFY:
   
    file_close(fp);
    return 0;
}



int32_t tlv_file_pack(struct tlv_file* tlv)
{
    int32_t ret;
    void*   fp;

    ret = tlv_file_pack_desc(tlv);
    if (ret) {
        err_print("tlv_file_pack_desc(%p) failed\n",tlv);
        return ERR_FILE;
    }
  
    tlv->crc32 = tlv_file_calc_crc32(tlv->head.file_id); 

    ret = tlv_file_append_crc32(tlv->head.file_id, tlv->crc32);
    if (ret < 0) {
        err_print("tlv_pack_desc_crc32(0x%x,%p) failed\n", tlv->crc32, fp);
        return ERR_FILE;
        
    }

    tlv_file_dump(tlv->head.file_id);

    return ret;
}

