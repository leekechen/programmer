#include <stdint.h>
#include "error.h"
#include "test.h"

int32_t temperature_io_set(int argc, char* argv[])
{
    return ERR_OK;
}

struct test_case test_case_temperature_ioset_h = {
    'h',
    "test temperature io set h",
    NULL,
    ERR_OK,
    temperature_io_set,
    0
};

struct test_case test_case_temperature_ioset_l = {
    'l',
    "test temperature io set l",  
    NULL,
    ERR_OK,
    temperature_io_set,
    0
};

struct test_case test_case_temperature_quit = {
    'q',
    "test temperature quit",
    NULL,
    QUIT,
    NULL,
    0
};

static struct test_case *test_temperature_cases[] = {
    &test_case_temperature_ioset_h,
    &test_case_temperature_ioset_l,
    &test_case_temperature_quit
};

int32_t test_temperature_switch(int argc, char* argv[])
{
    int32_t ret;
    test_case_temperature_ioset_h.func_idx = argc;
    test_case_temperature_ioset_l.func_idx = argc;
    while(1)
    {  
        ret = test_case_exec(test_temperature_cases, sizeof(test_temperature_cases)/sizeof(struct test_case*));           
        if(ret==QUIT) 
        {
            break;
        }
    }      
    return ERR_OK;
}

static struct test_case test_case_temperature_scl = {
    'c',
    "test temperature scl",
	NULL,
	ERR_OK,
	test_temperature_switch,
    0
};

static struct test_case test_case_temperature_sda = {
    'd',
    "test temperature sda",
	NULL,
	ERR_OK,
	test_temperature_switch,
    1
};

static struct test_case *test_cases[] = {
    &test_case_temperature_scl,
    &test_case_temperature_sda,
    &test_case_temperature_quit
};

static int32_t test_temperature(int argc, char* argv[])
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

struct test_case test_case_temperature = {
    'b',
    "test temperature",
	NULL,
	ERR_OK,
	test_temperature,
    0
};



