/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.40 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_X_OS.C
Purpose     : This file provides emWin Interface with FreeRTOS
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "GUI.h"
    
    /* FreeRTOS include files */
#include "cmsis_os2.h"
    
/*********************************************************************
*
* Global data
*/
//static osMutexId osMutex;
//static osSemaphoreId osSemaphore;
static osMutexId_t mutex_id;
static osSemaphoreId_t sem_id;
/*********************************************************************
*
* Timing:
* GUI_X_GetTime()
* GUI_X_Delay(int)

Some timing dependent routines require a GetTime
and delay function. Default time unit (tick), normally is
1 ms.
*/

int GUI_X_GetTime(void)
{
    return osKernelGetTickCount();
}

void GUI_X_Delay(int ms)
{
    osDelay( ms );
}

/*********************************************************************
*
* GUI_X_Init()
*
* Note:
* GUI_X_Init() is called from GUI_Init is a possibility to init
* some hardware which needs to be up and running before the GUI.
* If not required, leave this routine blank.
*/

void GUI_X_Init(void) 
{
}


/*********************************************************************
*
* GUI_X_ExecIdle
*
* Note:
* Called if WM is in idle state
*/

void GUI_X_ExecIdle(void)
{
    osThreadYield();
}

/*********************************************************************
*
* Multitasking:
*
* GUI_X_InitOS()
* GUI_X_GetTaskId()
* GUI_X_Lock()
* GUI_X_Unlock()
*
* Note:
* The following routines are required only if emWin is used in a
* true multi task environment, which means you have more than one
* thread using the emWin API.
* In this case the
* #define GUI_OS 1
* needs to be in GUIConf.h
*/

/* Init OS */
void GUI_X_InitOS(void)
{ 
    mutex_id = osMutexNew(NULL);
    
    sem_id = osSemaphoreNew(0xFFFF, 0, NULL);
}

void GUI_X_Unlock(void)
{   
    osStatus_t ret; 
    ret = osMutexRelease(mutex_id);
    if (ret) {
        printf("%s(), osMutexRelease(%p) ret=%x\n", __func__, mutex_id, ret);
    }
}

void GUI_X_Lock(void)
{
    osStatus_t ret; 
    ret = osMutexAcquire(mutex_id, osWaitForever);
    if (ret) {
        printf("%s(), osMutexAcquire(%p) ret=%x\n", __func__, mutex_id, ret);
    }
}

/* Get Task handle */
U32 GUI_X_GetTaskId(void) 
{ 
    return (U32) osThreadGetId();
}


void GUI_X_WaitEvent (void) 
{
    osStatus_t ret; 
    ret = osSemaphoreAcquire(sem_id , osWaitForever) ;
    if (ret) {
        printf("%s(), osSemaphoreAcquire(%p) ret=%x\n", __func__, sem_id, ret);
    }
}


void GUI_X_SignalEvent (void) 
{
    osStatus_t ret; 
    ret = osSemaphoreRelease(sem_id);
    if (ret) {
        printf("%s(), osSemaphoreRelease(%p) ret=%x\n", __func__, sem_id, ret);
    }
}

/*********************************************************************
*
* Logging: OS dependent

Note:
Logging is used in higher debug levels only. The typical target
build does not use logging and does therefor not require any of
the logging routines below. For a release build without logging
the routines below may be eliminated to save some space.
(If the linker is not function aware and eliminates unreferenced
functions automatically)

*/

void GUI_X_Log (const char *s) 
{
    printf("[GUI] %s\n", s);
}
void GUI_X_Warn (const char *s) 
{

    printf("[GUI] %s\n", s);    
}
void GUI_X_ErrorOut(const char *s) 
{

    printf("[GUI] %s\n", s);    
}

/*************************** End of file ****************************/