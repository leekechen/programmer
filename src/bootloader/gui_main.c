#include <stdint.h>
#include "cmsis_os2.h"
#include "st7789v.h"
#include "wm.h"
#include "key.h"
#include "gui_main.h"

void thread_gui(void* arg)
{
    int32_t index;
    WM_HWIN hWidgetCur;
    
    GUI_Init();
    GUI_ClearKeyBuffer();
    GUI_Clear();

    while (1) {
        hWidgetCur = CreateWelcome();
        index = GUI_ExecCreatedDialog(hWidgetCur);
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

