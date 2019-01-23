#include <stdint.h>
#include "error.h"
#include "test.h"
#include "led.h"

int32_t test_led_seton(int argc, char* argv[])
{
    int32_t ch,err;
    led_set(argc,LED_FLAG_ON);
    test_print("\tPlease check whether the LED port output level correctly.\n");
    test_print("\tPlease input test is pass[p] or fail[f].\n");
    while(1){
        ch = getchar();
        test_print("input:  %c\n", ch); 
        if(ch == 'p'){
            err = ERR_OK;          
            test_print("\tpassed to test led on\n");                                 
            break;
        }
        else if(ch == 'f'){
            err = ERR_TEST_FAILED;          
            test_print("\tFailed to test led on\n");                               
            break;
        }
        else{
            test_print("\tYour input is illegal characters, please enter again\n");    
        }
    }
    return err;
}

int32_t test_led_setoff(int argc, char* argv[])
{
    int32_t ch,err;
    led_set(argc,LED_FLAG_OFF);
    test_print("\tPlease check whether the LED port output level correctly.\n");
    test_print("\tPlease input test is pass[p] or fail[f].\n");
    while(1){
        ch = getchar();
        test_print("input:  %c\n", ch); 
        if(ch == 'p'){
            err = ERR_OK;          
            test_print("\tpassed to test led off\n");                                 
            break;
        }
        else if(ch == 'f'){
            err = ERR_TEST_FAILED;          
            test_print("\tFailed to test led off\n");                               
            break;
        }
        else{
            test_print("\tYour input is illegal characters, please enter again\n");    
        }
    }
    return err;
}

struct test_case test_case_led_on = {
    'o',
    "test led on",
    NULL,
    ERR_OK,
    test_led_seton,
    0
};

struct test_case test_case_led_off = {
    'f',
    "test led off",
    NULL,
    ERR_OK,
    test_led_setoff,
    0
};

struct test_case test_case_led_quit = {
    'q',
    "test led quit",
    NULL,
    QUIT,
    NULL,
    0
};

static struct test_case *test_led_cases[] = {
    &test_case_led_on,
    &test_case_led_off,
    &test_case_led_quit
};

static int32_t test_led_switch(int argc, char* argv[])
{
    int32_t ret;
    test_case_led_on.func_idx = argc;
    test_case_led_off.func_idx = argc;
    while(1){  
        ret = test_case_exec(test_led_cases, sizeof(test_led_cases)/sizeof(struct test_case*));           
        if(ret==QUIT) {
            break;
        }
    }      
    return ret;
}

static struct test_case test_case_led_busy = {
    'b',
    "test led switch busy",
    NULL,
    ERR_OK,
    test_led_switch,
    1
};

static struct test_case test_case_led_ng = {
    'n',
    "test led switch ng",
    NULL,
    ERR_OK,
    test_led_switch,
    2
};

static struct test_case test_case_led_ok = {
    'o',
    "test led switch ok",
    NULL,
    ERR_OK,
    test_led_switch,
    3
};

static struct test_case *test_cases[] = {
    &test_case_led_busy,
    &test_case_led_ng,
    &test_case_led_ok,
    &test_case_led_quit
};

static int32_t test_led(int argc, char* argv[])
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

struct test_case test_case_led = {
    '4',
    "test led",
    NULL,
    ERR_OK,
    test_led,
    0
};

