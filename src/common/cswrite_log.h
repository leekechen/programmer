#ifndef __CSWRITE_LOG_H__
#define __CSWRITE_LOG_H__

int32_t cswrite_log_get_num_ok(void);
int32_t cswrite_log_get_num_failed(void);
int32_t cswrite_log_set_chip(const char* name, uint8_t* id, uint8_t id_length);
int32_t cswrite_log_set_hex_time(struct tlv_date* day, struct tlv_time* time);

#endif /* __CSWRITE_LOG_H__ */
