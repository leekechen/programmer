#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>

#define LED_TYPE_OK              1
#define LED_TYPE_FAIL            2
#define LED_TYPE_BUSY            3
#define LED_TYPE_INVALID         -1

#define LED_FLAG_ON              1
#define LED_FLAG_OFF             0
#define LED_FLAG_INVALID             0

void led_set(int32_t type, uint32_t flag);

#endif /* __LED_H__ */
