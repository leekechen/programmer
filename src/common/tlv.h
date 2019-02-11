#ifndef __TLV_H__
#define __TLV_H__

#include <stdint.h>
#include <stdlib.h>

#include "list.h"
#include "common.h"


#define TLV_FILE_ID_DEV                         0x01
#define TLV_FILE_ID_FIRMWARE                    0x02
#define TLV_FILE_ID_HEX                         0x03
#define TLV_FILE_ID_IC                          0x04
#define TLV_FILE_ID_LOG                         0x05
#define TLV_FILE_ID_CFG                         0x06
#define TLV_FILE_ID_MAX                         (TLV_FILE_ID_CFG)

#define FILE_SEGEMENT_LENGTH                    (246) 


#define TLV_FILE_HEAD_LENGTH                   (5)

#define TLV_FILE_CRC32_LENGTH                  (4)

struct tlv_file_head {
    uint8_t     file_id;
    int32_t     file_length;
} ;

#define TLV_DESC_TAG_ID                        (0x01)
#define TLV_DESC_TAG_VERSION                   (0x02)
#define TLV_DESC_TAG_CRYPTO                    (0x03)
#define TLV_DESC_TAG_CODE                      (0x04)
#define TLV_DESC_TAG_VOLTAGE                   (0x05)
#define TLV_DESC_TAG_OSC                       (0x06)
#define TLV_DESC_TAG_CHIP                      (0x07)
#define TLV_DESC_TAG_MEMORY                    (0x08)
#define TLV_DESC_TAG_OPTION                    (0x09)
#define TLV_DESC_TAG_ROLLING_CODE              (0x0A)
#define TLV_DESC_TAG_VERIFY                    (0x0B)
#define TLV_DESC_TAG_TIME                      (0x0C)
#define TLV_DESC_TAG_LOG                       (0x0D)
#define TLV_DESC_TAG_PROGRAM                   (0x0E)
#define TLV_DESC_TAG_PROGRAM_LIMIT             (0x0F)

#define TLV_DESC_HEAD_LENGTH                   (5)

struct tlv_desc_head  {
    uint8_t  tag;
    uint32_t length;
} ;

#define TLV_DESC_HEAD_INIT(p, t, l)            do {                                             \
                                                    (p)->tag    = (t);                          \
                                                    (p)->length = (l) + TLV_DESC_HEAD_LENGTH;   \
                                               } while (0);

#define TLV_DATE_LENGTH                        (3)

struct tlv_date {
    uint8_t  year;
    uint8_t  month;
    uint8_t  date;
}  ;

#define TLV_DESC_ID_BODY_LENGTH                (12)


#define TLV_DESC_SERIAL_ID_TYPE_96_BITS        (1)
#define TLV_DESC_SERIAL_ID_TYPE_32_BITS        (2)
#define TLV_DESC_SERIAL_ID_MAX_SIZE            (3)

struct tlv_id {
    uint8_t                                     type;
    uint32_t                                    serial_id;
    struct tlv_date                             day;
    uint32_t                                    count;
} ;

#define TLV_DESC_ID_LENGTH                      (TLV_DESC_HEAD_LENGTH + TLV_DESC_ID_BODY_LENGTH)

struct  tlv_desc_id  {
    struct tlv_desc_head    head;
    struct tlv_id           body;
} ;

#define TLV_DESC_VERSION_TYPE_HW                1
#define TLV_DESC_VERSION_TYPE_FM                2
#define TLV_DESC_VERSION_TYPE_BOOT              3
#define TLV_DESC_VERSION_TYPE_PC                4

#define TLV_DESC_VERSION_BODY_LENGTH            (5)

struct tlv_version {
    uint8_t                                      type;
    uint32_t                                     value;
} ;

#define TLV_DESC_VERSION_LENGTH                 (TLV_DESC_HEAD_LENGTH + TLV_DESC_VERSION_BODY_LENGTH)

struct tlv_desc_version {
    struct tlv_desc_head                         head;
    struct tlv_version                           body;
} ;


#define TLV_DESC_CRYPTO_ALGORITHM_PLAINTEXT       0
#define TLV_DESC_CRYPTO_KEY_NONE                  0

#define TLV_DESC_CRYPTO_BODY_LENGTH              (2)

struct tlv_crypto {
    uint8_t                                      algorithm;
    uint8_t                                      key;
} ;

#define TLV_DESC_CRYPTO_LENGTH                   (TLV_DESC_HEAD_LENGTH + TLV_DESC_VERSION_BODY_LENGTH)

struct tlv_desc_crypto {
    struct tlv_desc_head                          head;
    struct tlv_crypto                             body;
} ;

#define TLV_DESC_CHECKSUM_TYPE_CRC32              0x01
#define TLV_DESC_CHECKSUM_TYPE_LEGACY             0x02

#define TLV_DESC_CODE_STUFF_LENGTH                (TLV_DESC_HEAD_LENGTH + 4 + 5)

