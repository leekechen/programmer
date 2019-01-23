#include <stdint.h>
#include "key.h"
#include "test.h"
#include "error.h"
 
#define KEY_NUM         3
#define MODULE_DEBUG    1 
 
#include "dbg_print.h"
 
 int32_t test_led_handle(int argc, char* argv[])
 {
     int32_t i;
     int32_t err;
     struct key_event key;
     int32_t key_code;
     int32_t key_select[KEY_NUM] = {KEY_CODE_PROGRAM, KEY_CODE_FUNC, KEY_CODE_PROGRAM};
          
     dbg_print("[Test] Test Key\n");
 
     for (i = 0; i < KEY_NUM; i++) 
     {
         switch (key_select[i]) 
         {
         case KEY_CODE_PROGRAM:
             if (i == 0) 
             {
                 dbg_print("\t Please Press \"Program Key\", it will wait for 1 second\n");
             } 
             else 
             {
                 dbg_print("\t Please sigal \"Program Machine Start Pin\", it will wait for 1 second\n");
             }
             break;
         case KEY_CODE_FUNC:
                 dbg_print("\t Please sigal \"Function Key\", it will wait for 1 second\n");
             break;
         }
         
         err = key_waiting(&key, osWaitForever);
         if (err) 
         {
             dbg_print("\t Failed! Can't get the \"Program Key\"\n");
         } 
         else 
         {
             switch (key.code) 
             {
             case KEY_CODE_PROGRAM:
                 if (key_select[i] != KEY_CODE_PROGRAM) 
                 {
                     dbg_print("\t Failed! got invalid key, key code:0x%x\n", key_code);          
                     break;
                 } 
                 else 
                 {
                     dbg_print("\t Ok! got key pressed\n"); 
                 }
                 break;
             case KEY_CODE_FUNC: 
                 if (key_select[i] != KEY_CODE_FUNC) 
                 {
                     dbg_print("\t Failed! got invalid key, key code:0x%x\n", key_code); 
                     break;
                 } 
                 else 
                 {
                     dbg_print("\t Ok! got key pressed\n"); 
                 }
                 break;   
             default:        
                 dbg_print("\t Test case had some error!\n");            
                 break;
             }
         }
     }
     return 0;
 }
 
 struct test_case test_case_key_on = {
     'k',
     "test key start",
     NULL,
     ERR_OK,
     test_led_handle,
     0
 };
 
 struct test_case test_case_key_quit = {
     'q',
     "test key quit",
     NULL,
     QUIT,
     NULL,
     0
 };
 
 static struct test_case *test_cases[] = {
     &test_case_key_on,
     &test_case_key_quit,
 };
 
 static int32_t test_key(int argc, char* argv[])
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
 
 struct test_case test_case_key = {
     '5',
     "Test key",
     NULL,
     ERR_OK,
     test_key,
     0
 };


 
