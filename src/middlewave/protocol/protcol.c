#include <string.h>
#include <cmsis_os2.h>

#include "error.h"
#include "hid.h"
#include "common.h"
#include "protcol.h"
#include "files.h"
#include "tlv.h"


#define MODULE_DEBUG       0 
#define MODULE_ERROR       1

#include "dbg_print.h"


#define SYNC_HOST                                                0xA5
#define SYNC_TARGET                                              0x5A
                                                                 
#define CMD_HANDE_SHAKE                                          0x02
#define CMD_REQUEST_WRITE                                        0x27
#define CMD_REQUEST_SEND                                         0x28
#define CMD_REQUEST_READ                                         0x29
#define CMD_REQUEST_RECV                                         0x2A
                                                                 
#define CMD_REPLY_HANDE_SHAKE                                    0x80
#define CMD_REPLY_WRITE                                          0x89
#define CMD_REPLY_SEND                                           0x8A
#define CMD_REPLY_READ                                           0x90
#define CMD_REPLY_RECV                                           0x91


#define FILE_WRITE                                               0
#define FILE_READ                                                1


#define PACKET_MAX_LENGTH                                        0x0100


struct packet {
    uint8_t     sync;
    uint8_t     cmd;
    uint16_t    length;
    uint8_t     body[PACKET_MAX_LENGTH];
} ;

struct file_info {
    uint8_t             id;
    int32_t             length;
    uint32_t            next_segment_id;
    void*               fp;
} ;

static struct packet        tx_pkt;
static struct packet        rx_pkt;
static struct file_info     file_list[TLV_FILE_ID_MAX];


static uint8_t calc_xor_sum(const uint8_t* data, size_t len)
{
    size_t i;
    uint8_t xorSum = 0;

    for (i = 0; i < len; i++) {
        xorSum ^= data[i];
    }
    
    return xorSum;
}

static struct file_info* get_file(uint32_t file_id)
{
    if (file_id == 0 || file_id > TLV_FILE_ID_MAX) {
        return NULL;
    }

    return &file_list[file_id - 1];
}

static int32_t pack(struct packet* pkt, size_t body_size)
{
    uint16_t    pkt_size;
    uint8_t     xor_sum;
    uint16_t    xor_sum_pos;

    /* 5 bytes packet head: 1 byte sync + 1 byte cmd + 2 byte length + 1 byte xor_sum */
    pkt_size    = body_size + 5;
    
    pkt->sync   = SYNC_TARGET;
    pkt->length = __REV16(pkt_size);

    xor_sum_pos = body_size;
    xor_sum     = calc_xor_sum((const uint8_t*)pkt, pkt_size - 1);
    pkt->body[xor_sum_pos] = xor_sum; 
    
    return pkt_size;
}

static int32_t reply_hande_shake(struct packet* pkt)
{
    int32_t size;
    int32_t pkt_len;

    memset(pkt, 0, sizeof(struct packet));
    
    pkt->cmd     = CMD_REPLY_HANDE_SHAKE;    
    pkt->body[0] = 0x22;
    pkt->body[1] = 0x33;

    pkt_len = pack(pkt, 2);

    size = hid_write((uint8_t*)pkt, pkt_len);
    if (size != pkt_len) {
        err_print("[Protocol] hid_write() failed!\n");
        return ERR_DRV_HID;
    }

    return ERR_OK;
}

static int32_t reply_write(struct packet* pkt, uint8_t file_id, int32_t err)
{   
    int32_t size;
    int32_t len;

    err          = __REV32(err);    
    pkt->cmd     = CMD_REPLY_WRITE;    
    pkt->body[0] = file_id;

    memcpy(&pkt->body[1], &err, 4);

    len = pack(pkt, 5);

    size = hid_write((uint8_t*)pkt, len);
    if (size != len) {
        err_print("[Protocol] hid_write() failed!\n");
        return ERR_DRV_HID;
    }

    return ERR_OK; 
}

