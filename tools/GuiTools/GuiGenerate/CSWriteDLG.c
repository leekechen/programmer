/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.44                          *
*        Compiled Nov 10 2017, 08:53:57                              *
*        (c) 2017 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
#include <stdio.h>
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_TEXT_0 (GUI_ID_USER + 0x01)
#define ID_TEXT_1 (GUI_ID_USER + 0x02)
#define ID_TEXT_2 (GUI_ID_USER + 0x03)
#define ID_TEXT_3 (GUI_ID_USER + 0x04)
#define ID_TEXT_4 (GUI_ID_USER + 0x05)
#define ID_TEXT_5 (GUI_ID_USER + 0x06)
#define ID_TEXT_6 (GUI_ID_USER + 0x07)
#define ID_TEXT_7 (GUI_ID_USER + 0x08)
#define ID_TEXT_8 (GUI_ID_USER + 0x09)
#define ID_TEXT_9 (GUI_ID_USER + 0x0A)
#define ID_TEXT_10 (GUI_ID_USER + 0x0B)
#define ID_TEXT_11 (GUI_ID_USER + 0x0C)
#define ID_TEXT_12 (GUI_ID_USER + 0x0D)
#define ID_TEXT_13 (GUI_ID_USER + 0x0E)
#define ID_TEXT_14 (GUI_ID_USER + 0x0F)
#define ID_TEXT_15 (GUI_ID_USER + 0x10)
#define ID_TEXT_16 (GUI_ID_USER + 0x11)
#define ID_TEXT_17 (GUI_ID_USER + 0x12)
#define ID_TEXT_18 (GUI_ID_USER + 0x13)
#define ID_TEXT_19 (GUI_ID_USER + 0x14)
#define ID_TEXT_20 (GUI_ID_USER + 0x15)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "CSWrite", ID_WINDOW_0, 4, 5, 320, 199, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Version", ID_TEXT_0, 2, 2, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Bootloader:", ID_TEXT_1, 16, 19, 70, 17, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "bVal", ID_TEXT_2, 85, 19, 47, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Firmware:", ID_TEXT_3, 148, 19, 60, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "fVal", ID_TEXT_4, 224, 19, 80, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Host:", ID_TEXT_5, 16, 37, 47, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "hVal", ID_TEXT_6, 85, 37, 49, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "UniqueID", ID_TEXT_7, 5, 78, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "32Bits", ID_TEXT_8, 16, 95, 48, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "12345678", ID_TEXT_9, 85, 95, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "96Bits:", ID_TEXT_10, 148, 95, 53, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "96Val", ID_TEXT_11, 224, 95, 94, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Status", ID_TEXT_12, 3, 113, 58, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Actived:", ID_TEXT_13, 148, 37, 50, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "aVal", ID_TEXT_14, 224, 37, 80, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Vdd(mV):", ID_TEXT_15, 16, 132, 44, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "dVal", ID_TEXT_16, 85, 132, 47, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Vpp(mV):", ID_TEXT_17, 148, 132, 52, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "8500", ID_TEXT_18, 224, 132, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Total Program Num:", ID_TEXT_19, 16, 57, 124, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "TotalVal", ID_TEXT_20, 148, 57, 80, 20, 0, 0x64, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
static void keyProcess(WM_MESSAGE* pMsg)
{
    int index;
    WM_HWIN hParent;
    WM_KEY_INFO* keyInfo;

    keyInfo = (WM_KEY_INFO*)pMsg->Data.p;

    printf("[GUI] CSWrite , %s(),Msg(Key=%d,PressedCnt=%d)\n", __func__, keyInfo->Key, keyInfo->PressedCnt);

    WM_DefaultProc(pMsg);
}

// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Version'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00408000));
    //
    // Initialization of 'Bootloader:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00804000));
    //
    // Initialization of 'bVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetText(hItem, "3.99.99");
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    //
    // Initialization of 'Firmware:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00804000));
    //
    // Initialization of 'fVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
    TEXT_SetText(hItem, "3.0.0");
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    //
    // Initialization of 'Host:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00804000));
    //
    // Initialization of 'hVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
    TEXT_SetText(hItem, "3.99.99");
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    //
    // Initialization of 'UniqueID'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00408000));
    //
    // Initialization of '32Bits'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00804000));
    //
    // Initialization of '12345678'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_9);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    //
    // Initialization of '96Bits:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_10);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00804000));
    //
    // Initialization of '96Val'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_11);
    TEXT_SetText(hItem, "0123456789AB");
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    //
    // Initialization of 'Status'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_12);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00408000));
    //
    // Initialization of 'Actived:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_13);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00804000));
    //
    // Initialization of 'aVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_14);
    TEXT_SetText(hItem, "2019/1/20");
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    //
    // Initialization of 'Vdd(mV):'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_15);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00804000));
    //
    // Initialization of 'dVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_16);
    TEXT_SetText(hItem, "5000");
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    //
    // Initialization of 'Vpp(mV):'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_17);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00804000));
    //
    // Initialization of '8500'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_18);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    //
    // Initialization of 'Total Program Num:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_19);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00804000));
    //
    // Initialization of 'TotalVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_20);
    TEXT_SetText(hItem, "9999999999");
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  // USER START (Optionally insert additional message handling)
  case WM_KEY:
    keyProcess(pMsg);
    break;
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateCSWrite
*/
WM_HWIN CreateCSWrite(void);
WM_HWIN CreateCSWrite(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
