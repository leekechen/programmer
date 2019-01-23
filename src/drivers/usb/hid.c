#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmsis_os2.h>
#include <rl_usb.h>
#include <stm32f4xx_hal.h>

#include "USBD_Config_HID_0.h"
#include "HID.h"

#define MODULE_DEBUG     1
#define MODULE_ERROR     1

#include "dbg_print.h"

struct msgObj {
    void*    pkt;
    uint32_t len;
} ;


struct hidDevcie {
    //char*               name;
    osMessageQueueId_t  msgQueue;
} ;

static struct hidDevcie hid;

static void setReport(uint32_t handle, const uint8_t* data, int32_t len)
{
    struct msgObj msg;

    if (len > USBD_HID0_IN_REPORT_MAX_SZ) {
    }
    
    msg.pkt = malloc(len);
    if (msg.pkt == NULL) {
        err_print("malloc(%d) failed!\n", len);
        return;
    }

    msg.len = len;
    memcpy(msg.pkt, data, len);

    osMessageQueuePut(hid.msgQueue, &msg, 0, osWaitForever);;
}


void USBD_HID0_Initialize (void)
{
    HAL_GPIO_WritePin(USB_EN_GPIO_Port, USB_EN_Pin, GPIO_PIN_RESET);
}


void USBD_HID0_Uninitialize (void)
{

}

int32_t USBD_HID0_GetReport (uint8_t rtype, uint8_t req, uint8_t rid, uint8_t *buf)
{
    (void)buf;
    switch (rtype) {
    case HID_REPORT_INPUT:
        switch (rid) {
        case 0:
            switch (req) {
                case USBD_HID_REQ_EP_CTRL:        // Explicit USB Host request via Control OUT Endpoint
                case USBD_HID_REQ_PERIOD_UPDATE:  // Periodic USB Host request via Interrupt OUT Endpoint
                break;
            case USBD_HID_REQ_EP_INT:         // Called after USBD_HID_GetReportTrigger to signal
                // data obtained.
                break;
            }
        break;
        }
        break;

    case HID_REPORT_FEATURE:
        break;
    }
    return (0);
}

bool USBD_HID0_SetReport (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len)
{
    (void)req;
    (void)rid;
    (void)len;

    switch (rtype) {
    case HID_REPORT_OUTPUT:
        setReport(0, buf, len);
        break;
    case HID_REPORT_FEATURE:
        break;
    }
    return true;
}

int32_t usb_hid_init(void)
{
    usbStatus ret;

    hid.msgQueue = osMessageQueueNew(10, sizeof(struct msgObj), NULL);
    
    NVIC_ClearPendingIRQ(OTG_FS_WKUP_IRQn);
    NVIC_EnableIRQ(OTG_FS_WKUP_IRQn);
    NVIC_ClearPendingIRQ(OTG_FS_IRQn);
    NVIC_EnableIRQ(OTG_FS_IRQn);

    ret = USBD_Initialize(0U);
    if (ret != usbOK ) {
        err_print("USBD_Initialize(0) failed! ret=0x%x\n", ret);
        return -1;
    }
    USBD_GetState(0U);
    USBD_Disconnect(0U);
    ret = USBD_Connect(0U);
    if (ret != usbOK ) {
        USBD_Uninitialize(0);
        err_print("USBD_Connect(0) failed! ret=0x%x\n", ret);
        return -1;
    }

    return 0;
}

int32_t hid_read(uint8_t* buf, int32_t len)
{
    osStatus_t      osRet;
    uint8_t         msg_prio;
    
    struct msgObj   msg;

    osRet= osMessageQueueGet(hid.msgQueue, &msg, &msg_prio, osWaitForever);
    if (osRet == osOK) {        
        memcpy(buf, msg.pkt, msg.len);
        free(msg.pkt);
        return msg.len;
    }
    
    return 0;
}

int32_t hid_write( const uint8_t* data, int32_t len)
{
    int32_t     i;
    usbStatus   usbRet;
    uint8_t     hidReport[USBD_HID0_IN_REPORT_MAX_SZ];
    int32_t     size;
    int32_t     residue;
    

    for (i = 0; i < len; i += size) {
        residue = len - i;
        size    = residue > USBD_HID0_IN_REPORT_MAX_SZ ? USBD_HID0_IN_REPORT_MAX_SZ : residue;
        memset(hidReport, 0, sizeof(USBD_HID0_IN_REPORT_MAX_SZ));
        memcpy(hidReport, &data[i], size);
        do {
            usbRet = USBD_HID_GetReportTrigger(0U, 0U, hidReport, USBD_HID0_IN_REPORT_MAX_SZ);
            osThreadYield();
        } while (usbRet != usbOK);
    }

    return len;
}
