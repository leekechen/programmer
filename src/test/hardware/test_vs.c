#include <stdint.h>
#include <stm32f4xx_hal.h>
#include "error.h"
#include "test.h"
#include "vs.h"

static int32_t vs_ad_vs_filter(void)
{
    int32_t  i;
    uint32_t vol = 0;
    uint32_t adc;
    
    for (i = 0; i < 16; i++) {
        vs_cali_read(VS_CALI_MODE_OUTPUT_2500_MV, &adc);
        vol += adc;
    }
    return vol / 16;
}

static int32_t vs_ad_vs_b_filter(void)
{
    int32_t  i;
    uint32_t vol = 0;
    uint32_t adc;
    
    for (i = 0; i < 16; i++) {
        vs_cali_read(VS_CALI_MODE_INPUT, &adc);
        vol += adc;
    }
    return vol / 16;
}


int32_t test_vs_out(int argc, char* argv[])
{
    int32_t  err;
    uint32_t ad_vs;
    uint32_t ad_vs_b;
    int32_t  ch;
    
    test_print("%s", "[Test] VS config to output:\n");
    err = vs_cali_config(VS_CALI_MODE_OUTPUT_2500_MV);
    
    
    for (ch = 0; ch != 'q'; ch = getchar()) {
        test_print("%s", "[Test] VS Output, any key (expect \'q\') ADC read value\n");
        ad_vs = vs_ad_vs_filter();
        ad_vs_b = vs_ad_vs_b_filter();
        test_print("\t AD-VS: %d mV\n", ad_vs);
        test_print("\t AD-VS-B: %d mV\n", ad_vs_b);
    }
    
    return ERR_OK;
}

int32_t test_vs_in(int argc, char* argv[])
{
    int32_t  err;
    uint32_t ad_vs;
    uint32_t ad_vs_b;
    int32_t  ch;
    
    test_print("%s", "[Test] VS config to input:\n");
    err = vs_cali_config(VS_CALI_MODE_INPUT);
    
    
    for (ch = 0; ch != 'q'; ch = getchar()) {
        test_print("%s", "[Test] VS Output, any key (expect \'q\') ADC read value\n");
        ad_vs = vs_ad_vs_filter();
        ad_vs_b = vs_ad_vs_b_filter();
        test_print("\t AD-VS: %d mV\n", ad_vs);
        test_print("\t AD-VS-B: %d mV\n", ad_vs_b);
    }
    
    return ERR_OK;
}

struct test_case test_case_vs_in = {
    'o',
    "test VS with ADC measure the input",
    NULL,
    ERR_OK,
    test_vs_in,
    0
};

struct test_case test_case_vs_out = {
    'f',
    "test VS with precise 2500mV output",
    NULL,
    ERR_OK,
    test_vs_out,
    0
};

struct test_case test_case_vs_quit = {
    'q',
    "test vs quit",
    NULL,
    QUIT,
    NULL,
    0
};

static struct test_case *test_cases[] = {
    &test_case_vs_out,
    &test_case_vs_in,
    &test_case_vs_quit,
};

static int32_t test_led(int argc, char* argv[])
{
    int32_t ret;
    while(1)
    {  
        ret = test_case_exec(test_cases, sizeof(test_cases)/sizeof(struct test_case*));           
        if (ret == QUIT) 
        {
            break;
        }
    }      
    return ERR_OK;
}





static int32_t test_vs(int argc, char* argv[])
{
    int32_t ret;
    while(1) {  
        ret = test_case_exec(test_cases, sizeof(test_cases)/sizeof(struct test_case*));           
        if (ret == QUIT) {
            break;
        }
    }      
    return ERR_OK;
}

struct test_case test_case_vs = {
    '9',
    "test vs",
    NULL,
    ERR_OK,
    test_vs,
    0
};
