#ifndef __TEST_H__
#define __TEST_H__

#include <stdint.h>
#include <stdio.h>


#define ERR_TEST_FAILED          -1
#define QUIT                      2


//#define TEST_CASE_DEF(ptr)         do {                         \
//                __asm (                                         \
//                ".pushsection  test_cases   "                     \
//                "DCD ptr                  "                     \
//                ".popshection);             "                     \
//            } while (0)

struct test_case {
    char        item_id;
    char*       prompt;
    void*       param;
    int32_t     err_code;
    int32_t     (*test_func)(int argc, char** argv);
    int32_t     func_idx;
} ;


#define test_print(...)             printf(__VA_ARGS__)

//struct test_case test_case_quit;


void osStart(void);

void usage_show(struct test_case** list, size_t size);
int32_t test_case_exec(struct test_case** list, size_t size);


#endif /* __TEST_H__ */

