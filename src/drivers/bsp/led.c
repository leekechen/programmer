#include <stm32f4xx_hal.h>
#include "led.h"

void led_set(int32_t type, uint32_t flag)
{
    switch (type) {
    case LED_TYPE_OK:
        HAL_GPIO_WritePin(OK_GPIO_Port, OK_Pin, (GPIO_PinState)flag);
        break;
    case LED_TYPE_FAIL:
        HAL_GPIO_WritePin(NG_GPIO_Port, NG_Pin, (GPIO_PinState)flag);
        break;
    case LED_TYPE_BUSY:
        HAL_GPIO_WritePin(BUSY_GPIO_Port, BUSY_Pin, (GPIO_PinState)flag);
        break;
    default:
        break;
    }
}
