#ifndef __VS_H__
#define __VS_H__

#include <stdint.h>


#define VS_CALI_MODE_INPUT                   0 /* VS calibration by measure the output of chip */

#define VS_CALI_MODE_OUTPUT_2500_MV          1 /* VS calibration by output 2.5V */



int32_t vs_cali_config(uint32_t mode);
int32_t vs_cali_read(uint32_t mode, uint32_t* voltage);

#endif /* __VS_H__ */
