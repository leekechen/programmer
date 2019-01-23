#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "tlv.h"
#include "crc32_soft.h"

#include "firmware.h"
#include "copy.h"


struct file_firmware {
    struct tlv_file_head        head;
    struct tlv_desc_crypto      crypto;
    struct tlv_desc_version*    ver_list;
    int32_t                     chip_num;
    struct tlv_desc_chip*       chip_list;
    struct tlv_desc_code        code;
    uint32_t                    crc32;
} ;


static struct file_firmware firmware;
static const uint8_t version_type[] = {TLV_DESC_VERSION_TYPE_HW, TLV_DESC_VERSION_TYPE_FM, TLV_DESC_VERSION_TYPE_BOOT, TLV_DESC_VERSION_TYPE_PC};


uint32_t htonl(uint32_t v)
{
    uint8_t* byte;

    byte = (uint8_t*)&v;

    return (byte[0] << 24) | (byte[1] << 16) | (byte[2] << 8) | byte[3]; 
}

void dir_helper(const char* name)
{
    DIR* dir;
    struct dirent* ptr;

    printf("DIR: %s\n", name);
    dir = opendir(name);
    if (dir == NULL) {
        perror(name);
        return;
    }
    
    for (ptr = readdir(dir); ptr != NULL; ptr = readdir(dir)) {
        printf("\t %s\n", ptr->d_name);
    }
    
    closedir(dir);
    
    return;
}

size_t get_file_size(const char* name) 
{
    FILE*           fp;
    size_t          size;

    fp = fopen(name, "rb");
    if (fp == NULL) {
        char buf[512];
        dir_helper(".");
        dir_helper("..");
        dir_helper("../..");
        dir_helper("..\\..\\bin\\");
        getcwd(buf,sizeof(buf));
        dir_helper(buf);
        perror(name);
        return 0;
    }

    size = fseek(fp, 0, SEEK_END);
    if (size < 0) {
        perror(name);
        return 0;
    }
    
    size = ftell(fp);
    if (size < 0) {
        perror(name);
        return 0;
    }
    
    fclose(fp);

    return size;
}

size_t pack_crypto(struct tlv_desc_crypto* crypto)
{
    memset(crypto, 0, sizeof(struct tlv_desc_crypto));
    
    crypto->head.tag    = TLV_DESC_TAG_CRYPTO;
    crypto->head.length = htobe32(2);
    crypto->algorithm   = TLV_DESC_CRYPTO_ALGORITHM_PLAINTEXT;
    crypto->key         = TLV_DESC_CRYPTO_KEY_NONE;    
    
    return sizeof(struct tlv_desc_head) + 2;
}

size_t pack_version(struct tlv_desc_version* ver, uint8_t type, uint32_t value)
{
    memset(ver, 0, sizeof(struct tlv_desc_version));
    
    ver->head.tag    = TLV_DESC_TAG_VERSION;
    ver->head.length = htobe32(5);
    ver->type        = type;
    ver->value       = htobe32(value);  

    return sizeof(struct tlv_desc_head) + 5;
}

size_t pack_chip(struct tlv_desc_chip* chip, const char* name)
{
    size_t len;
    
    memset(chip, 0, sizeof(struct tlv_desc_chip));
    len = strlen(name);
    
    chip->head.tag      = TLV_DESC_TAG_CHIP;
    chip->head.length   = htobe32(len + 1);
    chip->name_length   = len;
    chip->name          = malloc(len);
    if (chip->name == NULL) {
        perror("malloc() failed");
        return 0;
    }

    memcpy(chip->name, name, len);

    return sizeof(struct tlv_desc_head) + len + 1;
}