static int32_t reply_send(struct packet* pkt, uint8_t file_id, int32_t err)
{   
    int32_t size;
    int32_t len;

    err          = __REV32(err);
    pkt->cmd     = CMD_REPLY_SEND;    
    pkt->body[0] = file_id;

    memcpy(&pkt->body[1], &err, 4);

    len = pack(pkt, 5);

    size = hid_write((uint8_t*)pkt, len);
    if (size != len) {
        err_print("[Protocol] hid_write() failed!\n");
        return ERR_DRV_HID;
    }

    return ERR_OK; 
}

static int32_t reply_read(struct packet* pkt, uint8_t file_id, int32_t length)
{   
    int32_t size;
    int32_t len;

    pkt->cmd     = CMD_REPLY_READ;    
    pkt->body[0] = file_id;
    length       = __REV32(length);
    memcpy(&pkt->body[1], &length, 4);

    len = pack(pkt, 5);

    size = hid_write((uint8_t*)pkt, len);
    if (size != len) {
        err_print("[Protocol] hid_write() failed!\n");
        return ERR_DRV_HID;
    }

    return ERR_OK; 
}

static int32_t reply_recv(struct packet* pkt, uint8_t file_id, int32_t segment_id, int32_t segment_len)
{
    int32_t size;
    int32_t body_len;
    int32_t pkt_len;

    /* 5 bytes head: 1 byte file_id and 4 byte segment_id */
    body_len     = segment_len + 5;
    segment_id   = __REV32(segment_id);
    pkt->cmd     = CMD_REPLY_RECV;
    
    pkt->body[0] = file_id;
    memcpy(&pkt->body[1], &segment_id, 4);

    pkt_len = pack(pkt, body_len);

    size = hid_write((uint8_t*)pkt, pkt_len);
    if (size != pkt_len) {
        err_print("[Protocol] hid_write() failed!\n");
        return ERR_DRV_HID;
    }
    
    return ERR_OK;
}

static int32_t request_hande_shake(const uint8_t* data, size_t size)
{
    dbg_print("[Protocol] Got a hande shake packet\n");
    return reply_hande_shake(&tx_pkt);
}

static int32_t request_write_parser(const uint8_t* data, size_t size)
{
    int32_t  err = ERR_OK;
    uint32_t length;
    uint8_t  file_id;
    struct file_info* file;
    
    dbg_print("[Protocol] Got a write request packet\n");

    memset(&tx_pkt, 0, sizeof(tx_pkt));
   
    if (size != 5) {
        err = ERR_TLV_PKT_SYNTAX; 
        err_print("size(%d)!=5\n", size);
        goto ERR_REQUEST_WRITE_PARSER;
    }
    
    file_id = data[0];
    length  = byte2word(data[1], data[2], data[3], data[4]);
    
    dbg_print("[Protocol] file_id=%d, length=%d\n", file_id, length);
    file = get_file(file_id);
    if (file == NULL) {
        err = ERR_TLV_PKT_SYNTAX; 
        err_print("get_file() failed\n");
        goto ERR_REQUEST_WRITE_PARSER;
    }

    if (file->length > 0) {
        err_print("last operation don't finished!\n");
    }

    memset(file, 0, sizeof(struct file_info));
    
    file->fp = file_open(file_id, "w");
    if (file->fp == NULL) {
        err = ERR_TLV_PKT_SYNTAX; 
        err_print("file_open() failed\n");
        goto ERR_REQUEST_WRITE_PARSER;
    }
                  
    file->length = length;    
    
ERR_REQUEST_WRITE_PARSER:

    return reply_write(&tx_pkt, file_id, err);
}

