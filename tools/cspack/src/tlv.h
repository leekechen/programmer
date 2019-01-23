#ifndef __TLV_H__
#define __TLV_H__

#include <stdint.h>

#define __PACKED           __attribute__ ((packed))

#define TLV_FILE_ID_DEV                         0x01
#define TLV_FILE_ID_FIRMWARE                    0x02
#define TLV_FILE_ID_HEX                         0x03
#define TLV_FILE_ID_IC                          0x04
#define TLV_FILE_ID_LOG                         0x05
#define TLV_FILE_ID_CFG                         0x06

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
#define TLV_DESC_TAG_PROGRAM                   (0x0E)

struct tlv_file_head {
    uint8_t     file_id;
    int32_t     file_length;
} __PACKED;



struct tlv_desc_head  {
    uint8_t  tag;
    uint32_t length;
} __PACKED;

struct id_96bits  {
    uint8_t  id[12];
} __PACKED;

struct id_32bits  {
    uint32_t value;
} __PACKED;

struct time {
    uint8_t  year;
    uint8_t  month;
    uint8_t  date;
} ;

struct  tlv_desc_id  {
    struct tlv_desc_head    head;
    uint8_t                 type;
    struct id_32bits        id;
    struct time             active_time;
    uint32_t                count;
} __PACKED;

#define TLV_DESC_VERSION_TYPE_HW        1
#define TLV_DESC_VERSION_TYPE_FM        2
#define TLV_DESC_VERSION_TYPE_BOOT      3
#define TLV_DESC_VERSION_TYPE_PC        4


struct tlv_desc_version {
    struct tlv_desc_head    head;
    uint8_t                 type;
    uint32_t                value;
} __PACKED;


#define TLV_DESC_CRYPTO_ALGORITHM_PLAINTEXT       0
#define TLV_DESC_CRYPTO_KEY_NONE                  0

struct tlv_desc_crypto {
    struct tlv_desc_head    head;
    uint8_t                 algorithm;
    uint8_t                 key;
} __PACKED;

#define TLV_DESC_CHECKSUM_TYPE_CRC32              0x01
#define TLV_DESC_CHECKSUM_TYPE_LEGACY             0x02

struct tlv_desc_code {    
    struct tlv_desc_head    head;
    uint32_t                addr;
    uint8_t                 checksum_type;
    uint32_t                checksum;
    uint8_t*                bytes;
} __PACKED;

struct tlv_desc_voltage {
    struct tlv_desc_head    head;
    uint8_t                 type;
    uint32_t                value;
    uint32_t                offset;
} __PACKED;

struct tlv_desc_osc {
    struct tlv_desc_head    head;
    uint8_t                 type;
    uint32_t                value;
    uint32_t                offset;
} __PACKED;

struct tlv_desc_chip {
    struct tlv_desc_head    head;
    uint8_t                 name_length;
    char*                   name;
    uint8_t                 id_length;
    uint8_t*                id;
    uint8_t                 type_length;
    uint8_t*                type;
} __PACKED;


struct tlv_desc_memory {
    struct tlv_desc_head    head;
    uint8_t                 type;
    uint8_t                 width;
    uint32_t                addr;
    uint32_t                size;
} __PACKED;

struct tlv_desc_option {
    struct tlv_desc_head    head;
    uint32_t                addr;
} __PACKED;

struct tlv_desc_rolling_code {
    struct tlv_desc_head    head;
    uint8_t                 step;
    uint32_t                addr;
    uint8_t                 width;
    uint8_t*                bytes;                    
} ;

struct tlv_desc_verify {
    struct tlv_desc_head    head;
    uint32_t                addr;
    uint32_t                size;
} __PACKED;

struct tlv_desc_time {
    struct tlv_desc_head    head;
    uint8_t                 type;
    uint8_t                 year;
    uint8_t                 month;
    uint8_t                 date;
    uint8_t                 hour;
    uint8_t                 minutes;
} __PACKED;

struct tlv_desc_log {
    struct tlv_desc_head    head;
    uint8_t                 type;
    uint8_t                 error;
} __PACKED;

struct tlv_desc_program {    
    struct tlv_desc_head    head;
    uint8_t                 program;
} __PACKED;

#endif /* __TLV_H__ */
