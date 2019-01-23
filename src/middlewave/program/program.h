#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <stdlib.h>
#include <stdint.h>

#include "tlv.h"
#include "file_hex.h"

#define PROGRAM_NO_LIMIT          0

struct chip_operation {
    int32_t (*is_matched)   (const char* name, uint8_t* id, size_t id_len);
    int32_t (*hande_shake)  (struct tlv_file* hex);
    int32_t (*get_id)       (struct tlv_file* hex);
    int32_t (*osc_cali)     (struct tlv_file* hex);
    int32_t (*wdt_cali)     (struct tlv_file* hex);
    int32_t (*adc_cali)     (struct tlv_file* hex);
    int32_t (*blank_check)  (struct tlv_file* hex);
    int32_t (*erase)        (struct tlv_file* hex);
    int32_t (*program)      (struct tlv_file* hex);
    int32_t (*protect)      (struct tlv_file* hex);
    int32_t (*verify)       (struct tlv_file* hex);
    int32_t (*readback)     (struct tlv_file* hex);
    int32_t (*get_size)     (uint32_t* size);
} ;


int32_t chip_program(uint32_t bitmap);
int32_t chip_program_power_on(uint32_t vpp_delay_us, uint32_t stable_delay_ms);
int32_t chip_program_power_off(void);


#endif /* __PROGRAM_H__ */
