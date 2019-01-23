#include <string.h>
#include "stm32f4xx_hal.h"
#include "crc32.h"
#include "test.h"

#if TEST_CRC32


#define MODULE_DEBUG     						1
#define MODULE_ERROR     						1

#include "log.h"



#define testDump(msg, ptr, len)	                                                                \
                                do {                                                            \
                                        logLock();                                              \
                                        int32_t ___i;                                           \
                                        printf("[Thread:%p] %s,DumP: %p[%d]\n", osThreadGetId(), (msg), (ptr), (len));       \
                                        for (___i = 0; ___i < (len); ___i++) {                  \
                                            if ((___i % 16) == 0) { 							\
                                                printf("\n\t[0x%08x]\t",___i);					\
                                            }													\
                                            printf("0x%02x,", (ptr)[___i]);                       \
                                        }                                                       \
                                        printf("\n");                                           \
                                        logUnlock();                                            \
                                } while (0)

static uint8_t testBuf[1024*10];

static void myMemset(uint32_t* buf, uint32_t value, size_t size)
{
    size_t i;

    for (i = 0; i < size; i++) {
        buf[i] = value;
    }
}

void test_crc32_one_shot(void)
{
    int32_t             size;
    uint32_t            crc32HW;
    uint32_t            crc32SW;
    int32_t             random;
    int32_t             maxSize;
    int32_t             testCaseNum;
    struct _rand_state  state;

    dbgPrint("\nTest CRC32 One shot:\n");

    maxSize = sizeof(testBuf);

    for (testCaseNum = 0; testCaseNum < 1000; testCaseNum++) {
        _srand_r(&state, osKernelGetTickCount());

        random = _rand_r(&state);
        size   = random % maxSize;

        random = _rand_r(&state);
        myMemset((uint32_t*)testBuf, random, size / 4);

        crc32HW = crc32_hw_calc(testBuf, size);
        crc32SW = crc32_sw_calc(0xFFFFFFFF, testBuf, size) ^ 0xFFFFFFFF;
        if (crc32HW != crc32SW) {
            dbgPrint("\t Test case %d Failed!: value=0x%08x, size=%d, crc32SW=0x%08x, crc32HW=0x%08x\n",
                            testCaseNum, random, size, crc32SW, crc32HW);
            testDump("CRC32 source data", testBuf, size);
            return;
        } else {
            dbgPrint("\t Test case %d Passed!: value=0x%08x, size=%d, crc32SW=0x%08x, crc32HW=0x%08x\n",
                            testCaseNum, random, size, crc32SW, crc32HW);
        }
    }

    dbgPrint("\t Test %d cases had been Passed already!\n", testCaseNum);
}

void test_crc32_data_inject(void)
{
    int32_t             i;
    int32_t             size;
    uint32_t            crc32HW;
    uint32_t            crc32SW;
    int32_t             random;
    int32_t             maxSize;
    int32_t             testCaseNum;
    int32_t             aligned;
    int32_t             residue;
    uint32_t*           word;
    struct _rand_state  state;

    dbgPrint("\nTest CRC32 with Data multi inject:\n");

    maxSize = sizeof(testBuf);

    for (testCaseNum = 0; testCaseNum < 1000; testCaseNum++) {
        _srand_r(&state, osKernelGetTickCount());

        random  = _rand_r(&state);
        size    = (random % maxSize);
        aligned = size / 4;
        residue = size % 4;

        random = _rand_r(&state);
        myMemset((uint32_t*)testBuf, random, aligned);

        crc32SW = crc32_sw_calc(0xFFFFFFFF, testBuf, size) ^ 0xFFFFFFFF;

        crc32_hw_reset();
        word = (uint32_t*)testBuf;
        for (i = 0; i < aligned; i++) {
            crc32_hw_inject(&word[i], 1);
        }

        crc32HW = crc32_hw_get();
        if (residue) {
            crc32HW = crc32_hw_calc_residue(crc32HW, &testBuf[aligned * 4], residue);
        }
        crc32HW ^= 0xFFFFFFFF;

        if (crc32HW != crc32SW) {
            dbgPrint("\t Test case %d Failed!: value=0x%08x, size=%d, crc32SW=0x%08x, crc32HW=0x%08x\n",
                            testCaseNum, random, size, crc32SW, crc32HW);
            testDump("CRC32 source data", testBuf, size);
            return;
        } else {
            dbgPrint("\t Test case %d Passed!: value=0x%08x, size=%d, crc32SW=0x%08x, crc32HW=0x%08x\n",
                            testCaseNum, random, size, crc32SW, crc32HW);
        }
    }

    dbgPrint("\t Test %d cases had been Passed already!\n", testCaseNum);
}



void test_crc32(void)
{
    //int32_t i;

    dbgPrint("\nTest CRC32:\n");


    test_crc32_one_shot();
    test_crc32_data_inject();

}
#endif
