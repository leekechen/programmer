#include <cmsis_os2.h>
#include "test.h"
#include "lcd.h"
#include "error.h"
#include "key.h"
#include "buzzer.h"

#include "dbg_print.h"

#define TEST_CASE_NUM                         (sizeof(test_cases)/sizeof(struct test_case*))
#define TEST_FUNCTION_NUM                     (sizeof(test_function_cases)/sizeof(struct test_case*))

extern struct     test_case test_case_lcd;
extern struct     test_case test_case_power;
extern struct     test_case test_case_buzzer;
extern struct     test_case test_case_led;
extern struct     test_case test_case_key;
extern struct     test_case test_case_extension;
extern struct     test_case test_case_flash;
extern struct     test_case test_case_fram;
extern struct     test_case test_case_vs;
extern struct     test_case test_case_program_interface;
extern struct     test_case test_case_temperature;


extern struct     test_case test_case_fram;


struct test_case test_case_quit = {
    'q',
    "Quit test",
    NULL,
    QUIT,
    NULL,
    0
};

static struct test_case* test_cases[] = { 
    &test_case_lcd,
    &test_case_power,
    &test_case_buzzer,
    &test_case_led,
    &test_case_key,
    &test_case_extension,
    &test_case_flash,
    &test_case_fram,
    &test_case_vs,
    &test_case_program_interface,
    &test_case_temperature,
    &test_case_quit
};


static struct test_case* test_function_cases[] = { 
    &test_case_fram,
    &test_case_quit
};


void usage_show(struct test_case** list, size_t size)
{
    int32_t i;

    test_print("\n[Test]  Test Case List:\n");
    for (i = 0; i < size; i++){
        test_print("\t[%c]: %s\n", list[i]->item_id, list[i]->prompt);
    }
    test_print("\tplease select Test Case:\n");    
}

int32_t test_case_exec(struct test_case** list, size_t size)
{
    int32_t i;
    int32_t err;
    int32_t ch;
    
    usage_show(list, size);  
    ch = getchar();
    test_print("input: %c\n", ch); 
    for (i = 0; i < size; i++){
        if (ch == list[i]->item_id){
            err = list[i]->err_code;
            if(list[i]->item_id == 'q'){
                return err;
            }
            list[i]->err_code = list[i]->test_func(list[i]->func_idx, NULL);
            if (list[i]->err_code==ERR_TEST_FAILED){
                test_print("[Test] Failed to test %s\n", list[i]->prompt);
            } 
            else if(list[i]->err_code==ERR_OK){
                test_print("[Test] Passed to test %s\n", list[i]->prompt);
            }
            return err;
        }
    }
    test_print("[Test] error input '%c'\n", ch);
    return 0;
}

int32_t test_main(int argc, char** argv)
{ 
    int32_t ch,err;
    
    lcd_init();
    key_init();
    dbg_init();
    buzzer_init();
    test_print("[Test] CSWrite 3.0 Test Framework\n");    
    while (1) {

        test_print("[Test] Please select the test mode:\n\t h->hardware test; \n\t s->system test\n");  
        while(1){
            ch = getchar();
            test_print("input:  %c\n", ch); 
            if(ch == 'h'){                            
                test_print("\thardware test start\n");                                         
                break;
            }
            else if(ch == 's'){                    
                test_print("\tsystem test start\n");                                      
                break;
            }
            else{
                test_print("\tYour input is illegal characters, please enter again\n");    
            }
        }   
        if(ch == 'h'){
        
            while(1){
                err = test_case_exec(test_cases, TEST_CASE_NUM);
                if(err==QUIT) 
                {
                    break;
                }
            }
        }
        else if(ch == 's')
        {
            while(1){
                err = test_case_exec(test_function_cases, TEST_FUNCTION_NUM);
                if(err==QUIT) 
                {
                    break;
                }
            }
        }
    }  
}

void thread_main(void *arg)
{  
//    test_main(0, NULL);
    test_collection();
    while (1){
        ;
    }
}

void osStart(void)
{

    osThreadId_t threadId;

    threadId = osThreadNew(thread_main, NULL, NULL);
    if (threadId == NULL){
        return;
    }

    if(osKernelGetState() == osKernelInactive){
        osKernelInitialize();
    }

    if (osKernelGetState() == osKernelReady){ 
        osKernelStart();
    }
}

