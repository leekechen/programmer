#include <stdint.h>
#include <stdlib.h>

#include "cmsis_os2.h"
#include "files.h"
#include "file_ic.h"

#include "cswrite_cfg.h"
#include "error.h"
#include "key.h"

struct file_ic_object {
    struct list_head*   list_code;
    struct tlv_code*    code;
    osSemaphoreId_t     sem;
    uint8_t             pos;
    uint8_t             buf[FILE_SEGEMENT_LENGTH];
} ;

static struct file_ic_object file_ic;

int32_t file_ic_init(void)
{
    file_ic.sem = osSemaphoreNew(1, 0, NULL);
    if (file_ic.sem == NULL) {
        return ERR_SYS_OS;
    }

    memset(file_ic.buf, 0, FILE_SEGEMENT_LENGTH);

    file_ic.list_code = NULL;
    
    return ERR_OK;
}

void* file_ic_open(const char* mode)
{
    int32_t i;
    int32_t ret;
    int32_t length;
    struct tlv_node_code* node;
    
    ret = strcmp(mode, "r");
    if (ret == 0) {
        file_ic.pos = 0;
        file_ic.buf[file_ic.pos] = TLV_FILE_ID_IC;
        file_ic.pos++;
        
        length = file_ic_get_size();
        length = __REV32(length);
        memcpy(&file_ic.buf[file_ic.pos], &length, 4);
        file_ic.pos += 4;
        
        if (file_ic.list_code) {
            node = list_entry(file_ic.list_code, struct tlv_node_code, node);
            file_ic.buf[i] = node->desc.head.tag;
            file_ic.pos++;

            length = __REV32(node->desc.head.length);
            memcpy(&file_ic.buf[i], &length, 4);
            file_ic.pos += 4;
            file_ic.code = &node->desc.body;
        }
    }
    
    return &file_ic;
}

int32_t file_ic_set_code(void* fp, struct tlv_code* code)
{
    struct file_ic_object* ic;
    struct tlv_node_code*  node_node;

    ic = (struct file_ic_object*)fp;

    node_node = calloc(1, sizeof(struct tlv_node_code));
    if (node_node == NULL) {
        return ERR_SYS_NOT_ENOUGH_MEMORY;
    }

    INIT_LIST_HEAD(&node_node->node);

    memcpy(&node_node->desc.body, code, sizeof(struct tlv_code));
    node_node->desc.head.tag    = TLV_DESC_TAG_CODE;
    node_node->desc.head.length = code->end - code->start;

    if (ic->list_code == NULL) {
        ic->list_code = &node_node->node;
    } else {
        list_add(&node_node->node, ic->list_code);
    }
    
    return ERR_OK;
}

struct tlv_code* file_ic_get_code(uint32_t addr)
{
    int32_t i;
    struct list_head* pos;
    struct file_ic_object* ic;
    struct tlv_node_code* node;
    
    //ic = (struct file_ic_object*)fp;
    ic = &file_ic;
    if (ic->list_code == NULL) {
        return NULL;
    }
    
    list_for_each(pos, ic->list_code) {
        node = list_entry(pos, struct tlv_node_code, node);
        if (addr >= node->desc.body.start && addr < node->desc.body.end) {           
            return &node->desc.body;
        }
    }
    return NULL;
}

int32_t file_ic_code_write(uint32_t addr, const uint8_t* data, size_t size)
{
    int32_t length;
    struct tlv_code* code;

    length = FILE_SEGEMENT_LENGTH - file_ic.pos;
    while (size > 0) {
        if (addr >= file_ic.code->start && addr < file_ic.code->end) {
            length = length > size ? size : length;

            memcpy(&file_ic.buf[file_ic.pos], data, length);
            file_ic.pos += length;

            if (file_ic.pos == FILE_SEGEMENT_LENGTH) {
                file_ic.pos = 0;
                osSemaphoreRelease(file_ic.sem);
            }
        } else {
            code = file_ic_get_code(addr);
            if (code == NULL) {
                return -1;
            }
            file_ic.code = code;
            length = length > size ? size : length;

            memcpy(&file_ic.buf[file_ic.pos], data, length);
            file_ic.pos += length;

            if (file_ic.pos == FILE_SEGEMENT_LENGTH) {
                file_ic.pos = 0;
                osSemaphoreRelease(file_ic.sem);
            }
        }
    }
    return length;    
}

int32_t file_ic_write(const uint8_t* data, size_t size, void* fp)
{
    return fwrite(data, 1, size, fp);
}

int32_t file_ic_read(uint8_t* buf, size_t size, void* fp)
{
    int32_t ret;
    
    ret = osSemaphoreAcquire(file_ic.sem, 500);
    if (ret) {
        //TODO
        return 0;
    }
    memcpy(buf, file_ic.buf, size);

    file_ic.pos = 0;
    
    return size;
}

int32_t file_ic_close(void* fp)
{

    return fclose(fp);

}

uint32_t file_ic_get_size(void)
{
    struct list_head* pos;
    uint32_t size;
    struct tlv_node_code* node;
    
    size = TLV_FILE_HEAD_LENGTH;

    if (file_ic.list_code == NULL) {
        return size;
    }

    list_for_each(pos, file_ic.list_code) {
        node = list_entry(pos, struct tlv_node_code, node);
        size += TLV_DESC_HEAD_LENGTH;

        size += node->desc.body.end - node->desc.body.start;
        size += 1; /* the size of Checksum type */
        size += 4; /* the size of Checksum      */
    }

    return size;
}