static int32_t request_send_parser(const uint8_t* data, size_t size)
{
    int32_t  ret;
    uint8_t  file_id;
    uint32_t segment_id;
    int32_t  length;
    struct   file_info* file;

    memset(&tx_pkt, 0, sizeof(tx_pkt));
        
    file_id    = data[0];
    segment_id = byte2word(data[1], data[2], data[3], data[4]);
    
    dbg_print("[Protocol] Got a send request packet, file_id=%d, segment_id=%d\n", file_id, segment_id);

    file = get_file(file_id);
    if (file == NULL) {
        ret = ERR_TLV_FILE_ID;
        err_print("Invalid file_id=%d\n", file_id);
        goto REQUEST_SEND_ERROR;
    }

    if (file->next_segment_id != segment_id) {
        ret = ERR_TLV_FILE_SEGMENT_ID;
        err_print("Invalid segment_id=%d, wanna=%d\n", segment_id, file->next_segment_id);
        goto REQUEST_SEND_ERROR;
    }

    if (file->length > 0) {      
        length = file->length > FILE_SEGEMENT_LENGTH ? FILE_SEGEMENT_LENGTH : file->length;
        ret = file_write(&data[5], length, file->fp);
        if (ret != length) {
            goto REQUEST_SEND_ERROR;
        }
        file->length -= length;
        file->next_segment_id = segment_id + 1;
    } else {
        ret = ERR_TLV_FILE_LENGTH;
        err_print("Invalid file length=%d\n", file->length);
        goto REQUEST_SEND_ERROR;
    }

    if (file->length <= 0) {
        dbg_print("[Protocol] file send finished\n");
        file->next_segment_id = 0;
        ret = file_close(file->fp);
        if (ret) {
            err_print("file_close(%p) failed\n", file->fp);
        }

        ret = file_verify(file_id);
        if (ret) {        
            ret = file_upgrade(file_id);
            if (ret) {
                goto REQUEST_SEND_ERROR;
            }

            ret = err_get_code();
            dbg_print("%s(),ret=0x%x\n", __func__, ret);
        }
    }

REQUEST_SEND_ERROR:
    ret = reply_send(&tx_pkt, file_id, ret);

    return ret;
}

static int32_t request_read_parser(const uint8_t* data, size_t size)
{
    int32_t  err;
    uint8_t  file_id;
    struct   file_info* file;
    
    dbg_print("[Protocol] Got a read request packet\n");

    memset(&tx_pkt, 0, sizeof(tx_pkt));

    err     = 0;    
    file_id = data[0];

    file = get_file(file_id);
    if (file == NULL) {
        goto REQUEST_READ_ERROR;
    } 

    if (file->length > 0) {
        err_print("last operation don't finished!\n");
    }
    
    memset(file, 0, sizeof(struct file_info));

    file->length = file_get_size(file_id);
    err = file->length;
    file->fp = file_open(file_id, "r");
    if (file->fp == NULL) {
        goto REQUEST_READ_ERROR;
    }    
    
 REQUEST_READ_ERROR:   
    
    return reply_read(&tx_pkt, file_id, err);
}

static int32_t request_recv_parser(const uint8_t* data, size_t size)
{
    int32_t  ret;
    uint8_t  file_id;
    size_t   len;
    uint32_t segment_id;
    uint32_t file_length;
    struct   file_info* file;

    dbg_print("[Protocol] Got a reccv request packet\n");

    memset(&tx_pkt, 0, sizeof(tx_pkt));

    ret     = ERR_OK;    
    file_id = data[0];    

    file    = get_file(file_id);
    if (file == NULL) {
        ret = ERR_TLV_FILE_ID;
        goto REQUEST_RECV_ERROR;
    }

    segment_id  = file->next_segment_id;
    file_length = file->length;
    
    len         = file_length > FILE_SEGEMENT_LENGTH ? FILE_SEGEMENT_LENGTH : file_length;

    if (file->length > 0) {   
        ret = file_read(&tx_pkt.body[5], len, file->fp);
        if (ret != len) {
            err_print("Error, file_read() ret=%d\n", ret);
            ret = 0;
            goto REQUEST_RECV_ERROR;
        }        
        
        dbg_print("[Protocol] reply_recv(), file_id=%d, segment_id=%d, length=%d\n", file_id, segment_id, ret); 
        
        ret = reply_recv(&tx_pkt, file_id, segment_id, ret);
        if (ret == ERR_OK) {
            file->length -= len;
            file->next_segment_id++;
            dbg_print("[Protocol] file_id=%d, left length=%d, next_segment_id=%d\n", file_id, file->length, file->next_segment_id);
        }
    }
    if (file->length <= 0) {
        dbg_print("[Protocol] file recv finished\n");
        file->next_segment_id = 0;
        ret = file_close(file->fp);
    }

REQUEST_RECV_ERROR:

    return ret;
}

