#include <stdint.h>
#include <string.h>

#include "error.h"
#include "test.h"
#include "w25qxx.h"

#include "flash_map.h"

#define MODULE_DEBUG       1
#define MODULE_ERROR       1

#include "dbg_print.h"

#define FLASH_W25QXX_READ    0
#define FLASH_W25QXX_WRITE   1

int32_t test_flash_map(void)
{
    int32_t i;
    int32_t err;
    uint32_t addr;
    uint8_t src[256];
    uint8_t dst[256];
    uint32_t* ptr_src = (uint32_t*)src;
    uint32_t* ptr_dst = (uint32_t*)dst;

    uint32_t size = 512*1024;
    
    flash_map_init();
    
    flash_map_erase(0);
    
     for (addr = 0; addr < size; addr += sizeof(dst)) { 
        printf("Verify Erase addr=0x%08x~0x%08x ... ", addr, addr + sizeof(dst));  
        
        memset(dst, 0, sizeof(dst));
        err = flash_map_read(0, addr, dst, sizeof(dst));
        if (err != sizeof(dst)) {
            dbg_print("w25qxx_read() failed\n");
            return err;
        }

        for (i = 0; i < sizeof(dst); i++) {
            if (dst[i] != 0xFF) {
                printf("failed, 0x%08x != 0x%08x\n", ptr_dst[i], (addr+i));
                dbg_dump("Error Erase", dst, sizeof(dst));
                return -1;
            }
        }  
        dbg_print("Passed!\n");        
    }  
    
    for (addr = 0; addr < size; addr += sizeof(src)) {
        for (i = 0; i < (sizeof(src)/4); i++) {
            ptr_src[i] = addr + i;
        }

        printf("Program: addr=0x%08x~0x%08x ... ", addr, addr + sizeof(src));
        err = flash_map_write(0, addr, src, sizeof(src));
        if (err != sizeof(src)) {
            printf("flash_map_write() failed\n");
            return err;
        }
        printf("Passed! ");

        printf("Verify ... ");

        memset(ptr_dst, 0, sizeof(src));
        err = flash_map_read(0, addr, dst, sizeof(dst));
        if (err != sizeof(dst)) {
            printf(" flash_map_read() failed\n");
            return err;
        }

        for (i = 0; i < (sizeof(dst)/4); i++) {
            if (ptr_dst[i] != (addr + i)) {
                printf("failed, 0x%08x != 0x%08x\n", ptr_dst[i], (addr+i));
                dbg_dump("dst data", dst, sizeof(dst));
                dbg_dump("src data", src, sizeof(src));
                return -1;
            }

        }
        printf("Passed\n");
    }
    return 0;
}


int32_t test_w25qxx(void)
{
    int32_t err;
    int32_t i;
    int32_t handle;
    uint8_t src[256];
    uint8_t dst[256];

    memset(src, 0, sizeof(src));
    
    err = w25qxx_init();
    if (err) {
        printf("w25qxx_init() failed\n");
        return err;
    }
    handle = w25qxx_open(DEV_SPI1, 2*1024*1024);
    err = w25qxx_power_control(handle, ARM_POWER_FULL, 42000000);
    if (err) {
        printf("w25qxx_power_control() failed\n");
        return err;
    }
    printf("w25qxx_read_id()\n");
    err = w25qxx_read_id(handle, src, 3);
    if (err) {
        printf("w25qxx_read_id() failed\n");
        return err;
    }


{
    uint32_t addr = 0;
    uint32_t size = 1024 * 1024 * 2;
    uint32_t* ptr_src = (uint32_t*)src;
    uint32_t* ptr_dst = (uint32_t*)dst;
    
    printf("Test Case Erase:\n");
    
    printf("Erase: addr=0x%08x ~ 0x%08x ... ", addr, addr + size);
    
    err = w25qxx_erase(handle, 0, size);
    if (err) {
        dbg_print("w25qxx_erase() failed\n");
        return err;
    }
    
    printf("Passed!\n");
         
    for (addr = 0; addr < size; addr += sizeof(dst)) { 
        printf("Verify Erase: addr=0x%08x~0x%08x ... ", addr, addr + sizeof(dst));  
        
        memset(dst, 0, sizeof(dst));
        err = w25qxx_read(handle, addr, dst, sizeof(dst));
        if (err != sizeof(dst)) {
            dbg_print("w25qxx_read() failed\n");
            return err;
        }

        for (i = 0; i < sizeof(dst); i++) {
            if (dst[i] != 0xFF) {
                printf("failed, 0x%08x != 0x%08x\n", ptr_dst[i], (addr+i));
                dbg_dump("Error Erase", dst, sizeof(dst));
                return -1;
            }
        }  
        dbg_print("Passed!\n");        
    }  
    
    
    printf("\nTest Case Program:\n");
    
    for (addr = 0; addr < size; addr += sizeof(src)) {
        for (i = 0; i < (sizeof(src)/4); i++) {
            ptr_src[i] = addr + i;
        }
    
        printf("Program: addr=0x%08x~0x%08x ... ", addr, addr + sizeof(src));
        err = w25qxx_program(handle, addr, src, sizeof(src));
        if (err != sizeof(src)) {
            printf("w25qxx_program() failed\n");
            return err;
        }
        printf("Passed! ");
        
        printf("Verify ... ");

        memset(ptr_dst, 0, sizeof(src));
        err = w25qxx_read(handle, addr, dst, sizeof(dst));
        if (err != sizeof(dst)) {
            printf(" w25qxx_read() failed\n");
            return err;
        }

        for (i = 0; i < (sizeof(dst)/4); i++) {
            if (ptr_dst[i] != (addr + i)) {
                printf("failed, 0x%08x != 0x%08x\n", ptr_dst[i], (addr+i));
                dbg_dump("dst data", dst, sizeof(dst));
                dbg_dump("src data", src, sizeof(src));
                return -1;
            }
        
        }
        printf("Passed\n");
    }
}   
    printf("All test Passed!\n");
    return ERR_OK;
}

int32_t test_flash_read(int argc, char* argv[])
{
    int32_t err;
    err = test_w25qxx();
  
    return err;
}

int32_t test_flash_write(int argc, char* argv[])
{
    int32_t err;
    err = test_flash_map();
  
    return err;
}

static struct test_case test_case_flash_read = {
    'r',
    "test flash read",
    NULL,
    ERR_OK,
    test_flash_read,
    0
};

static struct test_case test_case_flash_write = {
    'w',
    "test flash write",
    NULL,
    ERR_OK,
    test_flash_write,
    0
};

struct test_case test_case_flash_quit = {
    'q',
    "test flash quit",
    NULL,
    QUIT,
    NULL,
    0
};

static struct test_case *test_cases[] = {
    &test_case_flash_read,
    &test_case_flash_write,
    &test_case_flash_quit
};

static int32_t test_spi_flash(int argc, char* argv[])
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
    return ret;
}

struct test_case test_case_flash = {
    '7',
    "test spi flash",
    NULL,
    ERR_OK,
    test_spi_flash,
    0
};




