#include <stm32f4xx_hal.h>
#include <MX_Device.h>
#include "i2c.h"
#include "delay.h"
#include "stm32f4xx_ll_gpio.h"

static const ARM_DRIVER_VERSION i2c_version         = { 1, 1};
static const ARM_I2C_CAPABILITIES i2c_capabilites   = {0,};

#define SDA_MODE_IN               (LL_GPIO_MODE_INPUT)
#define SDA_MODE_OUT              (LL_GPIO_MODE_OUTPUT)

struct i2c_pin {
    GPIO_TypeDef*   port;
    uint32_t        pin;
};

struct i2c_hw {
    ARM_I2C_STATUS  status;
    struct i2c_pin  sda;
    struct i2c_pin  scl;
    uint32_t        speed;
    uint32_t        data_count;
};

#define io_sda_mode(i, f)           (LL_GPIO_SetPinMode((i)->sda.port,      (i)->sda.pin, (f)))
#define io_scl_high(i)              (LL_GPIO_SetOutputPin((i)->scl.port,    (i)->scl.pin ))
#define io_scl_low(i)               (LL_GPIO_ResetOutputPin((i)->scl.port,  (i)->scl.pin ))
#define io_sda_high(i)              (LL_GPIO_SetOutputPin((i)->sda.port,    (i)->sda.pin ))
#define io_sda_low(i)               (LL_GPIO_ResetOutputPin((i)->sda.port,  (i)->sda.pin ))
#define io_sda_read(i)              (LL_GPIO_IsInputPinSet((i)->sda.port,   (i)->sda.pin ))

static struct i2c_hw i2c1_sw = {
    .sda = {
        .port = MX_I2C1_SDA_GPIOx,
        .pin  = MX_I2C1_SDA_GPIO_Pin
    }, 
    .scl = {
        .port = MX_I2C1_SCL_GPIOx,
        .pin  = MX_I2C1_SCL_GPIO_Pin
    },
    .speed = 10
};

static inline void i2c_io_start(const struct i2c_hw* i2c)
{
    /* Start Condition:
     *   when CLK is high, DATA change form high to low  
     */
    io_sda_mode(i2c, SDA_MODE_OUT); 
    
    io_sda_high(i2c);        
    io_scl_high(i2c);
    delay_us(i2c->speed);

    io_sda_low(i2c);
    delay_us(i2c->speed);

    io_scl_low(i2c);
}   

static inline void i2c_io_stop(const struct i2c_hw* i2c)
{
    /* Stop Condition:
     *   when CLK is high, DATA change form low to high 
     */
    io_sda_mode(i2c, SDA_MODE_OUT); 

    io_scl_low(i2c);
    io_sda_low(i2c);
    delay_us(i2c->speed);    
           
    io_scl_high(i2c);        
    io_sda_high(i2c);
    delay_us(i2c->speed);                                
}

static inline uint8_t i2c_io_wait_ack(const struct i2c_hw* i2c)
{
    uint8_t err_cnt=0;
    
    io_sda_mode(i2c, SDA_MODE_IN); 
    
    io_sda_high(i2c);
    delay_us(i2c->speed);      
    io_scl_high(i2c); 
    delay_us(i2c->speed);   
    while(io_sda_read(i2c)) 
    {
        delay_us(2);   
        err_cnt++;
        if(err_cnt > 250)                          
        {
            i2c_io_stop(i2c);
            return 1;
        }
    }
    io_scl_low(i2c) ; 
    return 0;  
} 

static inline void i2c_io_ack(const struct i2c_hw* i2c)
{
    io_scl_low(i2c);
    io_sda_mode(i2c, SDA_MODE_OUT); 
    io_sda_low(i2c);
    delay_us(i2c->speed);
    io_scl_high(i2c); 
    delay_us(i2c->speed);
    io_scl_low(i2c) ; 
}
           
static inline void i2c_io_nack(const struct i2c_hw* i2c)
{
    io_scl_low(i2c) ;
    io_sda_mode(i2c, SDA_MODE_OUT); 
    io_sda_high(i2c);
    delay_us(i2c->speed);
    io_scl_high(i2c); 
    delay_us(i2c->speed);
    io_scl_low(i2c); 
}                                        
        
