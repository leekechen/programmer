#include <stdint.h>
#include "tlv.h"
#include "error.h"
#include "list.h"

struct log_node {
    struct list_head node;    
} ;

struct log_head {
    uint32_t            hex_crc32;
    struct tlv_date     day;
    struct tlv_time     time;
    uint8_t             type;
    uint32_t            successed;
    uint32_t            failed;
} ;

struct cswrite_log {
    struct log_head  head;
    struct list_head node_list;
} ;

static struct cswrite_log log;

int32_t cswrite_log_set_hex_time(struct tlv_date* day, struct tlv_time* time)
{
    if (day == NULL || time == NULL) {
        return ERR_SYS_ARGUMENT;
    }
    
    memcpy(&log.head.day, day, sizeof(struct tlv_date));
    memcpy(&log.head.time, time, sizeof(struct tlv_time));
    
    return ERR_OK;
}

int32_t cswrite_log_set_hex_crc32(uint32_t crc32)
{
    log.head.hex_crc32 = crc32;
    
    return ERR_OK;
}

int32_t cswrite_log_add_legacy(uint32_t error)
{
    return ERR_OK;
}

int32_t cswrite_log_add_new(struct tlv_log_new* log)
{
    if (log == NULL) {
        return ERR_SYS_ARGUMENT;
    }

    return ERR_OK;
    
}

uint32_t cswrite_log_get_num_ok(void)
{

    return 0;
}

int32_t cswrite_log_get_num_failed(void)
{

    return 0;
}

int32_t cswrite_log_set_chip(const char* name, uint8_t* id, uint8_t id_length)
{
    return 0;
}

