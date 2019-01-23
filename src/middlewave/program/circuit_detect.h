#ifndef __CIRCUIT_DETECT_H__
#define __CIRCUIT_DETECT_H__

#include <stdint.h>

        
#define PIN_VDD          (1 << 0)
#define PIN_VS           (1 << 1)
#define PIN_MISO         (1 << 2)
#define PIN_MOSI         (1 << 3)
#define PIN_CLK          (1 << 4)
#define PIN_VPP          (1 << 5)
#define PIN_REST         (1 << 6)

#define SHIELDING_CHANNEL_VDD     (0)
#define SHIELDING_CHANNEL_VS      (1)
#define SHIELDING_CHANNEL_MISO    (2)
#define SHIELDING_CHANNEL_MOSI    (3)
#define SHIELDING_CHANNEL_CLK     (4)
#define SHIELDING_CHANNEL_VPP     (5)
#define SHIELDING_CHANNEL_REST    (6)
#define SHIELDING_CHANNEL_NONE    (7)

#define PROGRAM_OPEN_THRESHOLD_VALUE   600
#define PROGRAM_SHORT_THRESHOLD_VALUE  3000

int32_t circuit_detect_turn_off(void);

int32_t circuit_detect_turn_on(void);


int32_t circuit_detect_polling_all_adc_read(uint32_t shielding_channel,uint32_t *adc_value);
int32_t circuit_detect_pin_status_is_open(uint32_t pin_no, uint32_t *status);
int32_t circuit_detect_pin_status_is_short(uint32_t pin_no, uint32_t *status);
int32_t circuit_detect_pin_get_adc_value(uint32_t pin_no,uint32_t *adc_value);
int32_t circuit_detect_pin_get_voltage_value(uint32_t pin_no,uint32_t *voltage_value);

#endif /* __CIRCUIT_DETECT_H__ */
