/*
 * Copyright (c) 2013-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -----------------------------------------------------------------------------
 *
 * $Revision:   V5.1.0
 *
 * Project:     CMSIS-RTOS RTX
 * Title:       RTX Configuration
 *
 * -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include "cmsis_compiler.h"
#include "rtx_os.h"

#define THREAD_MAX_NUM     2
 
//extern osThreadId_t thread_id_main; 
//extern osThreadId_t thread_id_protocol;

//static osThreadId_t* list_thread[] = {
//    &thread_id_main,
//    &thread_id_protocol,
//} ;

//static uint32_t list_stack_free[THREAD_MAX_NUM] = {0,} ;

//static char* list_thread_name[] = {
//    "main    ",
//    "protocol",
//} ;

// OS Idle Thread
__WEAK __NO_RETURN void osRtxIdleThread (void *argument) {
  (void)argument;
//  uint32_t stack;
  for (;;) {
//    int32_t i;
//    
//    printf("[Idle] All of the free space of thread stack:\n");  
//    for (i = 0; i < THREAD_MAX_NUM; i++) {
//        list_stack_free[i] = osThreadGetStackSpace(list_thread[i]);
//        printf("\t %s: %d\n", list_thread_name[i], list_stack_free[i]);
//    }        
    __WFE(); 
  }
}
 
// OS Error Callback function
__WEAK uint32_t osRtxErrorNotify (uint32_t code, void *object_id) {
  (void)object_id;

  switch (code) {
    case osRtxErrorStackUnderflow:
      // Stack overflow detected for thread (thread_id=object_id)
      break;
    case osRtxErrorISRQueueOverflow:
      // ISR Queue overflow detected when inserting object (object_id)
      break;
    case osRtxErrorTimerQueueOverflow:
      // User Timer Callback Queue overflow detected for timer (timer_id=object_id)
      break;
    case osRtxErrorClibSpace:
      // Standard C/C++ library libspace not available: increase OS_THREAD_LIBSPACE_NUM
      break;
    case osRtxErrorClibMutex:
      // Standard C/C++ library mutex initialization failed
      break;
    default:
      // Reserved
      break;
  }
  for (;;) {}
//return 0U;
}
