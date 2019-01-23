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
#include "gui_main.h"
#include "tlv.h"
#include "cswrite_cfg.h"
#include "power.h"
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0    (ID_PAGE_CSWRITE + 0x00)
#define ID_TEXT_0      (ID_PAGE_CSWRITE + 0x01)
#define ID_TEXT_1      (ID_PAGE_CSWRITE + 0x02)
#define ID_TEXT_2      (ID_PAGE_CSWRITE + 0x03)
#define ID_TEXT_3      (ID_PAGE_CSWRITE + 0x04)
#define ID_TEXT_4      (ID_PAGE_CSWRITE + 0x05)
#define ID_TEXT_5      (ID_PAGE_CSWRITE + 0x06)
#define ID_TEXT_6      (ID_PAGE_CSWRITE + 0x07)
#define ID_TEXT_7      (ID_PAGE_CSWRITE + 0x08)
#define ID_TEXT_8      (ID_PAGE_CSWRITE + 0x09)
#define ID_TEXT_9      (ID_PAGE_CSWRITE + 0x0A)
#define ID_TEXT_10     (ID_PAGE_CSWRITE + 0x0B)
#define ID_TEXT_11     (ID_PAGE_CSWRITE + 0x0C)
#define ID_TEXT_12     (ID_PAGE_CSWRITE + 0x0D)
#define ID_TEXT_13     (ID_PAGE_CSWRITE + 0x0E)
#define ID_TEXT_14     (ID_PAGE_CSWRITE + 0x0F)
#define ID_TEXT_15     (ID_PAGE_CSWRITE + 0x10)
#define ID_TEXT_16     (ID_PAGE_CSWRITE + 0x11)
#define ID_TEXT_17     (ID_PAGE_CSWRITE + 0x12)
#define ID_TEXT_18     (ID_PAGE_CSWRITE + 0x13)
#define ID_TEXT_19     (ID_PAGE_CSWRITE + 0x14)
#define ID_TEXT_20     (ID_PAGE_CSWRITE + 0x15)


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

static void initCSWrite(WM_MESSAGE* pMsg)
{
    int32_t ret;
    WM_HWIN hItem;
    struct tlv_version ver;
    uint8_t*           ptr;
    uint32_t           serial_id;
    uint8_t            id_96bits[12];
    char               buf[128];
    struct tlv_date    day;
    uint32_t           mV;
    //
    // Initialization of 'BootloaderVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    ver.type = TLV_DESC_VERSION_TYPE_BOOT;
    ret = cswrite_get_version(&ver);
    if (!ret) {
        ptr = (uint8_t*)ver.value;
        snprintf(buf, sizeof(buf), "%d.%d.%d", ptr[1], ptr[2], ptr[3]);
        TEXT_SetText(hItem, buf);
    }
    
    //
    // Initialization of 'FirmwareVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    ver.type = TLV_DESC_VERSION_TYPE_FM;
    ret = cswrite_get_version(&ver);
    if (!ret) {
        ptr = (uint8_t*)ver.value;
        snprintf(buf, sizeof(buf), "%d.%d.%d", ptr[1], ptr[2], ptr[3]);
        TEXT_SetText(hItem, buf);
    }
    
    //
    // Initialization of 'HostVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    ver.type = TLV_DESC_VERSION_TYPE_PC;
    ret = cswrite_get_version(&ver);
    if (!ret) {
        ptr = (uint8_t*)ver.value;
        snprintf(buf, sizeof(buf), "%d.%d.%d", ptr[1], ptr[2], ptr[3]);
        TEXT_SetText(hItem, buf);
    }
    //
    // Initialization of '32Bits UniqueIDVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_9);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    ret = cswrite_cfg_get_serial_id(TLV_DESC_SERIAL_ID_TYPE_32_BITS, (uint8_t*)&serial_id);
    if (!ret) {
        snprintf(buf, sizeof(buf), "%X", serial_id);
        TEXT_SetText(hItem, buf);
    }
    //
    // Initialization of '96Val'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_11);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    ret = cswrite_cfg_get_serial_id(TLV_DESC_SERIAL_ID_TYPE_96_BITS, id_96bits);
    if (!ret) {
        int32_t i;
        char tmp[32];
        ptr = (uint8_t*)id_96bits;
        memset(buf, 0, sizeof(buf));
        for (i = 0; i < sizeof(id_96bits); i++) {
            snprintf(tmp, sizeof(tmp), "%02X", id_96bits[i]);
            strcat(buf, tmp);
        }
        TEXT_SetText(hItem, buf);
    }
    //
    // Initialization of 'ActiveVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_14);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    ret = cswrite_cfg_set_active_date(&day);
    if (!ret) {
        snprintf(buf, sizeof(buf), "20%d/%d/%d", day.year, day.month, day.date);
        TEXT_SetText(hItem, buf);
    }
    //
    // Initialization of 'VddVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_16);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    ret = power_voltage_get(TLV_DESC_VOLTAGE_TYPE_VDD, &mV);
    if (!ret) {
        snprintf(buf, sizeof(buf), "%d", mV);
        TEXT_SetText(hItem, buf);
    }    
    //
    // Initialization of 'VppVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_18);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    ret = power_voltage_get(TLV_DESC_VOLTAGE_TYPE_VPP, &mV);
    if (!ret) {
        snprintf(buf, sizeof(buf), "%d", mV);
        TEXT_SetText(hItem, buf);
    }
    //
    // Initialization of 'TotalVal'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_20);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetText(hItem, "9999999999");

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
    // Initialization of 'Firmware:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00804000));
    //
    // Initialization of 'Host:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00804000));
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
    // Initialization of '96Bits:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_10);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00804000));
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
    // Initialization of 'Vdd(mV):'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_15);
    TEXT_SetFont(hItem, GUI_FONT_13B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00804000));
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
    // USER START (Optionally insert additional code for further widget initialization)
    initCSWrite(pMsg);
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