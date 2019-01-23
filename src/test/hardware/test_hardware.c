#include "test.h"
#include "test_hardware.h"
#include "error.h"


static void prompt_show(void)
{
    test_print("\t a: Test Case Key\n"); 
    test_print("\t b: Test Case LED and Program machine Interface\n"); 
    test_print("\t c: Test Case LCD\n"); 
    test_print("\t d: Test Case Buzzer\n");
    test_print("\t e: Test Case Temperature Calibration Interface\n");
    test_print("\t f: Test Case Extension Interface\n");

}

int32_t test_hardware(int argc,  char** argv)
{
    int ch;
    
//    test_print("[Test] Hardware Test Cases:\n");

//    while (1) {
//        prompt_show();
//        
//        ch = getchar();
//        switch (ch) {
//        case 'a':
//            test_key();
//            break;
//        case 'b':
//            test_led();
//            break;
//        case 'c':
//            test_lcd();
//            break;
//        case 'd':
//            test_buzzer();
//            break;
//        case 'e':
////            test_T_cali();
//            break;
//        case 'f':
////            test_ext_interface();
//            break;
//        default:
//            continue;
//        }
//    }
    return ERR_OK;
}

struct test_case test_case_hardwware = {
    'h',
    "Test Hardware",
    NULL,
    ERR_OK,
    test_hardware
} ;


//TEST_CASE_DEF(test_case_hardwware);


//__asm (                                        
//".pushsection  .data.test_cases   \n"                     
//".long test_case_hardwware                  \n"                     
//".popsection \n");                                  
