#ifndef __TIMING_H__
#define __TIMING_H__

#include <stdint.h>
#include "main.h"
#include "stm32f4xx_ll_gpio.h"
#include "delay.h"
/*
 *  ICSP pin map        CSWrite3.0
 *   Vdd        <------> Vdd
 *   Vpp        <------> Vpp
 *   GND        <------> GND
 *   Vs         <------> Vs
 *   CLK        <------> CLK_A_Pin
 *   DA         <------> MOSI_A_Pin
 */
#define IO_ICSP_DA2                         LL_GPIO_PIN_11
#define IO_ICSP_DA                          LL_GPIO_PIN_12
#define IO_ICSP_CLK                         LL_GPIO_PIN_10
#define IO_OE                               LL_GPIO_PIN_2

#define IO_ICSP_PORT                        GPIOC
#define IO_OE_PORT                          GPIOE

#define IO_ICSP_IN                          LL_GPIO_MODE_INPUT
#define IO_ICSP_OUT                         LL_GPIO_MODE_OUTPUT

#define IO_ICSP_LEVEL_H                     1
#define IO_ICSP_LEVEL_L                     0

#define ID_CRC4                             4
#define ID_ACK4                             4
#define ID_ACK6                             6
#define ID_CMD                              6
#define ID_DATA                             8

#define CMD_LOAD_DATA_FOR_PROG_MEM          0x40
#define CMD_READ_DATA_FROM_PROG_MEM         0x58
#define CMD_BEGIN_PROG                      0x70
#define CMD_BEGIN_READING                   0x44
#define CMD_BEGIN_E2_OP                     0x4C
#define CMD_END                             0x48
#define CMD_ACK                             0x54
#define CMD_READ_CHIP_ID1                   0xC4
#define CMD_READ_CHIP_ID2                   0xC8
#define CMD_READ_CHIP_ID3                   0xCC
#define CMD_READ_CODE_OPTION                0xD0
#define CMD_READ_CHK_HBYTE                  0xD4
#define CMD_READ_CHK_LBYTE                  0xD8
#define CMD_CAL_WDT                         0x84
#define CMD_CAL_OSC_16M                     0x8C
#define CMD_CFG_ADC                         0x20
#define CMD_START_ADC                       0x24
#define CMD_READ_ADC_HBYTE                  0x28
#define CMD_READ_ADC_LBYTE                  0x2C
#define CMD_WRITE_ALL                       0x30
#define CMD_WRITE_CODE_OPTION               0x34
#define CMD_CHIP_RESET                      0xFC
#define CMD_ENTER_PROG_MODE                 0xA5
#define CMD_IC_ACK                          0x15
#define CMD_VERIFY                          0x34


