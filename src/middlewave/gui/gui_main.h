#ifndef __GUI_H__
#define __GUI_H__

#include "key.h"
#include "DIALOG.h"

#define ID_PAGE_SUMMARY         (GUI_ID_USER     + 0x20)
#define ID_PAGE_HEX             (ID_PAGE_SUMMARY + 0x20)
#define ID_PAGE_CSWRITE         (ID_PAGE_HEX     + 0x20)
#define ID_PAGE_CHIP            (ID_PAGE_CSWRITE + 0x20)
#define ID_PAGE_CONFIG          (ID_PAGE_CHIP    + 0x20)




WM_HWIN CreateFramewin(void);
WM_HWIN CreateSummary(void);
WM_HWIN CreateHex(void);
WM_HWIN CreateCSWrite(void);
WM_HWIN CreateChip(void);
WM_HWIN CreateConfig(void);

int32_t gui_init(void);
void gui_key_process(struct key_event* key);

#endif /* __GUI_H__ */

