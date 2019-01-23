#ifndef __POWER_H__
#define __POWER_H__

#include <stdint.h>

#include "tlv.h"

#define POWER_FLAG_OFF           0
#define POWER_FLAG_ON            1

struct voltage {
    uint32_t rated;
    uint32_t error;
} ;

int32_t power_init              (void (*power_event_callback)(void));
int32_t power_voltage_set       (struct tlv_voltage* voltage);
int32_t power_voltage_get       (int32_t type, uint32_t* mv);
int32_t power_voltage_switch    (int32_t type, uint32_t flag); 

static inline int32_t power_voltage_is_valid  (struct tlv_voltage* voltage)
{
    uint32_t mv;
    uint32_t limit_upper;
    uint32_t limit_lower;
    
    limit_upper = voltage->value + voltage->offset;
    limit_lower = voltage->value - voltage->offset;
    power_voltage_get(voltage->type, &mv);
    if (limit_lower < mv && mv < limit_upper) {
        return 1;
    }
    return 0;
}

#endif /* __POWER_CTRL_H__ */
