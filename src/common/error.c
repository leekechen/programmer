#include <stdlib.h>
#include <cmsis_os2.h>
#include <stm32f4xx_hal.h>
#include "error.h"
#include "buzzer.h"
#include "led.h"

static osSemaphoreId_t  sem_err;

static uint32_t         err_code;

static void buzzer_error_beep(void)
{    
    buzzer_beep(80);
    osDelay(160);
    buzzer_beep(80);
    osDelay(160);
    buzzer_beep(80);
    osDelay(160);
}

static void led_failed(void)
{ 
    led_set(LED_TYPE_OK, LED_FLAG_OFF);   
    led_set(LED_TYPE_FAIL, LED_FLAG_ON);
    led_set(LED_TYPE_BUSY, LED_FLAG_OFF); 
}

static void led_ok(void)
{
    led_set(LED_TYPE_FAIL, LED_FLAG_OFF);   
    led_set(LED_TYPE_OK, LED_FLAG_ON);
    led_set(LED_TYPE_BUSY, LED_FLAG_OFF); 
}

void err_working(void)
{
    led_set(LED_TYPE_OK, LED_FLAG_OFF); 
    led_set(LED_TYPE_FAIL, LED_FLAG_OFF); 
    led_set(LED_TYPE_BUSY, LED_FLAG_ON);
}


int32_t err_init(void)
{
    sem_err = osSemaphoreNew(1, 0, NULL);
    if (!sem_err) {
        //err_print("osSemaphoreNew(1,0,NULL) failed\n");
        return ERR_SYS_OS;
    }
    return ERR_OK;
}

void err_handler(int32_t err)
{
    if (err) {
        //buzzer_error_beep();
        led_failed();
    } else {
        led_ok();
    }
    err_code = err;
    osSemaphoreRelease(sem_err);
}

uint32_t err_get_code(void)
{
    osSemaphoreAcquire(sem_err, osWaitForever);
    return err_code;
}

const char* errGetMsg(int32_t errCode)
{
    (void)errCode;
    return NULL;
}