void i2c_io_send_byte(const struct i2c_hw* i2c, uint8_t data)
{                        
    uint8_t i; 
    
    io_sda_mode(i2c, SDA_MODE_OUT); 
    io_scl_low(i2c); 
    
    for(i = 0; i < 8; i++) {
        if (data & 0x80) {
            io_sda_high(i2c);
        } else {
            io_sda_low(i2c);
        }
        data <<= 1;      
        delay_us(i2c->speed);
        
        io_scl_high(i2c); 
        delay_us(i2c->speed); 

        io_scl_low(i2c);  
        delay_us(i2c->speed);
    }    
}       
  
uint8_t i2c_io_read_byte(const struct i2c_hw* i2c, bool ack)
{
    uint8_t i;
    uint8_t receive=0;

    io_sda_mode(i2c, SDA_MODE_IN); 
    for(i = 0; i < 8; i++) {          
        io_scl_high(i2c); 
        delay_us(i2c->speed);
        receive <<= 1;
        if(io_sda_read(i2c)){
            receive++;   
        }
        io_scl_low(i2c); 
        delay_us(i2c->speed); 
    }    
    if (!ack) {
        i2c_io_nack(i2c);
    } else {
        i2c_io_ack(i2c);
    }  
    return receive;
}

static ARM_DRIVER_VERSION get_version(void)
{
    return i2c_version;
}

static ARM_I2C_CAPABILITIES get_capabilities(void)
{
    return i2c_capabilites;
}

static int32_t initialize(ARM_I2C_SignalEvent_t cb_event,struct i2c_hw* i2c)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    i2c->status.busy = 0;
    i2c->status.bus_error = 0;
    i2c->data_count = 0;
    return ARM_DRIVER_OK;
}

static int32_t uninitialize(const struct i2c_hw* i2c)
{
    HAL_GPIO_DeInit(GPIOB,GPIO_PIN_6|GPIO_PIN_7);
    return ARM_DRIVER_OK;
}

static int32_t power_control(ARM_POWER_STATE state, const struct i2c_hw* i2c)
{
    return ARM_DRIVER_OK;
}

static int32_t master_transmit(uint32_t addr, const uint8_t* data, uint32_t num, bool xfer_pending,struct i2c_hw* i2c)
{  
    int32_t i,err;
    
    i2c->status.busy = 1;
    i2c_io_start(i2c);
    i2c_io_send_byte(i2c,(uint8_t)(addr/256));
    err = i2c_io_wait_ack(i2c);
    if (err) {
        i2c->status.busy = 0;
        i2c->status.bus_error = 1;
        return err;
    }
  
    i2c_io_send_byte(i2c, (uint8_t)(addr%256));
    err = i2c_io_wait_ack(i2c);
    if (err) {
        i2c->status.busy = 0;
        i2c->status.bus_error = 1;
        return err;
    }
    i2c->data_count = 0;
    for (i = 0; i < num; i++) {
        i2c_io_send_byte(i2c, data[i]); 
        err = i2c_io_wait_ack(i2c);
        if (err) {
            i2c->status.busy = 0;
            i2c->status.bus_error = 1;
            return err;
        }
        i2c->data_count++;
    }
    if (!xfer_pending) {
        i2c_io_stop(i2c);
        i2c->status.busy = 0;
    }   
    return ARM_DRIVER_OK;
}

