#include <stm32f4xx_hal.h>

static void test_USB_EN_pin(void)
{
    HAL_GPIO_WritePin(USB_EN_GPIO_Port, USB_EN_Pin, GPIO_PIN_RESET);
    
    return;
}