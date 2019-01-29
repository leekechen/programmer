#include <stdint.h>
#include <cmsis_os2.h>
#include "dbg_print.h"
#include "power.h"
#include "test.h"
#include "error.h"

#define TEST_CASE_NUM    (sizeof(test_cases)/sizeof(struct test_case*))



struct tlv_voltage power_voltage;


struct test_case test_power_value = {
    'v',
    "voltage value",
    NULL,
    ERR_OK,
    NULL,
    0
};

struct test_case test_power_exit = {
    'q',
    "test quit",
    NULL,
    ERR_OK,
    NULL,
    0
};

struct test_case *test_powers[] = {
    &test_power_value,
    &test_power_exit
};


int32_t test_power_handle(int argc, char* argv[])
{
    uint8_t i;
    uint32_t vdd,vpp;
    int32_t ch,err,voltage,value[4]={0,}; 
    test_print("\tPlease input voltage value, the unit is the mV\n");
    test_print("\tIf you want to Back at the next higher level, please enter the 'q'\n");
    while(1)
    {
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
        test_print("input: %d mV\n",voltage);
        if((voltage<10000)&&(voltage>0)){
            power_voltage.value = voltage;
            if(argc){
                power_voltage.type = TLV_DESC_VOLTAGE_TYPE_VDD;
                power_voltage_set(&power_voltage);  
                power_voltage_switch(TLV_DESC_VOLTAGE_TYPE_VDD, POWER_FLAG_ON);
            }
            else{
                power_voltage.type = TLV_DESC_VOLTAGE_TYPE_VPP;
                power_voltage_set(&power_voltage);    
                power_voltage_switch(TLV_DESC_VOLTAGE_TYPE_VPP, POWER_FLAG_ON);                        
            }
            test_print("\tPlease use Oscilloscope to measure the voltage, and check the average value of Voltage\n\tand Pk-Pk value of Voltage.\n");
            test_print("\tPlease input test is pass[p] or fail[f].\n");
            while(1){
                ch = getchar();
                test_print("input:  %c\n", ch); 
                if(ch == 'p'){
                    err = ERR_OK;
                    if(argc){
                        test_print("\tpassed to test power vdd\n");                       
                    }
                    else{
                        test_print("\tpassed to test power vpp\n");    
                    }
                    break;
                }
                else if(ch == 'f'){
                    err = ERR_TEST_FAILED;
                    if(argc){
                        test_print("\tFailed to test power vdd\n");                       
                    }
                    else{
                        test_print("\tFailed to test power vpp\n");    
                    }
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

struct test_case test_case_power_vpp = {
    'p',
    "Vpp test",
    NULL,
    ERR_OK,
    test_power_handle,
    0
};

struct test_case test_case_power_vdd = {
    'd',
    "Vdd test",
    NULL,
    ERR_OK,
    test_power_handle,
    1
};

struct test_case test_case_power_quit = {
    'q',
    "test power quit",
    NULL,
    QUIT,
    NULL,
    0
};

static struct test_case *test_cases[] = {
    &test_case_power_vpp,
    &test_case_power_vdd,
    &test_case_power_quit
};

static int32_t test_power(int argc, char* argv[]){
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

struct test_case test_case_power = {
    '2',
    "Test Power",
    NULL,
    ERR_OK,
    test_power,
    0
} ;








