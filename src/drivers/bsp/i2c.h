#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>
#include <Driver_I2C.h>


ARM_DRIVER_VERSION      i2c_get_version(void);
ARM_I2C_CAPABILITIES    i2c_get_capabilities(void);
int32_t                 i2c_initialize(ARM_I2C_SignalEvent_t cb_event);
int32_t                 i2c_uninitialize(void);
int32_t                 i2c_power_control(ARM_POWER_STATE state);
int32_t                 i2c_master_transmit(uint32_t addr, const uint8_t* data, uint32_t num, bool xfer_pending);
int32_t                 i2c_master_receive(uint32_t addr, uint8_t* data, uint32_t num, bool xfer_pending);
int32_t                 i2c_slave_transmit(const uint8_t *data, uint32_t num);
int32_t                 i2c_slave_receive(uint8_t *data, uint32_t num);
int32_t                 i2c_get_data_count(void );
int32_t                 i2c_control (uint32_t control, uint32_t arg);
ARM_I2C_STATUS          i2c_get_status(void);
void                    i2c_signal_event(uint32_t event);



#endif /* __I2C_H__ */

