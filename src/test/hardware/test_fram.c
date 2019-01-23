#include <stdint.h>
#include <stm32f4xx_hal.h>
#include "test.h"
#include "error.h"
#include "i2c.h"
#include "mb85rcxx.h"



extern ARM_DRIVER_I2C driver_i2c1_sw;

int32_t test_fram_read_data(int argc, char* argv[])
{ 
    int32_t err;
    uint8_t fram_write_data[10] = {0,1,2,3,4,5,6,7,8,9},fram_read_data[10] = {0};   
    
    fram_init();
    err = fram_read(0x0008,&fram_read_data[0],10);
    if (err) {
        return err;
    } 
    
    if (fram_read_data[9] != 0x99){
        return ERR_TEST_FAILED;
    }
    return ERR_OK;
}

int32_t test_fram_write_data(int argc, char* argv[])
{
    int32_t err;
    uint8_t fram_write_data[10] = {0xAA,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99};
    
    fram_init();
    err = fram_write(0x0008, &fram_write_data[0], 10);
    if (err) {
        return err;
    }
    return ERR_OK;
}

struct test_case test_case_fram_quit = {
    'q',
    "test fram quit",
    NULL,
    QUIT,
    NULL,
    0 
};

struct test_case test_case_fram_read = {
    'r',
    "test fram read",
    NULL,
    ERR_OK,
    test_fram_read_data,
    0
};

struct test_case test_case_fram_write = {
    'w',
    "test fram write",
    NULL,
    ERR_OK,
    test_fram_write_data,
    0
};

static struct test_case *test_cases[] = {
    &test_case_fram_read,
    &test_case_fram_write,
    &test_case_fram_quit
};

static int32_t test_fram(int argc, char* argv[])
{
    int32_t ret;
    while(1)
    {  
        ret = test_case_exec(test_cases, sizeof(test_cases)/sizeof(struct test_case*));           
        if(ret==QUIT) 
        {
            break;
        }
    }      
    return ERR_OK;
}

struct test_case test_case_fram = {
    '8',
    "test fram",
    NULL,
    ERR_OK,
    test_fram,
    0
};