struct tlv_code {
    uint32_t                        addr;
    uint32_t                        start;
    uint32_t                        end;
    uint8_t                         checksum_type;
    uint32_t                        checksum;
} ;

struct tlv_desc_code {
    struct tlv_desc_head            head;
    struct tlv_code                 body;
} ;

#define TLV_DESC_VOLTAGE_TYPE_VDD   0x01
#define TLV_DESC_VOLTAGE_TYPE_VPP   0x02
#define TLV_DESC_VOLTAGE_TYPE_MAX   TLV_DESC_VOLTAGE_TYPE_VPP

struct tlv_voltage {
    uint8_t                         type;
    uint32_t                        value;
    uint32_t                        offset;
} ;

#define TLV_DESC_VOLTAGE_LENGTH     (TLV_DESC_HEAD_LENGTH + 9)


struct tlv_desc_voltage {
    struct tlv_desc_head            head;
    struct tlv_voltage              body;
} ;

#define TLV_DESC_OSC_TYPE_OSC       1
#define TLV_DESC_OSC_TYPE_WDT       2
#define TLV_DESC_OSC_TYPE_MAX       TLV_DESC_OSC_TYPE_WDT

struct tlv_osc {
    uint8_t                         type;
    uint32_t                        value;
    uint32_t                        offset;
    uint32_t                        addr;
} ;

#define TLV_DESC_OSC_LENGTH         (TLV_DESC_HEAD_LENGTH + 13)


struct tlv_desc_osc {
    struct tlv_desc_head            head;
    struct tlv_osc                  body;
} ;

#define TLV_DESC_MAX_LENGTH_NAME    64
#define TLV_DESC_MAX_LENGTH_ID      64

struct tlv_chip {
    uint8_t                         name_length;
    uint8_t*                        name;
    uint8_t                         id_length;
    uint8_t*                        id;
    uint8_t                         type_length;
    uint8_t*                        type;
} ;

struct tlv_desc_chip {
    struct tlv_desc_head            head;
    struct tlv_chip                 body;
} ;

#define TLV_DESC_MEMORY_TYPE_OTP    1
#define TLV_DESC_MEMORY_TYPE_MTP    2
#define TLV_DESC_MEMORY_TYPE_FLASH  3
#define TLV_DESC_MEMORY_TYPE_REG    4
#define TLV_DESC_MEMORY_TYPE_EEPROM 5

struct tlv_memory {
    uint8_t                         type;
    uint8_t                         width;
    uint32_t                        addr;
    uint32_t                        size;
} ;

#define TLV_DESC_MEMORY_LENGTH      (TLV_DESC_HEAD_LENGTH + 10)

struct tlv_desc_memory {
    struct tlv_desc_head            head;
    struct tlv_memory               body;
} ;

#define TLV_DESC_OPTION_TYPE_NO_ADDR     1
#define TLV_DESC_OPTION_TYPE_ADDR        2

struct tlv_option {
    uint8_t                         type;
    uint8_t                         option_len;
    uint32_t                        addr;
    uint8_t*                        bytes;
} ;

struct tlv_desc_option {
    struct tlv_desc_head            head;
    struct tlv_option               body;
} ;

struct tlv_rolling_code {
    uint8_t                         step;
    uint8_t                         width;
    uint32_t                        addr;
    uint32_t                        value;
} ;


struct tlv_desc_rolling_code {
    struct tlv_desc_head            head;
    struct tlv_rolling_code         body;
} ;


struct tlv_verify {
    uint32_t                        addr;
    uint32_t                        size;
} ;

#define TLV_DESC_VERIFY_LENGTH      (TLV_DESC_HEAD_LENGTH + 8)

struct tlv_desc_verify {
    struct tlv_desc_head            head;
    struct tlv_verify               body;
} ;

#define TLV_TIME_LENGTH            (2)

struct tlv_time {
    uint8_t                         hour;
    uint8_t                         minutes;
} ;

#define TLV_DESC_TIME_TYPE_YYMMDDhhmm       (1)
#define TLV_DESC_TIME_LENGTH       (TLV_DESC_HEAD_LENGTH + TLV_TIME_LENGTH + TLV_DATE_LENGTH + 1)

struct tlv_desc_time {
    struct tlv_desc_head            head;
    uint8_t                         type;
    struct tlv_date                 day;
    struct tlv_time                 time;
} ;

#define TLV_DESC_LOG_TYPE_LEGACY    1
#define TLV_DESC_LOG_TYPE_NEW       2

#define TLV_SERIAL_ID_96_BITS_LEN   (96 / 8)

struct tlv_log_legacy {
    uint32_t                        error;
} ;

struct tlv_log_new {
    uint8_t                         serial_id[TLV_SERIAL_ID_96_BITS_LEN];
    uint32_t                        error;
} ;