size_t pack_code(struct tlv_desc_code* code, struct firmware_info* info)
{
    size_t len;
    size_t size;
    FILE*  fp;

    fp = NULL;
    memset(code, 0, sizeof(struct tlv_desc_code));
    len = get_file_size(info->src);
    if (len == 0) {
        goto PACK_CODE_ERROR;
    }
    
    code->head.tag    = TLV_DESC_TAG_CODE;
    code->head.length = htobe32(len + 9);
    code->addr        = htobe32(info->addr);
    code->bytes       = malloc(len);
    if (code->bytes == NULL) { 
        perror("malloc()");
        goto PACK_CODE_ERROR;
    }
    
    fp = fopen(info->src, "rb");
    if (fp == NULL) {  
        perror("fopen()");        
        goto PACK_CODE_ERROR;
    }

    size = fread(code->bytes, 1, len, fp);
    if (size != len) {
        perror("fread()"); 
        goto PACK_CODE_ERROR;
    }
    
    code->checksum_type = TLV_DESC_CHECKSUM_TYPE_CRC32;
    code->checksum      = htobe32(crc32_sw_calc(0xFFFFFFFF, code->bytes, len) ^ 0xFFFFFFFF);
    
    fclose(fp);

    return sizeof(struct tlv_desc_head) + len + 9;

PACK_CODE_ERROR:
    if (code->bytes != NULL) {
        free(code->bytes);
    }

    if (fp != NULL) {
        fclose(fp);
    }
    
    return 0;
}

int32_t pack_file(struct file_firmware* fm, struct firmware_info* info)
{
    int32_t  i;
    size_t   len;
    size_t   size;
    
    len = 0;
    memset(fm, 0, sizeof(struct file_firmware));
    
    fm->head.file_id = TLV_FILE_ID_FIRMWARE;
    size = pack_crypto(&fm->crypto);
    if (size == 0) {
        goto PACK_FILE_ERROR;
    }
    len += size;

    fm->ver_list = calloc(sizeof(struct tlv_desc_version), MAX_VERSION_NUM);
    if (fm->ver_list == NULL) {
        size = 0;
        goto PACK_FILE_ERROR;
    }

    fm->chip_list = calloc(sizeof(struct tlv_desc_chip), info->chip_num);
    if (fm->chip_list == NULL) {
        size = 0;
        goto PACK_FILE_ERROR;
    }

    for (i = 0; i < MAX_VERSION_NUM; i++) {    
        size = pack_version(&fm->ver_list[i], version_type[i], info->version[i]);
        if (size == 0) {
            goto PACK_FILE_ERROR;
        }
        len += size;        
    }    

    for (i = 0; i < info->chip_num; i++) {
        size = pack_chip(&fm->chip_list[i], info->chip_name[i]);
        if (size == 0) {
            goto PACK_FILE_ERROR;
        }
        len += size;    
    }
    fm->chip_num = info->chip_num;

    size = pack_code(&fm->code, info);
    if (size == 0) {
        goto PACK_FILE_ERROR;
    }
    len += size;

    fm->head.file_length = htobe32(len + 4);
    
    return sizeof(struct tlv_file_head) + len + 4;

    
PACK_FILE_ERROR:    
    if (fm->code.bytes != NULL) {
        free(fm->ver_list);
    }
    
    if (fm->ver_list != NULL) {
        free(fm->ver_list);
    }

    if (fm->chip_list != NULL) {
        free(fm->chip_list);
    }
    
    return size;
}