static int32_t master_receive(uint32_t addr, uint8_t* data, uint32_t num, bool xfer_pending,struct i2c_hw* i2c)
{
    int32_t i;
    int32_t err;
    uint8_t a[3];
    a[0] = (uint8_t)((addr>>16)&0xFF);
    a[1] = (uint8_t)((addr>>8)&0xFF);
    a[2] = (uint8_t)(addr&0xFF);
    i2c->status.busy = 1;
    
    i2c_io_start(i2c);
    i2c_io_send_byte(i2c, a[1]); 
    err = i2c_io_wait_ack(i2c);
    if (err) {
        i2c->status.busy = 0;
        i2c->status.bus_error = 1;
        return err;
    }
    i2c_io_send_byte(i2c, a[2]);
    err = i2c_io_wait_ack(i2c);
    if (err) {
        i2c->status.busy = 0;
        i2c->status.bus_error = 1;
        return err;
    }

    i2c_io_start(i2c);
    i2c_io_send_byte(i2c,a[0]);
    err = i2c_io_wait_ack(i2c);
    if (err) {
        i2c->status.busy = 0;
        i2c->status.bus_error = 1;
        return err;
    }
    i2c->data_count = 0;
    for (i = 0; i < num; i++)
    {
        data[i] = i2c_io_read_byte(i2c,1);
        i2c->data_count++;
    }
    if (!xfer_pending) {
        i2c_io_stop(i2c);
        i2c->status.busy = 0;
    }     
    return ARM_DRIVER_OK;
}

static int32_t slave_transmit(const uint8_t *data, uint32_t num, const struct i2c_hw* i2c)
{
    return ARM_DRIVER_OK;
}

static int32_t slave_receive(uint8_t *data, uint32_t num, const struct i2c_hw* i2c)
{
    return ARM_DRIVER_OK;
}

static int32_t get_data_count(const struct i2c_hw* i2c)
{
    return i2c->data_count;
}

static int32_t control (uint32_t ctrl, uint32_t arg,struct i2c_hw* i2c)
{
    switch (ctrl) {
    case ARM_I2C_OWN_ADDRESS:
        break;
    case ARM_I2C_BUS_SPEED:
        switch(arg){
            case ARM_I2C_BUS_SPEED_STANDARD: i2c->speed = 40;break;
            case ARM_I2C_BUS_SPEED_FAST:     i2c->speed = 30;break;   
            case ARM_I2C_BUS_SPEED_FAST_PLUS:i2c->speed = 20;break;   
            case ARM_I2C_BUS_SPEED_HIGH:     i2c->speed = 10;break;   
            default:break;
        }
        break;
    case ARM_I2C_BUS_CLEAR:
        break;
    case ARM_I2C_ABORT_TRANSFER:
        break;
    default:
        break;
    }
    return ARM_DRIVER_OK;
}

ARM_I2C_STATUS get_status(const struct i2c_hw* i2c)
{
    return i2c->status;
}

int32_t i2c1_sw_initialize (ARM_I2C_SignalEvent_t cb_event) 
{
    return initialize(cb_event, &i2c1_sw);
}

int32_t i2c1_sw_uninitialize (void) 
{
    return uninitialize(&i2c1_sw);
}

int32_t i2c1_sw_power_control (ARM_POWER_STATE state) 
{
    return power_control(state, &i2c1_sw);
}

int32_t i2c1_sw_master_transmit (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) 
{
    return master_transmit(addr, data, num, xfer_pending, &i2c1_sw);
}

int32_t i2c1_sw_master_receive (uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending) 
{
    return master_receive(addr, data, num, xfer_pending, &i2c1_sw);
}

int32_t i2c1_sw_slave_transmit (const uint8_t *data, uint32_t num) 
{
    return slave_transmit(data, num, &i2c1_sw);
}

int32_t i2c1_sw_slave_receive (uint8_t *data, uint32_t num) 
{
    return slave_receive(data, num, &i2c1_sw);
}

int32_t i2c1_sw_get_data_count (void) 
{
    return get_data_count(&i2c1_sw);
}

int32_t i2c1_sw_control (uint32_t ctrl, uint32_t arg) 
{
    return control(ctrl, arg, &i2c1_sw);
}

ARM_I2C_STATUS i2c1_sw_get_status (void) 
{
    return get_status(&i2c1_sw);
}

ARM_DRIVER_I2C driver_i2c1_sw = {
    get_version,
    get_capabilities,
    i2c1_sw_initialize,
    i2c1_sw_uninitialize,
    i2c1_sw_power_control,
    i2c1_sw_master_transmit,
    i2c1_sw_master_receive,
    i2c1_sw_slave_transmit,
    i2c1_sw_slave_receive,
    i2c1_sw_get_data_count,
    i2c1_sw_control,
    i2c1_sw_get_status
};