static inline void io_icsp_init(void)
{
//    LL_GPIO_InitTypeDef gpio_def;
//
//    gpio_def.Pin         = MOSI_A_Pin | MISO_A_Pin | CLK_A_Pin;
//    gpio_def.Mode        = LL_GPIO_MODE_OUTPUT;
//    gpio_def.Speed       = LL_GPIO_SPEED_FREQ_LOW;
//    gpio_def.OutputType  = LL_GPIO_OUTPUT_PUSHPULL;
//    gpio_def.Pull        = LL_GPIO_PULL_UP;
//    LL_GPIO_Init(MOSI_A_GPIO_Port, &gpio_def);
   

    LL_GPIO_SetPinMode(IO_ICSP_PORT, IO_ICSP_DA, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(IO_ICSP_PORT, IO_ICSP_DA2, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(IO_ICSP_PORT, IO_ICSP_CLK, LL_GPIO_MODE_OUTPUT);
    
    LL_GPIO_SetPinOutputType(IO_ICSP_PORT, IO_ICSP_DA|IO_ICSP_DA2|IO_ICSP_CLK, LL_GPIO_OUTPUT_PUSHPULL);
    
    LL_GPIO_SetPinSpeed(IO_ICSP_PORT, IO_ICSP_DA,  LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinSpeed(IO_ICSP_PORT, IO_ICSP_DA2,  LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinSpeed(IO_ICSP_PORT, IO_ICSP_CLK, LL_GPIO_SPEED_FREQ_LOW);
    
    LL_GPIO_SetPinPull(IO_ICSP_PORT, IO_ICSP_DA, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinPull(IO_ICSP_PORT, IO_ICSP_DA2, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinPull(IO_ICSP_PORT, IO_ICSP_CLK, LL_GPIO_PULL_UP);

//    LL_GPIO_SetPinPull(IO_ICSP_PORT, IO_ICSP_DA, LL_GPIO_PULL_DOWN);
//    LL_GPIO_SetPinPull(IO_ICSP_PORT, IO_ICSP_DA2, LL_GPIO_PULL_DOWN);
//    LL_GPIO_SetPinPull(IO_ICSP_PORT, IO_ICSP_CLK, LL_GPIO_PULL_DOWN);
    
    LL_GPIO_SetOutputPin(IO_OE_PORT, IO_OE);
}

static inline void io_icsp_da_set_in(void)
{
    LL_GPIO_SetPinOutputType(IO_ICSP_PORT, IO_ICSP_DA, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinPull(IO_ICSP_PORT, IO_ICSP_DA, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinMode(IO_ICSP_PORT, IO_ICSP_DA, IO_ICSP_IN);
}

static inline void io_icsp_da_set_out(void)
{
    LL_GPIO_SetPinOutputType(IO_ICSP_PORT, IO_ICSP_DA, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(IO_ICSP_PORT, IO_ICSP_DA, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinMode(IO_ICSP_PORT, IO_ICSP_DA, IO_ICSP_OUT);
}

static inline void io_icsp_da2_set_in(void)
{
    LL_GPIO_SetPinMode(IO_ICSP_PORT, IO_ICSP_DA2, IO_ICSP_IN);
}

static inline void io_icsp_da2_set_out(void)
{
    LL_GPIO_SetPinMode(IO_ICSP_PORT, IO_ICSP_DA2, IO_ICSP_OUT);
}

static inline void io_icsp_da2_high(void)
{
    LL_GPIO_SetOutputPin(IO_ICSP_PORT, IO_ICSP_DA2);
}

static inline void io_icsp_da_high(void)
{
    LL_GPIO_SetOutputPin(IO_ICSP_PORT, IO_ICSP_DA);
}

static inline void io_icsp_da_toggle(void)
{
    LL_GPIO_TogglePin(IO_ICSP_PORT, IO_ICSP_DA);
}


static inline void io_icsp_test(void)
{
    uint32_t odr;
    
    odr = LL_GPIO_ReadReg(IO_ICSP_PORT, ODR);
    
    odr |= IO_ICSP_DA | IO_ICSP_DA2;
    odr &= (~(IO_ICSP_CLK));
    LL_GPIO_WriteReg(IO_ICSP_PORT, ODR, odr);
    
    io_icsp_delay_us(30);
    
    odr |= (IO_ICSP_CLK | IO_ICSP_DA2);
    odr &= (~(IO_ICSP_DA));
    LL_GPIO_WriteReg(IO_ICSP_PORT, ODR, odr);
    
    odr |= IO_ICSP_DA | IO_ICSP_CLK;
    odr &= (~(IO_ICSP_DA2));
    LL_GPIO_WriteReg(IO_ICSP_PORT, ODR, odr);
    
    io_icsp_delay_us(30);
    
    odr |= (IO_ICSP_CLK);
    odr &= (~(IO_ICSP_DA | IO_ICSP_DA2));
    LL_GPIO_WriteReg(IO_ICSP_PORT, ODR, odr);
   
    io_icsp_delay_us(30); 
    
    odr |= IO_ICSP_DA | IO_ICSP_DA2;
    odr &= (~(IO_ICSP_CLK));
    LL_GPIO_WriteReg(IO_ICSP_PORT, ODR, odr);
    
    io_icsp_delay_us(30);
    
    odr |= (IO_ICSP_DA2);
    odr &= (~(IO_ICSP_DA | IO_ICSP_CLK));
    LL_GPIO_WriteReg(IO_ICSP_PORT, ODR, odr);
    
    io_icsp_delay_us(30);
}

static inline void io_icsp_da2_toggle(void)
{
    LL_GPIO_TogglePin(IO_ICSP_PORT, IO_ICSP_DA2);
}

static inline void io_icsp_clk_toggle(void)
{
    LL_GPIO_TogglePin(IO_ICSP_PORT, IO_ICSP_CLK);
}

static inline void io_icsp_da_low(void)
{
    LL_GPIO_ResetOutputPin(IO_ICSP_PORT, IO_ICSP_DA);
}

static inline void io_icsp_clk_high(void)
{
    LL_GPIO_SetOutputPin(IO_ICSP_PORT, IO_ICSP_CLK);
}

static inline void io_icsp_clk_low(void)
{
    LL_GPIO_ResetOutputPin(IO_ICSP_PORT, IO_ICSP_CLK);
}

static inline int32_t io_icsp_da_read(void)
{
    return LL_GPIO_IsInputPinSet(IO_ICSP_PORT, IO_ICSP_DA);
}

static inline void io_icsp_send(uint8_t data, uint8_t bits_num)
{
    int32_t i;
    uint8_t msb_mask[] = {0x80,  0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

    for (i = 0; i < bits_num; i++) {
        io_icsp_clk_low();
        io_icsp_delay_ns(1);
        if (data & msb_mask[i]) {
            io_icsp_da_high();
        } else {
            io_icsp_da_low();
        }
        io_icsp_clk_high();
        io_icsp_delay_ns(1);
    }
    io_icsp_clk_low();
    io_icsp_da_low();
    delay_us(1);
}

static inline void io_icsp_send_dummy_cycle(uint32_t cycle)
{
    int32_t i;

    for (i = 0; i < cycle; i++) {
        io_icsp_clk_low();
        io_icsp_delay_us(1);
        io_icsp_clk_high();
        io_icsp_delay_us(1);
    }
    io_icsp_clk_low();
}

static inline uint8_t io_icsp_read(uint8_t bits_num)
{
    int32_t i;
    uint8_t value;

    for (i = 0, value = 0; i < bits_num; i++) {
        io_icsp_clk_low();
        io_icsp_delay_ns(1);
        io_icsp_clk_high();
        value <<= 1;
        if (io_icsp_da_read()) {
            value++;
        }
    }
    io_icsp_clk_low();

    return value;
}

#endif /* __TIMING_H__ */
