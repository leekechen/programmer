#include <stdint.h>
#include <stm32f4xx_hal.h>
#include "test.h"
#include "error.h"

int32_t test_extension_switch(int argc, char* argv[])
{
    int32_t ch,err;
    HAL_GPIO_WritePin(D0_GPIO_Port,D0_Pin,(GPIO_PinState)argc);
    HAL_GPIO_WritePin(D1_GPIO_Port,D1_Pin,(GPIO_PinState)argc);
    HAL_GPIO_WritePin(D2_GPIO_Port,D2_Pin,(GPIO_PinState)argc);
    HAL_GPIO_WritePin(D3_GPIO_Port,D3_Pin,(GPIO_PinState)argc);
    HAL_GPIO_WritePin(D4_GPIO_Port,D4_Pin,(GPIO_PinState)argc);
    HAL_GPIO_WritePin(D5_GPIO_Port,D5_Pin,(GPIO_PinState)argc);
    HAL_GPIO_WritePin(D6_GPIO_Port,D6_Pin,(GPIO_PinState)argc);
    HAL_GPIO_WritePin(D7_GPIO_Port,D7_Pin,(GPIO_PinState)argc);
    test_print("\tPlease check whether the IO port output level correctly.\n");
    test_print("\tPlease input test is pass[p] or fail[f].\n");
    while(1){
        ch = getchar();
        test_print("input:  %c\n", ch); 
        if(ch == 'p'){
            err = ERR_OK;          
            test_print("\tpassed to test extension\n");                                 
            break;
        }
        else if(ch == 'f'){
            err = ERR_TEST_FAILED;          
            test_print("\tFailed to test extension\n");                               
            break;
        }
        else{
            test_print("\tYour input is illegal characters, please enter again\n");    
        }
    }
    return err;
}


struct test_case test_case_extern_output_high = {
    'h',
    "test extension high",
    NULL,
    ERR_OK,
    test_extension_switch,
    1
};

struct test_case test_case_extern_output_low = {
    'l',
    "test extension low",
    NULL,
    ERR_OK,
    test_extension_switch,
    0
};

struct test_case test_case_extern_quit = {
    'q',
    "test extension quit",
    NULL,
    QUIT,
    NULL,
    0
};

static struct test_case *test_cases[] = {
    &test_case_extern_output_high,
    &test_case_extern_output_low,
    &test_case_extern_quit
};

static int32_t test_extension(int argc, char* argv[])
{
    int32_t ret;
    while(1){  
        ret = test_case_exec(test_cases, sizeof(test_cases)/sizeof(struct test_case*));            
        if(ret==QUIT) {
            break;
        }
    }      
    return ret;
}

struct test_case test_case_extension = {
    '6',
    "Test extension",
    NULL,
    ERR_OK,
    test_extension,
    0
};

