#include <cmsis_os2.h>
#include <stm32f4xx_hal.h>
#include "error.h"
#include "dbg_print.h"
#include "buzzer.h"

static osTimerId_t timer;

static void buzerr_timer_callback(void* arg)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET); 
}

int32_t buzzer_init(void)
{
    timer = osTimerNew(buzerr_timer_callback, osTimerOnce, NULL, NULL);
    if (timer == NULL) {
        err_print("%s(), osTimerNew() failed\n", __func__);
        return ERR_DRV_BUZZER;
    }
    return ERR_OK;
}

void buzzer_beep(uint32_t beep_time)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET); 
    osTimerStart(timer, beep_time);
    return;
}

