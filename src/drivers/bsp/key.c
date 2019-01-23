#include <string.h>
#include <cmsis_os2.h>
#include <stm32f4xx_hal.h>
#include "error.h"
#include "key.h"

#define MODULE_DEBUG       0
#define MODULE_ERROR       1

#include "dbg_print.h"


#define KEY_NUM                 2


#define __HAL_GPIO_EXTI_MASK_IT(__EXTI_LINE__)   (EXTI->IMR &= ~(__EXTI_LINE__))
#define __HAL_GPIO_EXTI_UNMASK_IT(__EXTI_LINE__) (EXTI->IMR |= (__EXTI_LINE__) )

struct key_info {
    uint32_t        key;
    uint32_t        state;   
    osTimerId_t     timer;
} ;

struct keyboard_info {
    struct key_info         keys[KEY_NUM];    
    osMessageQueueId_t      msg_queue1;
    osMessageQueueId_t      msg_queue2;
} ;

static struct keyboard_info keyboard;

static uint32_t get_state(uint32_t key_code)
{
    int32_t i;

    for (i = 0; i < KEY_NUM; i++) {
        if (keyboard.keys[i].key == key_code) {
            return keyboard.keys[i].state;
        }
    }
    return 0;
}

static void set_state(uint32_t key_code, uint32_t state)
{
    int32_t i;

    for (i = 0; i < KEY_NUM; i++) {
        if (keyboard.keys[i].key == key_code) {
            keyboard.keys[i].state = state;
            return;
        }
    }
}


static osTimerId_t get_timer(uint32_t key_code)
{
    int32_t i;

    for (i = 0; i < KEY_NUM; i++) {
        if (keyboard.keys[i].key == key_code) {
            return keyboard.keys[i].timer;
        }
    }
    return 0;
}

static void timer_callback(void *arg)
{
    uint32_t         key_code;
    struct key_event evt;
    uint32_t         pin;
    GPIO_TypeDef*    port;
    uint32_t         state;

    key_code  = (uint32_t)arg;
    switch (key_code) {
    case KEY_CODE_FUNC:
        port = KEY_MODE_GPIO_Port;
        pin  = KEY_MODE_Pin;
        break;
    case KEY_CODE_PROGRAM:
        port = START_GPIO_Port;
        pin  = START_Pin;
        break;
    default:
        break;
    } 
    
    evt.code  = key_code;
    evt.state = HAL_GPIO_ReadPin(port, pin);
    state     = get_state(key_code);
    dbg_print("[Key] state: cur:%d,last=%d\n", evt.state, state);
    if (state !=  evt.state) {
        /* If we got the key state is same to last, which means it's a jitter.  */
        set_state(key_code, evt.state);        
        osMessageQueuePut(keyboard.msg_queue2, &evt, 0, osWaitForever);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{  
    int32_t ret;
    struct key_event evt;
    
    switch (GPIO_Pin) {
    case KEY_MODE_Pin:
        evt.code  = KEY_CODE_FUNC;
        break;
    case START_Pin:
        evt.code  = KEY_CODE_PROGRAM;
        break;
    default:
        return;
    }

    ret = osMessageQueuePut(keyboard.msg_queue1, &evt, 0, 0);
    if (ret) {
        err_print("osMessageQueuePut() ret=%d\n", ret);
    }
}

void thread_key(void *arg)
{ 
    int32_t            ret;
    int32_t            state;
    struct key_event   evt;
    osTimerId_t        timer;

    while (1) {
        ret = osMessageQueueGet(keyboard.msg_queue1, &evt, NULL, osWaitForever);
        if (ret) {
            if (ret == osErrorTimeout) {
                err_print("osMessageQueueGet() osErrorTimeout\n");
             } else {
                err_print("osMessageQueueGet() ret=%d\n", ret);
            }
        }

        timer = get_timer(evt.code);

        state = osTimerIsRunning(timer);
        if (state) {                
            dbg_print("[Key] last signal edge is jitter, skip it\n");
            /* last signal edge is jitter, skip it */
            osTimerStop(timer);
        }
            
        /* Start the jitter-filter timer */            
        dbg_print("[Key] start jitter-filter timer\n");
        osTimerStart(timer, KEY_PULSE_MINI_THRESHOLD);
    }
}


int32_t key_init(void)
{
    osThreadId_t        thread_id_key;

    /* The Start(EXTI 15) / Mode(EXTI14) had been configurated trigger by rise/fall edge*/

    keyboard.msg_queue1 = osMessageQueueNew(50, sizeof(struct key_event), NULL);
    if (keyboard.msg_queue1 == NULL) {
        err_print("osMessageQueueNew() failed\n");
        return ERR_SYS_OS;
    }

    keyboard.msg_queue2 = osMessageQueueNew(50, sizeof(struct key_event), NULL);
    if (keyboard.msg_queue2 == NULL) {
        err_print("osMessageQueueNew() failed\n");
        return ERR_SYS_OS;
    }

    keyboard.keys[0].key   = KEY_CODE_PROGRAM;
    keyboard.keys[0].timer = osTimerNew(timer_callback, osTimerOnce, (void*)KEY_CODE_PROGRAM, NULL);
    keyboard.keys[0].state = KEY_STATE_RELEASE;
  
    keyboard.keys[1].key   = KEY_CODE_FUNC;
    keyboard.keys[1].timer = osTimerNew(timer_callback, osTimerOnce, (void*)KEY_CODE_FUNC, NULL);
    keyboard.keys[1].state = KEY_STATE_RELEASE;

    thread_id_key = osThreadNew(thread_key, NULL, NULL);
    if (thread_id_key == NULL) {
        err_print("osThreadNew() failed\n");
        return ERR_SYS_OS;
    }
    
    return ERR_OK;
}

int32_t key_waiting(struct key_event* evt, uint32_t timeout)
{ 
    int32_t            ret;

    while (1) {
        ret = osMessageQueueGet(keyboard.msg_queue2, evt, NULL, osWaitForever);
        if (ret) {
            if (ret == osErrorTimeout) {
                err_print("osMessageQueueGet() osErrorTimeout\n");
            }
            if (ret == osErrorResource) {
                err_print("osMessageQueueGet() osErrorResource\n");
            }
            if (ret == osErrorParameter) {
                err_print("osMessageQueueGet() osErrorResource\n");
            }
            continue;
        }

        break;
    }
    
    return ERR_OK;
}


int32_t key_set(struct key_event* evt)
{  
    return osMessageQueuePut(keyboard.msg_queue2, evt, 0, osWaitForever);
}