int32_t serialize(struct file_firmware* fm, struct firmware_info* info, size_t file_size)
{
    FILE*    fp_tmp;
    FILE*    fp_dst;
    size_t   len;
    size_t   size;
    int32_t  i;
    int32_t  err;
    uint8_t* buf;

    buf = NULL;
    fp_dst = NULL;
    //fp_tmp = tmpfile();
    fp_tmp = fopen("tmp.bin", "w+");
    if (fp_tmp == NULL) {
        err = -1;
        perror("tmpfile() failed, can't open file");
        goto SERIALIZE_ERROR;
    }
    
    size = fwrite((uint8_t*)&fm->head, 1, sizeof(fm->head), fp_tmp);
    if (size !=  sizeof(fm->head)) {
        err = -1;
        perror("fwrite() failed");
        goto SERIALIZE_ERROR;
    }

    size = fwrite((uint8_t*)&fm->crypto, 1, sizeof(fm->crypto), fp_tmp);
    if (size !=  sizeof(fm->crypto)) {
        err = -1;
        perror("fwrite() failed");
        goto SERIALIZE_ERROR;
    }

    for (i = 0; i < MAX_VERSION_NUM; i++) {
        size = fwrite((uint8_t*)&fm->ver_list[i], 1, sizeof(struct tlv_desc_version), fp_tmp);
        if (size !=  sizeof(struct tlv_desc_version)) {
            err = -1;
            perror("fwrite() failed");
            goto SERIALIZE_ERROR;
        }
    }

    for (i = 0; i < fm->chip_num; i++) {
        size = fwrite((uint8_t*)&fm->chip_list[i].head, 1, sizeof(struct tlv_desc_head), fp_tmp);
        if (size != sizeof(struct tlv_desc_head)) {
            err = -1;
            perror("fwrite() failed");
            goto SERIALIZE_ERROR;
        }
        
        size = fwrite((uint8_t*)&fm->chip_list[i].name_length, 1, 1, fp_tmp);
        if (size != 1) {            
            err = -1;
            perror("fwrite() failed");
            goto SERIALIZE_ERROR;
        }
        len = fm->chip_list[i].name_length;
        size = fwrite((uint8_t*)fm->chip_list[i].name, 1, len, fp_tmp);
        if (size != len) {            
            err = -1;
            perror("fwrite() failed");
            goto SERIALIZE_ERROR;
        }
    }

    size = fwrite((uint8_t*)&fm->code.head, 1, sizeof(struct tlv_desc_head), fp_tmp);
    if (size != sizeof(struct tlv_desc_head)) {        
        err = -1;
        perror("fwrite() failed");
        goto SERIALIZE_ERROR;
    }
    
    size = fwrite((uint8_t*)&fm->code.addr, 1, 4, fp_tmp);
    if (size != 4) {        
        err = -1;
        perror("fwrite() failed");
        goto SERIALIZE_ERROR;
    }

    len = be32toh(fm->code.head.length) - 9;
    size = fwrite((uint8_t*)fm->code.bytes, 1, len, fp_tmp);
    if (size != len) {        
        err = -1;
        perror("fwrite() failed");
        goto SERIALIZE_ERROR;
    }

    size = fwrite((uint8_t*)&fm->code.checksum_type, 1, 1, fp_tmp);
    if (size != 1) {        
        err = -1;
        perror("fwrite() failed");
        goto SERIALIZE_ERROR;
    }

    size = fwrite((uint8_t*)&fm->code.checksum, 1, 4, fp_tmp);
    if (size != 4) {        
        err = -1;
        perror("fwrite() failed");
        goto SERIALIZE_ERROR;
    }

    buf = malloc(len);
    if (buf == NULL) {        
        err = -1;
        perror("malloc() failed");
        goto SERIALIZE_ERROR;
    }

    size = fseek(fp_tmp, 5, SEEK_SET);
    if (size != 0) {        
        err = -1;
        perror("fseek() failed");
        goto SERIALIZE_ERROR;
    }

    len = file_size - sizeof(struct tlv_file_head) - 4;
    size = fread(buf, 1, len, fp_tmp);
    if (size != len) {        
        err = -1;
        perror("fread() failed");
        goto SERIALIZE_ERROR;
    }

    fm->crc32 = htobe32(crc32_sw_calc(0xFFFFFFFF, buf, len) ^ 0xFFFFFFFF); 
    
    size = fwrite((uint8_t*)&fm->crc32, 1, 4, fp_tmp);
    if (size !=  4) {        
        err = -1;
        perror("fwrite() failed");
        goto SERIALIZE_ERROR;
    }

    size = fseek(fp_tmp, 0, SEEK_SET);
    if (size != 0) {        
        err = -1;
        perror("fseek() failed");
        goto SERIALIZE_ERROR;
    }

    fp_dst = fopen(info->dst, "w+");
    if (fp_dst == NULL) {
        err = -1;
        perror("fopen() failed");
        goto SERIALIZE_ERROR;
    }    

    copy_by_fp(fp_dst, fp_tmp);
   
    err = 0;

SERIALIZE_ERROR: 
    
    if (buf != NULL) {
        free(buf);
    }
    
    if (fm->code.bytes != NULL) {
        free(fm->code.bytes);
    }
    
    if (fm->ver_list != NULL) {
        free(fm->ver_list);
    }

    if (fm->chip_list != NULL) {
        free(fm->chip_list);
    }

    if (fp_tmp) {
        fclose(fp_tmp);
        remove("tmp.bin");
    }

    if (fp_dst) {
        fclose(fp_dst);
    }

    return err;
}


int cspack_tlv(struct firmware_info* info) 
{
    size_t  size;
    
    size = pack_file(&firmware, info);
    if (size == 0) {
        return -1;
    }

    size = serialize(&firmware, info, size);
    if (size == 0) {
        return -1;
    }

    return 0;
}

