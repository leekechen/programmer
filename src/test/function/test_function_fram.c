#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stm32f4xx_hal.h>
#include "test.h"
#include "error.h"
#include "mb85rcxx.h"
#include "cmsis_os2.h"

static uint8_t page_data[EEPROM_SIZE];

static int32_t test_function_Random_data(int argc, char* argv[])
{
    int32_t err;
    uint32_t i,random,addr,size,value,testCaseNum = 100;
    struct _rand_state state;
    _srand_r(&state, osKernelGetTickCount());

    do
    {
        random = _rand_r(&state);
        addr = random % EEPROM_SIZE;

        random = _rand_r(&state);
        size = random % EEPROM_SIZE;
        size = size > (EEPROM_SIZE - addr) ? (EEPROM_SIZE - addr) : size;

        random = _rand_r(&state);
        value  = random & 0xFF;
      
        memset(&page_data[addr],value,size);
        
        test_print("\t Address[0x%x~0x%x],size=%d,value=0x%x\n", addr, addr + size, size, value); 
        
        err = fram_write(addr,&page_data[addr],size);
        if(err){
            test_print("\t\t Error,fram Random write failed\n");
            return err;    
        }

        err = fram_read(addr,&page_data[addr],size);
        if(err){
            test_print("\t\t Error,fram Random read failed\n");
            return err;    
        }

        for(i=0;i<size;i++){
            if(page_data[i+addr] != value){
                test_print("\t\t Error,fram Random check failed\n");
                return -1;    
            }
        }
    }while(--testCaseNum);
    return ERR_OK;
}


static int32_t test_function_auto_write_all_page(int argc, char* argv[])
{
    int32_t err;
    uint32_t i;
    memset(&page_data[0],0x55,EEPROM_SIZE); 
    err = fram_write(0,page_data,EEPROM_SIZE); 
    if(err){
            test_print("\t\t Error,fram write failed\n");
            return err;    
    }
    err = fram_read(0,page_data,EEPROM_SIZE);
    if(err){
        test_print("\t\t Error,fram read failed\n");
        return err;    
    }
    for(i=0;i<EEPROM_SIZE;i++){
        if(page_data[i] != 0x55){
            test_print("\t\t Error,fram check failed\n");
            return -1;    
        }
    }
    return ERR_OK;
}

static int32_t test_function_write_all_page(int argc, char* argv[])
{
    int32_t err;
    memset(&page_data[0],0xAA,EEPROM_SIZE); 
    err = fram_write(0,page_data,EEPROM_SIZE); 
    if(err){
            test_print("\t\t Error,fram write failed\n");
            return err;    
    }            
    return ERR_OK;
}

static int32_t test_function_read_all_page(int argc, char* argv[])
{
    uint32_t i;
    int32_t err;
    err = fram_read(0,page_data,EEPROM_SIZE);
    if(err){
        test_print("\t\t Error,fram read failed\n");
        return err;    
    }
    for(i=0;i<EEPROM_SIZE;i++){
        if(page_data[i] != 0xAA){
            test_print("\t\t Error,fram check failed\n");
            return -1;    
        }
    }
    return ERR_OK;
}


struct test_case test_function_fram_Random_write_read = {
    'd',
    "test function Random write read",
    NULL,
    ERR_OK,
    test_function_Random_data,
    0
};

struct test_case test_function_fram_auto_allpage = {
    'a',
    "test function the whole area auto write read",
    NULL,
    ERR_OK,
    test_function_auto_write_all_page,
    0
};


struct test_case test_function_fram_write_allpage = {
    'w',
    "test function write all page",
    NULL,
    ERR_OK,
    test_function_write_all_page,
    0
};

struct test_case test_function_fram_read_allpage = {
    'r',
    "test function read all page",
    NULL,
    ERR_OK,
    test_function_read_all_page,
    0
};

struct test_case test_function_fram_quit = {
    'q',
    "test function quit",
    NULL,
    QUIT,
    NULL,
    0
};

static struct test_case *test_function_cases[] = {
    &test_function_fram_Random_write_read,
    &test_function_fram_auto_allpage,
    &test_function_fram_write_allpage,
    &test_function_fram_read_allpage,
    &test_function_fram_quit
};

static int32_t test_function_fram_exec(int argc, char* argv[])
{
    int32_t ret;
    while(1)
    {  
        ret = test_case_exec(test_function_cases, sizeof(test_function_cases)/sizeof(struct test_case*));           
        if(ret==QUIT) 
        {
            break;
        }
    }      
    return ERR_OK;
}

struct test_case test_function_fram = {
    '1',
    "test function fram",
    NULL,
    ERR_OK,
    test_function_fram_exec,
    0
};


