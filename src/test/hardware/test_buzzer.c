#include <stdint.h>
#include "buzzer.h"
#include "error.h"
#include "test.h"


int32_t test_buzzer_seton(int argc, char* argv[])
{
    uint8_t i;
    int32_t ch,err,voltage,value[4]={0,}; 
    test_print("\tPlease enter the buzzer time you want, the unit is milliseconds\n");
    test_print("\tIf you want to Back at the next higher level, please enter the 'q'\n");
    while(1){
        i = 0;
        do{
            ch = getchar();
            if(ch == 'q'){
                return -2;
            }   
            else if((ch>='0')&&(ch<='9')){
                value[i++] = ch-'0';    
            }                  
        }while(ch != '\n');
        switch(i){
            case 1:
            voltage = value[0];
            break;
            case 2:
            voltage = value[0]*10+value[1];
            break;
            case 3:
            voltage = value[0]*100+value[1]*10+value[2];
            break;
            case 4:
            voltage = value[0]*1000+value[1]*100+value[2]*10+value[3];
            break;
            default:
            break;
        }
        test_print("input: %d ms\n",voltage);
        if((voltage<10000)&&(voltage>0)){
            buzzer_beep(voltage); 
            test_print("\tPlease check whether the buzzer is according to the set standards.\n");
            test_print("\tPlease input test is pass[p] or fail[f].\n");
            while(1){
                ch = getchar();
                test_print("input:  %c\n", ch); 
                if(ch == 'p'){
                    err = ERR_OK;                  
                    test_print("\tpassed to test buzzer on\n");                                         
                    break;
                }
                else if(ch == 'f'){
                    err = ERR_TEST_FAILED;                
                    test_print("\tFailed to test buzzer on\n");                                      
                    break;
                }
                else{
                    test_print("\tYour input is illegal characters, please enter again\n");    
                }
            }               
        }
        else{
            test_print("\t[Test] error input '%d'\n", voltage);
        }
    } 
    return err;
    
}


struct test_case test_buzzer_on = {
    'o',
    "test buzzer on",
    NULL,
    ERR_OK,
    test_buzzer_seton,
    0
};

struct test_case test_buzzer_quit = {
    'q',
    "test buzzer quit",
    NULL,
    QUIT,
    NULL,
    0
};

static struct test_case *test_cases[] = {
    &test_buzzer_on,
    &test_buzzer_quit
};

static int32_t test_buzzer(int argc, char* argv[])
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

struct test_case test_case_buzzer = {
    '3',
    "test buzzer",
    NULL,
    ERR_OK,
    test_buzzer,
    0
};







