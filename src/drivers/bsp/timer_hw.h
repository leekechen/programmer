#ifndef __TIMER_HW_H__
#define __TIMER_HW_H__

#include "MX_Device.h"
#include "stm32f4xx_ll_tim.h"

#define TIMER_HW_FREQUENCY                      (MX_APB1TimFreq_Value)

static inline void timer_hw_start(void)
{
    LL_TIM_SetCounter(TIM14, 0);
    LL_TIM_SetAutoReload(TIM14, 0xFFFF);
    LL_TIM_EnableCounter(TIM14);
}

static inline void timer_hw_stop(void)
{
    LL_TIM_DisableCounter(TIM14);
}

static int32_t timer_hw_get_count(void)
{
    return LL_TIM_GetCounter(TIM14);
}

#endif /* __HW_TIMER_H__ */