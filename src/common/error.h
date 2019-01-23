#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdint.h>

#define INVALID_HANDLE     (0)


#define ERR_OK                      0


#define ERR_GROUP_LEGENCY(e)        ((e) <<  0)
#define ERR_GROUP_SYS(e)            ((e) <<  8)
#define ERR_GROUP_TLV(e)            ((e) << 16)
#define ERR_GROUP_DRV(e)            ((e) << 24)


/* The error code list of system */
#define ERR_SYS_NOT_ENOUGH_MEMORY   (ERR_GROUP_SYS(0x01))
#define ERR_SYS_ARGUMENT            (ERR_GROUP_SYS(0x02))
#define ERR_SYS_OS                  (ERR_GROUP_SYS(0x03))
#define ERR_SYS_TIMEOUT             (ERR_GROUP_SYS(0x04))

/* The error code list of drivers */
#define ERR_DRV_HID                 (ERR_GROUP_DRV(0x01))
#define ERR_DRV_FLASH               (ERR_GROUP_DRV(0x02))
#define ERR_DRV_BUZZER              (ERR_GROUP_DRV(0x03))
#define ERR_DRV_FRAM                (ERR_GROUP_DRV(0x04))
#define ERR_DRV_LCD                 (ERR_GROUP_DRV(0x05))
#define ERR_DRV_ICSP_CRC4           (ERR_GROUP_DRV(0x06))
#define ERR_DRV_ICSP_ACK4           (ERR_GROUP_DRV(0x06))
#define ERR_DRV_ICSP_ACK6           (ERR_GROUP_DRV(0x06))

/* The error code list of protocol */
#define ERR_TLV_PKT_SYNTAX          (ERR_GROUP_TLV(0x01))
#define ERR_TLV_PKT_CHECK_SUM       (ERR_GROUP_TLV(0x02))
#define ERR_TLV_FILE_ID             (ERR_GROUP_TLV(0x03))
#define ERR_TLV_FILE_LENGTH         (ERR_GROUP_TLV(0x04))
#define ERR_TLV_FILE_SEGMENT_ID     (ERR_GROUP_TLV(0x05))
#define ERR_TLV_DESC                (ERR_GROUP_TLV(0x06))
#define ERR_TLV_VERIFY              (ERR_GROUP_TLV(0x07))

/* The error code list of legency */
#define ERR_ENTER_PROG_MODE         (ERR_GROUP_LEGENCY(  1)) /*  1 -- 握手不成功       */
#define ERR_GET_CHIPID              (ERR_GROUP_LEGENCY(  2)) /*  2 -- id烧录错误       */
#define ERR_ERASE                   (ERR_GROUP_LEGENCY(  3)) /*  3 -- 擦除不成功       */
#define ERR_BLANK                   (ERR_GROUP_LEGENCY(  4)) /*  4 -- 芯片查空不为空   */
#define ERR_ADC                     (ERR_GROUP_LEGENCY(  5)) /*  5 -- ADC校准不成功    */
#define ERR_16M                     (ERR_GROUP_LEGENCY(  6)) /*  6 -- 16m校准不成功    */
#define ERR_WDT                     (ERR_GROUP_LEGENCY(  7)) /*  7 -- 32k校准不成功    */
#define ERR_PROGRAM                 (ERR_GROUP_LEGENCY(  8)) /*  8 -- 烧录错误         */
#define ERR_READ                    (ERR_GROUP_LEGENCY(  9)) /*  9 -- 读取失败         */
#define ERR_VERIFY                  (ERR_GROUP_LEGENCY( 10)) /* 10 -- 校验错误         */
#define ERR_UPDATE_SEQ              (ERR_GROUP_LEGENCY( 11)) /* 11 -- 时序升级不成功   */
#define ERR_PRG_LIMIT               (ERR_GROUP_LEGENCY( 12)) /* 12 -- 烧录次数限制     */
#define ERR_PC_DN                   (ERR_GROUP_LEGENCY( 13)) /* 13 -- pc发送数据错误   */
#define ERR_VOL                     (ERR_GROUP_LEGENCY( 14)) /* 14 -- 烧录电压错误     */
#define ERR_ROLL                    (ERR_GROUP_LEGENCY( 15)) /* 15 -- 滚动码错误       */
#define ERR_OPTIONS                 (ERR_GROUP_LEGENCY( 16)) /* 16 -- 代码选项出错     */
#define ERR_CHKSUM                  (ERR_GROUP_LEGENCY( 17)) /* 17 -- checksum出错     */
#define ERR_CHID                    (ERR_GROUP_LEGENCY( 18)) /* 18 -- id/hex不一致     */
#define ERR_CRYPTO                  (ERR_GROUP_LEGENCY( 19)) /* 19 -- 加密失败         */
#define ERR_OPTION                  (ERR_GROUP_LEGENCY( 20)) /* 20 -- option失败       */
#define ERR_VDD                     (ERR_GROUP_LEGENCY( 21)) /* 21 -- VDD 电压错误     */
#define ERR_VPP                     (ERR_GROUP_LEGENCY( 22)) /* 22 -- VPP 电压错误     */
#define ERR_FILE                    (ERR_GROUP_LEGENCY( 23)) /* 23 -- 烧录文件错误     */
#define ERR_TEMPERATURE             (ERR_GROUP_LEGENCY( 24)) /* 24 -- 温度标定错误     */
#define ERR_COMMUNICATION           (ERR_GROUP_LEGENCY( 25)) /* 25 -- 7766通信异常     */
#define ERR_DATAPROCESS             (ERR_GROUP_LEGENCY( 26)) /* 26 -- 7766数据异常     */
#define ERR_ADJDIFFERENT            (ERR_GROUP_LEGENCY( 27)) /* 27 -- 7766数据异常     */
#define ERR_SPI_PROTECTED           (ERR_GROUP_LEGENCY( 28)) /* 28 -- 烧录器写保护     */
#define ERR_OPTION_REWRITTEN        (ERR_GROUP_LEGENCY( 29)) /* 29 -- 代码选项被改写   */
#define ERR_PROGRAM_DISTURB         (ERR_GROUP_LEGENCY( 30)) /* 30 --                  */
#define ERR_PROGRAM_DEPTH           (ERR_GROUP_LEGENCY( 31))
#define ERR_CIRCUIT_SHORT           (ERR_GROUP_LEGENCY( 32))
#define ERR_CIRCUIT_OPEN            (ERR_GROUP_LEGENCY( 33))


//const char* errGetMsg(int32_t errCode);
int32_t err_init(void);

void err_handler(int32_t err);
uint32_t err_get_code(void);




#endif /* __ERROR_H__ */

