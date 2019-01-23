#include <stdint.h>
#include "cmsis_os2.h"
#include "st7789v.h"
#include "wm.h"
#include "key.h"
#include "gui_main.h"

#define MODULE_DEBUG       1
#define MODULE_ERROR       1

#include "dbg_print.h"


static WM_HWIN hWidgetCur;

void gui_key_process(struct key_event* key)
{
    WM_MESSAGE  msg;
    WM_KEY_INFO wm_key;

    if (key->state == KEY_STATE_PRESS) {
        switch (key->code) {
        case KEY_CODE_FUNC:
            GUI_SendKeyMsg(GUI_KEY_PGDOWN, 1);
            break;
        case KEY_CODE_PROGRAM:
            GUI_SendKeyMsg(GUI_KEY_ENTER, 1);
            break;
        default:
            GUI_SendKeyMsg(key->code, 1);
            break;
        }
        GUI_SendKeyMsg(KEY_CODE_GUI_HEX_UPGRADED, 1);
    }
}

void thread_gui(void* arg)
{
    int32_t index;
    WM_HWIN hWidgetCur;
    
    GUI_Init();
    GUI_ClearKeyBuffer();
    GUI_Clear();

    while (1) {
        hWidgetCur = CreateFramewin();
        index = GUI_ExecCreatedDialog(hWidgetCur);

        dbg_print("%s(),%d,hWidgetCur=0x%x\n", __func__, __LINE__, index);
    }
}

int32_t gui_init(void)
{
    osThreadId_t thread_id_gui;

    thread_id_gui = osThreadNew(thread_gui, NULL, NULL);
    if (thread_id_gui == NULL) {
        return -1;
    }

    return 0;
}