#ifndef __IO_SPI_H__
#define __IO_SPI_H__

#include <stdint.h>

#define IO_SPI_USE_HW          1


#if (IO_SPI_USE_HW == 1) 
int32_t io_spi_init(void);

int32_t io_spi_send_byte(uint8_t data);

int32_t io_spi_send_word(uint16_t word);

int32_t io_spi_read_byte(uint8_t* data);

int32_t io_spi_read_word(uint16_t* word);

int32_t io_spi_send_dummy(uint8_t cycles_num);

#else
static INLINE int32_t io_spi_send_byte(uint8_t data)
{
    uint16_t i;

    for(i = 0; i < 8; i++) {
        CK_PORT &= ~ck_mask;         //low
        if(data & 0x80) {
            DA_PORT |= da_mask;
        } else {
            DA_PORT &= ~da_mask;
        }
        ndelay(1);
        CK_PORT |= ck_mask;             //high
        ndelay(1);
        data <<= 1;
    }

    CK_PORT &= ~ck_mask;         //low

    return ERR_OK;
}



static INLINE void io_spi_send_word(uint16_t data)
{
    uint16_t i;

    for(i = 0; i < 16; i++) {
        CK_PORT &= ~ck_mask;         //low
        if(data & 0x8000) {
            DA_PORT |= da_mask;
        } else {
            DA_PORT &= ~da_mask;
        }
        ndelay(1);
        CK_PORT |= ck_mask;             //high
        ndelay(1);
        data <<= 1;
    }

    CK_PORT &= ~ck_mask;         //low
    return ERR_OK;
}




static INLINE void io_spi_recv_byte(uint8_t channelBuf[CHANNEL_NUM_MAX])
{
    int32_t i;

    MyMemset(channelBuf, 0, sizeof(channelBuf));

    for(i = 7; i >= 0; i-- ) {
        CK_PORT |= ck_mask;             //high
        if (RD_DA_PORT & da_single_mask[0]) {
            channelBuf[CHANNEL_1] |= (1 << i);
        }
        if (RD_DA_PORT & da_single_mask[1]) {
            channelBuf[CHANNEL_2] |= (1 << i);
        }
        if (RD_DA_PORT & da_single_mask[2]) {
            channelBuf[CHANNEL_3] |= ( 1<< i);
        }
        if (RD_DA_PORT & da_single_mask[3]) {
            channelBuf[CHANNEL_4] |= (1 << i);
        }
        CK_PORT &= ~ck_mask;         //low
        ndelay(1);
    }
    CK_PORT &= ~ck_mask;         //low

    return ERR_OK;
}

static INLINE void ioSPIRecvWord(uint16_t channelBuf[CHANNEL_NUM_MAX])
{
    int32_t i;

    MyMemset(channelBuf, 0, sizeof(channelBuf));

    for(i = 15; i >= 0; i-- ) {
        CK_PORT |= ck_mask;             //high
        if(RD_DA_PORT & da_single_mask[0]) {
            channelBuf[CHANNEL_1] |= (1 << i);
        }
        if(RD_DA_PORT & da_single_mask[1]) {
            channelBuf[CHANNEL_2] |= (1 << i);
        }
        if(RD_DA_PORT & da_single_mask[2]) {
            channelBuf[CHANNEL_3] |= ( 1<< i);
        }
        if(RD_DA_PORT & da_single_mask[3]) {
            channelBuf[CHANNEL_4] |= (1 << i);
        }
        CK_PORT &= ~ck_mask;         //low
        ndelay(1);
    }
    CK_PORT &= ~ck_mask;         //low
}

static INLINE void ioSPISendDummyCycle(uint8_t peroidCount)
{
    uint32_t i;

    for(i = 0; i < peroidCount; i++ ) {
        CK_PORT |= ck_mask;             //high
        ndelay(1);
        CK_PORT &= ~ck_mask;         //low
        ndelay(1);
    }

    CK_PORT &= ~ck_mask;         //low

    return ERR_OK;
}
#endif


#endif /* __IO_SPI_H__ */
