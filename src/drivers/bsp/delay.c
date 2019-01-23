#include "delay.h"

/*
 * TIM6/TIM7/TIM13's clock source is APB1, the frequency is MX_APB1TimFreq_Value
 * TIM10's clock source is AP2, the frequency is MX_APB2TimFreq_Value
 */

void delay_init(void)
{    
    /* Used as iscp delay, in us, MAX delay is 65536 us*/
    LL_TIM_SetAutoReload(TIM6, 0xFFFF);
    /* Used as general delay is, MAX delay is 65536 us*/
    LL_TIM_SetAutoReload(TIM7, 0xFFFF);
    /* Used as iscp delay, in ns, MAX delay is 386662 ns*/
    LL_TIM_SetAutoReload(TIM10, 0xFFFF);
    /* Used as delay_ms, in ms, MAX delay is 65536 ms*/
    LL_TIM_SetAutoReload(TIM13, 0xFFFF);
}

void delay_us(int32_t us)
{
    int32_t count;

    LL_TIM_SetCounter(TIM7, 0);
    count = 0;
    LL_TIM_EnableCounter(TIM7);
    do {
        count = LL_TIM_GetCounter(TIM7);
    } while (count < us);
    LL_TIM_DisableCounter(TIM7);
}

void delay_ms(int32_t ms)
{
    int32_t count;
    
    ms *= 2;
    
    LL_TIM_SetCounter(TIM13, 0);
    count = 0;
    LL_TIM_EnableCounter(TIM13);
    do {
        count = LL_TIM_GetCounter(TIM13);
    } while (count < ms);
    LL_TIM_DisableCounter(TIM13);    
}



//#define TEST_CYCLE_NUM   10

//#include "io_icsp.h"
//#include "power.h"
//#include "error.h"

//void test_delay(void)
//{
//    int32_t i;
//    int32_t err;
//    
//     err = power_voltage_switch(POWER_TYPE_VDD, POWER_FLAG_ON);
//    if (err) {
//        return ;
//    }   

//    io_icsp_init();
//    io_icsp_da_set_out();
//    io_icsp_da2_set_out();
// 
//while (1) {    
//    for (i = 0; i < TEST_CYCLE_NUM; i++) {
//        io_icsp_delay_us(1);
//        io_icsp_da_toggle();
//        io_icsp_da2_toggle();
//        io_icsp_clk_toggle();
//    }
//} 

//while (1) {
//    for (i = 0; i < TEST_CYCLE_NUM; i++) {
//        io_icsp_delay_us(110);
//        io_icsp_da_toggle();
//    }
//} 
//while (1) {
//    for (i = 0; i < TEST_CYCLE_NUM; i++) {
//        io_icsp_delay_ns(1);
//        io_icsp_da_toggle();
//    }
//}
//while (1) {
//    for (i = 0; i < TEST_CYCLE_NUM; i++) {
//        delay_ms(200);
//        io_icsp_da_toggle();
//    }
//}
//}
