#include <Driver_I2C.h>
#include "error.h"
#include "mb85rcxx.h"
#include "i2c.h"


#define MB85RC16V_ENABLE                1     

/* The Device Type Code of MB85RC16V and MB85RC64V are both "1010" */
#define DEV_TYPE_CODE_MB85RCXX          (0x0A)
/* The I2C Address Pin A0/A1/A2 */
#define DEV_ADDR_CODE_MB85RC64V         (0x00)
/* The I2C Address */
#define DEV_ADDR_CODE_MB85RC16V(a)      (((a) >> 8) & 0x07)

#define READ_CDOE                       (0x01)
#define WRITE_CODE                      (0x00)


//#define DEV_ADDR_WORD(t, a, rw)         ((((t)<<4) | ((a) << 1) | (rw)) & 0xFF)

#if (MB85RC64V_ENABLE == 1)
#define DEV_ADDR_WORD(t, a, rw)         ((((t)<<4) | ((DEV_ADDR_CODE_MB85RC64V) << 1) | (rw)) & 0xFF)
#elif (MB85RC16V_ENABLE == 1)
#define DEV_ADDR_WORD(t, a, rw)         ((((t)<<4) | ((DEV_ADDR_CODE_MB85RC16V(a)) << 1) | (rw)) & 0xFF)
#endif

extern ARM_DRIVER_I2C Driver_I2C1;
extern ARM_DRIVER_I2C driver_i2c1_sw;

static const ARM_DRIVER_I2C* i2c = &driver_i2c1_sw;

//static void pack_frame(uint8_t* frame_buf, uint32_t addr, uint8_t data)
//{
//    frame_buf[0] = DEV_ADDR_WORD(DEV_TYPE_CODE_MB85RCXX, addr, WRITE_CODE);
//    frame_buf[1] = (addr & 0xFF00) >> 8;
//    frame_buf[2] = addr & 0xFF;
//    frame_buf[3] = data;
//}

//static int32_t mb85rcxx_reset_sw(void)
//{
//    return ERR_OK;
//}

//static int32_t mb85rcxx_write_byte(uint32_t addr, uint8_t data)
//{
//    uint8_t frame[4];

//    frame[0] = DEV_ADDR_WORD(DEV_TYPE_CODE_MB85RCXX, addr, WRITE_CODE);
//    frame[1] = addr & 0xFF;
//    frame[2] = data;
//   
//    i2c->MasterTransmit(frame[0], &frame[1], 2, true);

//    return ERR_OK;
//}

//static int32_t mb85rcxx_write_page(uint32_t addr, uint8_t* data, size_t size)
//{

//    return ERR_OK;
//}

//static int32_t mb85rcxx_read_current()
//{
//    return ERR_OK;
//}

//static int32_t mb85rcxx_read_random()
//{
//    return ERR_OK;
//}

//static int32_t mb85rcxx_read_sequential()
//{
//    return ERR_OK;
//}



int32_t fram_init(void)
{
    int32_t err;
    
    err = i2c->Initialize(NULL);
    if (err) {
        return err;
    }

    err = i2c->PowerControl (ARM_POWER_FULL);
    if (err) {
        return err;
    }

    err = i2c->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD); 
    if (err) {
        return err;
    }
    
    err = i2c->Control(ARM_I2C_BUS_CLEAR, 0);
    if (err) {
        return err;
    }
   

    return ERR_OK;
}

#define EEPROM_I2C_ADDR 0

int32_t fram_read(uint32_t addr, uint8_t* buf, size_t size)
{
    int32_t err;
    uint32_t frame_addr;
    if(addr>EEPROM_SIZE_MASK){
        return -1;
    }
    if(size>(EEPROM_SIZE-addr)){
        return -1;
    }
    frame_addr = (DEV_ADDR_WORD(DEV_TYPE_CODE_MB85RCXX, addr, READ_CDOE) << 16)|
                 (DEV_ADDR_WORD(DEV_TYPE_CODE_MB85RCXX, addr, WRITE_CODE) << 8)|(addr & 0xFF);   

    err = i2c->MasterReceive(frame_addr,buf,size,false);
    if(err){
        return err;
    }
    /* Wait until transfer completed */
    while (i2c->GetStatus().busy) {
        ;
    }
    
    /* Check if all data transferred */
    if(i2c->GetDataCount () != size) {
        return -1;
    }
    
    return ERR_OK;  
}

int32_t fram_write(uint32_t addr, uint8_t* buf, size_t size)
{
    int32_t err;
    uint32_t frame_addr;
    
    if(addr > EEPROM_SIZE_MASK){
        return -1;
    }
    
    if(size > (EEPROM_SIZE - addr)){
        return -1;
    }
    
    frame_addr = (DEV_ADDR_WORD(DEV_TYPE_CODE_MB85RCXX, addr, WRITE_CODE) << 8)|(addr & 0xFF);
    err = i2c->MasterTransmit(frame_addr, buf, size, false);
    if(err){
        return err;
    }
    
    /* Wait until transfer completed */
    while (i2c->GetStatus().busy) {
        ;
    }
    
    /* Check if all data transferred */
    if(i2c->GetDataCount () != size) {
        return -1;
    }
    
    return ERR_OK;
}
