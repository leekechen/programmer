#ifndef __DELAY_H__
#define __DELAY_H__

#include <stdint.h>
#include "stm32f4xx_ll_tim.h"

void delay_init(void);
void delay_us(int32_t us);
void delay_ms(int32_t ms);
void delay_ns(int32_t ms);

static inline void io_icsp_delay_us(int32_t us)
{
    int32_t count;

    LL_TIM_SetCounter(TIM6, 0);
    count = 0;
    LL_TIM_EnableCounter(TIM6);
    do {
        count = LL_TIM_GetCounter(TIM6);
    } while (count < us);
    LL_TIM_DisableCounter(TIM6);
}

static inline void io_icsp_delay_ns(int32_t ns)
{
#if 1
    while (ns--) {
        ;
    }
#else    
    int32_t count;
    int32_t max_count = ns / 6;
    
    if (max_count < 6) {
        return;
    }    
    
    LL_TIM_SetCounter(TIM10, 0);
    count = 0;
    LL_TIM_EnableCounter(TIM10);
    do {
        count = LL_TIM_GetCounter(TIM10);
    } while (count < max_count);
    LL_TIM_DisableCounter(TIM10);
#endif   
}

#endif /* __DELAY_H__ */
