#ifndef __KEY_H__
#define __KEY_H__

#include <stdint.h>

#define KEY_CODE_PROGRAM                    0x101
#define KEY_CODE_FUNC                       0x102
#define KEY_CODE_FIRMWARE                   0x103
#define KEY_CODE_HEX                        0x104
#define KEY_CODE_CFG                        0x105
#define KEY_CODE_REBOOT                     0x106
#define KEY_CODE_READ_IC                    0x107

#define KEY_CODE_GUI_HEX_UPGRADED           0x200

#define KEY_STATE_PRESS                     0
#define KEY_STATE_RELEASE                   1


#define KEY_PULSE_MINI_THRESHOLD            20

struct key_event {
    uint32_t code;
    uint32_t state;
} ;

int32_t key_init    (void);
int32_t key_waiting (struct key_event* evt, uint32_t timeout);
int32_t key_set     (struct key_event* evt);

#endif /* __KEY_H__ */
