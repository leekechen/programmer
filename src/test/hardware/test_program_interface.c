#include <stdint.h>
#include "error.h"
#include "test.h"
#include "circuit_detect.h"



static int32_t program_interface_io_status_detection(int argc, char* argv[])
{
    int32_t ch,err;
    uint32_t i,channel,value[7] = {0},status_buf[7] = {0};
    test_print("\tPlease input you want to shielding the channel number.\n");
    test_print("\tPIN_VDD-->0\n\tPIN_VS-->1\n\tPIN_MISO-->2\n\tPIN_MOSI-->3\n\tPIN_CLK-->4\n\tPIN_VPP-->5\n\tPIN_REST-->6\n\tNONE-->7\n");
    while(1){
        ch = getchar();
        test_print("input:  %c\n", ch); 

        if((ch >= '0')&&(ch <= '7')){
            err = ERR_OK;  
            channel = ch - '0';  
            if(!argc){
//                circuit_detect_polling_all_adc_read(channel,value);
                for(i=0;i<7;i++){
                    if(value[i]<PROGRAM_OPEN_THRESHOLD_VALUE){             
                        status_buf[i] = 1;
                    }   
                    if(channel<7)
                    {
                        status_buf[channel] = 2;                                
                    }  
                }
                test_print("\t1:open 2:shielding 0:other\n");      
                test_print("\tPIN_VDD:  %d\n",status_buf[0]);      
                test_print("\tPIN_VS:   %d\n",status_buf[1]);                                            
                test_print("\tPIN_MISO: %d\n",status_buf[2]);    
                test_print("\tPIN_MOSI: %d\n",status_buf[3]);    
                test_print("\tPIN_CLK:  %d\n",status_buf[4]);    
                test_print("\tPIN_VPP:  %d\n",status_buf[5]);    
                test_print("\tPIN_REST: %d\n",status_buf[6]);    
                break;
            }
            else{
//                circuit_detect_polling_all_adc_read(channel,value);
                for(i=0;i<7;i++){
                    if(value[i]>PROGRAM_SHORT_THRESHOLD_VALUE){             
                        status_buf[i] = 1;
                    }
                    if(channel<7)
                    {
                        status_buf[channel] = 2;                                
                    }  
                }
                test_print("\t1:short 2:shielding 0:other\n");      
                test_print("\tPIN_VDD:  %d\n",status_buf[0]);      
                test_print("\tPIN_VS:   %d\n",status_buf[1]);                                            
                test_print("\tPIN_MISO: %d\n",status_buf[2]);    
                test_print("\tPIN_MOSI: %d\n",status_buf[3]);    
                test_print("\tPIN_CLK:  %d\n",status_buf[4]);    
                test_print("\tPIN_VPP:  %d\n",status_buf[5]);    
                test_print("\tPIN_REST: %d\n",status_buf[6]);    
                break;
            }                 
        }
        else{
            test_print("\tYour input is illegal characters, please enter again\n");    
        }
    }
    for(i=0;i<7;i++){
        if(status_buf[i] == 0){
            err = ERR_TEST_FAILED;
        }
    }
    return err;
}

static int32_t program_interface_voltage_get(int argc, char* argv[])
{
    int32_t ch,err;
    uint32_t i,value,channel;
    test_print("\tPlease input you want to get the voltage channel.\n");
    test_print("\tPIN_VDD-->0\n\tPIN_VS-->1\n\tPIN_MISO-->2\n\tPIN_MOSI-->3\n\tPIN_CLK-->4\n\tPIN_VPP-->5\n\tPIN_REST-->6\n");
    test_print("\tIf you want to quit,Please input test is pass[p] or fail[f] and exit.\n");
     while(1){
        ch = getchar();
        test_print("input:  %c\n", ch); 
        if((ch >= '0')&&(ch < '7')){
            err = ERR_OK;  
            channel = ch - '0';  
            i = 1<<channel;
//            circuit_detect_pin_get_voltage_value(i,&value);
            switch(channel){
                case 0:
                 test_print("\tPIN_VDD voltage:  %dmV\n",value);   
                break;
                case 1:
                 test_print("\tPIN_VS voltage:  %dmV\n",value);   
                break;
                case 2:
                 test_print("\tPIN_MISO voltage:  %dmV\n",value);   
                break;
                case 3:
                 test_print("\tPIN_MOSI voltage:  %dmV\n",value);   
                break;
                case 4:
                 test_print("\tPIN_CLK voltage:  %dmV\n",value);   
                break;
                case 5:
                 test_print("\tPIN_VPP voltage:  %dmV\n",value);   
                break;
                case 6:
                 test_print("\tPIN_REST voltage:  %dmV\n",value);   
                break;
                default:
                break;                
            }
        }
        else if(ch == 'p'){
            err = ERR_OK;          
            test_print("\tpassed to test program io voltage get\n");                                 
            break;
        }
        else if(ch == 'f'){
            err = ERR_TEST_FAILED;          
            test_print("\tFailed to test program io voltage get\n");                               
            break;
        }
        else{
            test_print("\tYour input is illegal characters, please enter again\n");                  
        }
     }
     return ERR_OK;
}

struct test_case test_case_program_quit = {
    'q',
    "test program quit",
    NULL,
    QUIT,
    NULL,
    0
};

struct test_case test_case_program_open_detect = {
    'o',
    "test program io open detection",
    NULL,
    ERR_OK,
    program_interface_io_status_detection,
    0
};

struct test_case test_case_program_short_detect = {
    's',
    "test program io short detection",
    NULL,
    ERR_OK,
    program_interface_io_status_detection,
    1
};

struct test_case test_case_program_get_voltage = {
    'v',
    "test program io voltage get",
    NULL,
    ERR_OK,
    program_interface_voltage_get,
    0
};

static struct test_case *test_cases[] = {
    &test_case_program_open_detect,
    &test_case_program_short_detect,
    &test_case_program_get_voltage,
    &test_case_program_quit
};

static int32_t test_program(int argc, char* argv[])
{
    int32_t ret;
    while(1){
        ret = test_case_exec(test_cases, sizeof(test_cases)/sizeof(struct test_case*));           
        if(ret==QUIT){ 
            break;
        }
    }
    return ret;   
}

struct test_case test_case_program_interface = {
    'a',
    "test program interface",
    NULL,
    ERR_OK,
    test_program,
    0
};