#define TLV_DESC_LOG_LEGACY_LENGTH  (TLV_DESC_HEAD_LENGTH + 5)
#define TLV_DESC_LOG_NEW_LENGTH     (TLV_DESC_HEAD_LENGTH + 17)


struct tlv_desc_log {
    struct tlv_desc_head            head;
    void*                           body;
} ;

#define TLV_DESC_PROGRAM_LENGTH     (TLV_DESC_HEAD_LENGTH + 1)

#define TLV_PROGRAM_BLANK_CHECK     (1 << 0)
#define TLV_PROGRAM_ERASE           (1 << 1)
#define TLV_PROGRAM_PROGRAM         (1 << 2)
#define TLV_PROGRAM_VERIFY          (1 << 3)
#define TLV_PROGRAM_READ            (1 << 4)
#define TLV_PROGRAM_UPGRADE_HEX     (1 << 5)
#define TLV_PROGRAM_ACTIVE          (1 << 7)

struct tlv_program {
    uint8_t                         bitmap;
} ;

#define TLV_DESC_PROGRAM_LENGTH     (TLV_DESC_HEAD_LENGTH + 1)

struct tlv_desc_program {
    struct tlv_desc_head            head;
    struct tlv_program              body;
} ;

struct tlv_limit {
    uint32_t                        count;
} ;

#define TLV_DESC_LIMIT_LENGTH      (TLV_DESC_HEAD_LENGTH + 4)

struct tlv_desc_limit {
    struct tlv_desc_head            head;
    struct tlv_limit                body;
} ;

struct tlv_program_status {
    uint32_t                        ok;
    uint32_t                        failed;
} ; 

#define TLV_DESC_PROGRAM_STATUS_LENGTH  (TLV_DESC_HEAD_LENGTH + 8)

struct tlv_desc_program_status {
    struct tlv_desc_head            head;
    struct tlv_program_status       body;
} ;


struct tlv_node_id {
    struct list_head                node;
    struct tlv_desc_id              desc;
} ;

struct tlv_node_chip {
    struct list_head                node;
    struct tlv_desc_chip            desc;
} ;

struct tlv_node_voltage {
    struct list_head                node;
    struct tlv_desc_voltage         desc;
} ;

struct tlv_node_osc {
    struct list_head                node;
    struct tlv_desc_osc             desc;
} ;

struct tlv_node_code {
    struct list_head                node;
    struct tlv_desc_code            desc;
} ;

struct tlv_node_memory {
    struct list_head                node;
    struct tlv_desc_memory          desc;
} ;

struct tlv_node_option {
    struct list_head                node;
    struct tlv_desc_option          desc;
} ;

struct tlv_node_verify {
    struct list_head                node;
    struct tlv_desc_verify          desc;
} ;

struct tlv_node_time {
    struct list_head                node;
    struct tlv_desc_time            desc;
} ;

struct tlv_node_crypto {
    struct list_head                node;
    struct tlv_desc_crypto          desc;
} ;

struct tlv_node_version {
    struct list_head                node;
    struct tlv_desc_version         desc;
} ;

struct tlv_node_program {
    struct list_head                node;
    struct tlv_desc_program         desc;
} ;

struct tlv_node_limit {
    struct list_head                node;
    struct tlv_desc_limit           desc;
} ;

struct tlv_node_rolling_code {
    struct list_head                node;
    struct tlv_desc_rolling_code    desc;
} ;

struct tlv_node_program_status {
    struct list_head                node;
    struct tlv_desc_program_status  desc;
} ;


struct tlv_file {
    struct tlv_file_head            head; 
    struct list_head                list_id;
    struct list_head                list_version;
    struct list_head                list_crypto;
    struct list_head                list_code;
    struct list_head                list_voltage;
    struct list_head                list_osc;
    struct list_head                list_chip;
    struct list_head                list_memory;
    struct list_head                list_option;
    struct list_head                list_rolling_code;
    struct list_head                list_verify;
    struct list_head                list_time;
    struct list_head                list_log;
    struct list_head                list_program;
    struct list_head                list_limit;
    struct list_head                list_program_status;
    uint32_t                        crc32;
} ;


static inline void* tlv_desc_new(size_t size)
{
    void* buf;

    buf = calloc(1, size);
    if (buf == NULL) {
        return NULL;
    }

    return buf;
}

static inline void tlv_desc_free(void* buf)
{
    free(buf);
}

static inline void tlv_desc_add(struct list_head* list, struct list_head* new)
{
    list_add(new, list);
}

static inline void tlv_desc_del(struct list_head* node)
{
    list_del(node);  
}


int32_t tlv_file_init(struct tlv_file* tlv);
int32_t tlv_file_destroy(struct tlv_file* tlv);
int32_t tlv_file_unpack(int32_t file_id, struct tlv_file* tlv);
int32_t tlv_file_verify(int32_t file_id);
int32_t tlv_file_pack(struct tlv_file* tlv);

#endif /* __TLV_H__ */









