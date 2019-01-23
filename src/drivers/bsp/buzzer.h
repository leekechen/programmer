#ifndef __BUZZER_H__
#define __BUZZER_H__

#include <stdint.h>

int32_t buzzer_init(void);
void 	buzzer_beep(uint32_t beep_time);
void 	buzzer_beep_on(void);
void 	buzzer_beep_off(void);

#endif /* __BUZZER_H__ */
