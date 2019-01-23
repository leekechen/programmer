#include <stdlib.h>
#include <cmsis_os2.h>
#include <stm32f4xx_hal.h>


//__asm(".global __use_no_semihosting_swi\n\t");
//extern int stderr_putchar (int ch);
//extern int stdin_getchar (void);
//extern int stdout_putchar (int ch);
//extern void ttywrch (int ch);
//extern void _sys_exit(int return_code);
//extern char *_sys_command_string(char *cmd, int len);

extern int stderr_putchar (int ch)
{
    uint8_t c;
    
    c = (uint8_t)ch;
    while (!(USART1->SR & USART_SR_TC)) {
    }
    if (c == '\n') { 
        USART1->DR = '\r';                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
        while (!(USART1->SR & USART_SR_TC)) {
        }       
       
    }
    USART1->DR = (uint8_t)ch;
    return ch;

}

extern int stdin_getchar (void)
{
    while (!(USART1->SR & USART_SR_RXNE)) {
    }
    
    return (int)USART1->DR;
}

extern int stdout_putchar (int ch)
{
    uint8_t c;
    
    c = (uint8_t)ch;
    while (!(USART1->SR & USART_SR_TC)) {
    }
    if (c == '\n') { 
        USART1->DR = '\r';                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
        while (!(USART1->SR & USART_SR_TC)) {
        }       
       
    }
    USART1->DR = (uint8_t)ch;
    return ch;
}

extern void ttywrch (int ch)
{
    (void)ch;
    
    return;
}

extern void _sys_exit(int return_code)
{
    (void)return_code;
label:  goto label;  /* endless loop */
}

extern char *_sys_command_string(char *cmd, int len)
{
    (void)cmd;
    (void)len;
    
    return NULL;
}

uint32_t HAL_GetTick(void)
{
    return osKernelGetTickCount();
}
