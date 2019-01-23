#ifndef __CSWRITE_CFG_H__
#define __CSWRITE_CFG_H__

#include <stdint.h>
#include "tlv.h"

#define CSWRITE_LANGUAGE_ID_CHN     0
#define CSWRITE_LANGUAGE_ID_ENG     1

#define CSWRITE_BUZZER_MUTE         0
#define CSWRITE_BUZZER_UNMUTE       1

#define CSWRITE_FIRMWARE_BAK_A      0
#define CSWRITE_FIRMWARE_BAK_B      1

#define CSWRITE_FIRMWARE_NEW        0
#define CSWRITE_FIRMWARE_UPGRADE    0

int32_t cswrite_cfg_load(void);

int32_t cswrite_cfg_reset(void);

int32_t cswrite_cfg_set_chip(const char* name, uint8_t* id, size_t id_len);

int32_t cswrite_get_version(struct tlv_version* ver);

uint32_t cswrite_cfg_get_chip_size(void);

struct chip_operation* cswrite_cfg_get_chip_ops(void);
/**
 * Get the serial ID of CSWrite3.0
 * @pram type
 *      CSWRITE_ID_LEGACY          1
 *      CSWRITE_ID_32_BITS         2
 * @param id  output buffer of CSWrite serial ID
 * @return successed return ERR_OK, others failed
 */
int32_t cswrite_cfg_get_serial_id(int32_t type, uint8_t* id);

/**
 * Get the language ID of CSWrite3.0
 * @param id  output buffer of CSWrite serial ID
 *      CSWRITE_LANGUAGE_ID_CHN          0
 *      CSWRITE_LANGUAGE_ID_ENG          1
 * @return successed return ERR_OK, others failed
 */
int32_t cswrite_cfg_get_language(uint8_t* lang_id);

/**
 * Set the language ID of CSWrite3.0
 * @pram id
 *      CSWRITE_LANGUAGE_ID_CHN          1
 *      CSWRITE_LANGUAGE_ID_ENG          2
 * @return successed return ERR_OK, others failed
 */
int32_t cswrite_cfg_set_language(uint8_t lang_id);

/**
 * Set the buzzer state of CSWrite3.0
 * @pram state output buffer
 *      CSWRITE_BUZZER_MUTE              0
 *      CSWRITE_BUZZER_UNMUTE            1
 * @return successed return ERR_OK, others failed
 */
int32_t cswrite_cfg_get_buzzer_state(uint8_t* state);

/**
 * Set the buzzer state of CSWrite3.0
 * @pram state 
 *      CSWRITE_BUZZER_MUTE            1
 *      CSWRITE_BUZZER_UNMUTE          2
 * @return successed return ERR_OK, others failed
 */
int32_t cswrite_cfg_set_buzzer_state(uint8_t state);

/**
 * Get the CSWrite activated time of CSWrite3.0
 * @pram tm output buffer of actived time
 * @return successed return ERR_OK, others failed
 */
int32_t cswrite_cfg_get_active_day(struct tlv_date* date);

/**
 * Set the CSWrite activated time of CSWrite3.0
 * @pram tm actived time
 * @return successed return ERR_OK, others failed
 */
int32_t cswrite_cfg_set_active_date(struct tlv_date* date);

int32_t cswrite_cfg_get_version(struct list_head* list);
int32_t cswrite_cfg_get_chip(struct list_head* list);

int32_t cswrite_get_firmware_flag(uint32_t* flag);

int32_t cswrite_set_firmware_flag(uint32_t flag);

int32_t cswrite_set_hex_flag(uint32_t flag);


int32_t cswrite_cfg_set_rolling_code(struct tlv_rolling_code* roll);
struct tlv_rolling_code* cswrite_cfg_get_rolling_code(int32_t flag);

int32_t cswrite_cfg_get_id(struct list_head* list);


#endif /* __CSWRITE_CFG_H__ */

