#ifndef __HID_H__
#define __HID_H__

#include <stdint.h>

#include "USBD_Config_HID_0.h"



#define HID_DEVICE_BUF_SIZE     (USBD_HID0_IN_REPORT_MAX_SZ*5)


int32_t usb_hid_init(void);
int32_t hid_read(uint8_t* buf, int32_t len);
int32_t hid_write(const uint8_t* data, int32_t len);

#endif /* __HID_H__ */