static int32_t unpack(const struct packet* pkt, size_t len)
{
    uint8_t  xor_sum;
    uint8_t  xor_sum_pos;
    uint16_t pkt_len;
    uint16_t body_len;
    uint16_t recv_len;
    uint8_t* buf;
    
    if (pkt->sync != SYNC_HOST) {
        return ERR_PC_DN;
    }

    pkt_len = __REV16(pkt->length);
    if (pkt_len > PACKET_MAX_LENGTH) {
        return ERR_PC_DN;
    }
    
    /* 4: 1 byte Sync, 1 byte Cmd, 2 bytes Length */
    recv_len = len - 4;
    for (body_len = pkt_len - 4; body_len > recv_len; recv_len += len) {   
        buf = (uint8_t*)&pkt->body[recv_len];
        len = hid_read(buf, PACKET_MAX_LENGTH - recv_len);
        if (len == 0) {
            err_print("[Protocol] hid_read() failed, errno=%d\n", len);
            return ERR_PC_DN;
        } 
    }

    /* 1: 1 byte xor sum */    
    body_len   -= 1;
    xor_sum_pos = body_len;
    /* 1: xor sum don't include self, and include all of others */
    xor_sum     = calc_xor_sum((uint8_t*)pkt, pkt_len - 1);
    if (xor_sum != pkt->body[xor_sum_pos]) { 
        err_print("[Protocol] Error, 0x%x!(Calc)=0x%x(recv)\n", xor_sum, pkt->body[xor_sum_pos]);
        return ERR_PC_DN;
    }

    switch (pkt->cmd) {
    case CMD_HANDE_SHAKE:
        return request_hande_shake(pkt->body, body_len);
    case CMD_REQUEST_WRITE:
        return request_write_parser(pkt->body, body_len);
    case CMD_REQUEST_SEND:
        return request_send_parser(pkt->body, body_len);
    case CMD_REQUEST_READ: 
        return request_read_parser(pkt->body, body_len);
    case CMD_REQUEST_RECV: 
        return request_recv_parser(pkt->body, body_len);
    default:
        err_print("[Protocol] Error! CMD=0x%x, unsported!\n", pkt->cmd);
        dbg_dump("[Protocol] packet dump", (uint8_t*)pkt, body_len);
        break;
    }
    
    return ERR_OK;
}

void thread_protocol(void* arg)
{
    int32_t len;
    int32_t err;
    
    while (1) {
        len = hid_read((uint8_t*)&rx_pkt, sizeof(rx_pkt));
        if (len == 0) {
            err_print("[USB] hid_read() failed, errno=%d\n", len);
            continue;
        }
        
        err = unpack(&rx_pkt, len);
        if (err) {
            err_print("[USB] unpack() packet failed, errno=%d\n", err);
            continue;
        }
    }
}

int32_t protcol_init(void)
{     
    int32_t err;
    osThreadId_t thread_id_protocol;
 
    thread_id_protocol = osThreadNew(thread_protocol, NULL, NULL);
    if (thread_id_protocol == NULL) {
        return ERR_SYS_OS;
    } 
    
    err = usb_hid_init();
    if (err) {
        err_print("usb_hid_init() failed, errno=%d\n", err);
        return ERR_DRV_HID;
    }  
    
    return ERR_OK;
}

