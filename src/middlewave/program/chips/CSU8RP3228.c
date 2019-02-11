#include "error.h"
#include "power.h"
#include "files.h"
#include "file_hex.h"
#include "io_icsp.h"
#include "delay.h"
#include "program.h"
#include "config.h"
#include "io_icsp.h"
#include "crc4_soft.h"
#include "timer_hw.h"
#include "cswrite_cfg.h"

#define MODULE_DEBUG       1
#define MODULE_ERROR       1

#include "dbg_print.h"

#define CHIP_NUM                    ((sizeof(csu8rp3228_name_list))/(sizeof(char*)))


#define CMD_CFG_PTM                 (0x4C)
#define CMD_CFG_ID_MEM              (0x30)
#define CMD_CFG_OSC_SEL             (0x10)

#if 0
#define CMD_VERIFY                  (0x34)
#define CMD_CFG_PTM_MARGIN_READ1    (0x58)
#define CMD_CFG_PTM_MARGIN_READ2    (0x5C)
#define CMD_PAGE_RESET              (0xF0)
#define CMD_LD_PAGE_MEM             (0x74)
#define CMD_START_BYTE_PRG          (0x70)
#define CMD_BEGIN_PG_PGS            (0xB4)
#define CMD_SFR_WRITE               (0x1C)
#define CMD_SFR_READ                (0x18)
#endif

#define ACK4                                        (0x09)
#define ACK6                                        (0x15)

#define EXT_ROM_BYTE                                (64)

#define ROM_SIZE                                    (0x2000)   /* in Byte */
#define ROM_RESERVED_SIZE                           (18)       /* in Byte */
#define ROM_FREE_SIZE                               (ROM_SIZE - ROM_RESERVED_SIZE)

#define CSU8RP3228_OSC_FREQUENCY_16M                (16000000) /* in Hz */
#define CSU8RP3228_OS_FREQUENCYC_8M                 ( 8000000) /* in Hz */
#define CSU8RP3228_OSC_FREQUENCY_4M                 ( 4000000) /* in Hz */
#define CSU8RP3228_OSC_FREQUENCY_2M                 ( 2000000) /* in Hz */
#define CSU8RP3228_OSC_FREQUENCY_32M                (32000000) /* in Hz */
#define CSU8RP3228_WDT_FREQUENCY                    (   32768) /* in Hz */
#define CSU8RP3228_OSC_DIV_FACTOR                   (    1024)

#define CSU8RP3228_MEASURE_CYCLE_NUM                (6)

#define CSU8RP3228_OSC_DIV_FREQUENCY                (CSU8RP3228_OSC_FREQUENCY_16M / CSU8RP3228_OSC_DIV_FACTOR)
#define CSU8RP3228_OSC_STD_COUNT                    ((TIMER_HW_FREQUENCY / CSU8RP3228_OSC_DIV_FREQUENCY) * CSU8RP3228_MEASURE_CYCLE_NUM)


#define CSU8RP3228_WDT_DIV_FREQUENCY                (32768)
#define CSU8RP3228_WDT_STD_COUNT                    ((TIMER_HW_FREQUENCY / CSU8RP3228_WDT_DIV_FREQUENCY) * CSU8RP3228_MEASURE_CYCLE_NUM)

#define CSU8RP3228_SIZE(rom, option)                ((TLV_DESC_CODE_STUFF_LENGTH + (rom)) + (TLV_DESC_CODE_STUFF_LENGTH + (option)));


static const char* csu8rp3228_name_list[] = {
    /* CSU8RP3228 */
    STR_CSU8RP3424
    STR_CSU8RP3425
    STR_CSU8RP3426
    STR_CSU8RP3427
    STR_CSU8RP3429
    STR_CSU8RP3418
    STR_CSU8RP3234
    STR_CSU8RP3235
    STR_CSU8RP3236
    STR_CSU8RP3225
    STR_CSU8RP3226
} ;

static uint8_t          ext_rom_buf[EXT_ROM_BYTE];
static uint16_t         best_trim_osc;
static uint16_t         best_trim_wdt;
static const uint32_t   std_count;

static int32_t measure_clk(uint32_t cycle)
{
    int32_t i;
    int32_t count;
    int32_t level;

    io_icsp_da_set_in();

    /* filte first 2 cycle */
    for (i = 0; i < 2; i++) {
        count = 0;
        do {
            level = io_icsp_da_read();
            if (count++ > 0xFF00) {
                return -1;
            }
        } while (level == 0);
        count = 0;
        do {
            level = io_icsp_da_read();
            if (count++ > 0xFF00) {
                return -1;
            }
        } while (level == 1);
    }

    timer_hw_start();
    for (i = 0; i < 6; i++) {
        count = 0;
        do {
            level = io_icsp_da_read();
            if (count++ > 0xFF00) {
                return -1;
            }
        } while (level == 0);
        count = 0;
        do {
            level = io_icsp_da_read();
            if (count++ > 0xFF00) {
                return -1;
            }
        } while (level == 1);
    }
    timer_hw_stop();

    return timer_hw_get_count();
}

static inline void cmd_reset(void)
{
    io_icsp_da_set_out();
    io_icsp_send(CMD_CHIP_RESET, ID_CMD);
}

static inline void cmd_verify(void)
{
    io_icsp_da_set_out();

    io_icsp_send(CMD_VERIFY, ID_CMD);
    io_icsp_send_dummy_cycle(16);
}

static inline int32_t cmd_config_ptm(uint8_t b1, uint8_t b2, int32_t err)
{
    uint8_t ack4;
    uint8_t crc4;

    io_icsp_da_set_out();

    crc4 = crc4_sw_calc((b1 << 8) | b2);
    io_icsp_send(CMD_CFG_PTM, ID_CMD);
    io_icsp_send(b1, ID_DATA);
    io_icsp_send(b2, ID_DATA);
    io_icsp_send(crc4 << 4, ID_CRC4);
    io_icsp_da_set_in();
    ack4 = io_icsp_read(ID_ACK4);
    if (ack4 != ACK4) {
        err_print("ack4(0x%x) != ACK4(0x09)\n", ack4);
        return ERR_DRV_ICSP_ACK4 | err;
    }

    return ERR_OK;
}

static inline int32_t cmd_osc_sel(uint8_t ick, int32_t err)
{
    uint8_t crc4;
    uint8_t ack4;

    crc4 = crc4_sw_calc(ick) << 4;

    io_icsp_da_set_out();

    io_icsp_send(CMD_CFG_OSC_SEL, ID_CMD);
    io_icsp_send(0, ID_DATA);
    io_icsp_send(ick, ID_DATA);
    io_icsp_send(crc4, ID_CRC4);
    io_icsp_da_set_in();
    ack4 = io_icsp_read(ID_ACK4);
    if (ack4 != ACK4) {
        err_print("ack4(0x%x) != ACK4(0x09)\n", ack4);
        return ERR_DRV_ICSP_ACK4 | err;
    }

    return ERR_OK;
}

static inline int32_t cmd_osc_cali(uint8_t cmd, uint8_t trim, int32_t err)
{
    uint8_t crc4;
    uint8_t ack4;

    crc4 = crc4_sw_calc((uint16_t)trim) << 4;

    io_icsp_da_set_out();

    io_icsp_send(cmd, ID_CMD);
    io_icsp_send(0, ID_DATA);
    io_icsp_send(trim, ID_DATA);
    io_icsp_send(crc4, ID_CRC4);
    io_icsp_da_set_in();
    ack4 = io_icsp_read(ID_ACK4);
    if (ack4 != ACK4) {
        err_print("ack4(0x%x) != ACK4(0x09)\n", ack4);
        return ERR_DRV_ICSP_ACK4 | err;
    }

    return ERR_OK;
}

static inline void cmd_clock_stop(void)
{
    io_icsp_send_dummy_cycle(6);
    io_icsp_delay_us(100);
}

static inline void cmd_id_mem(void)
{
    io_icsp_da_set_out();

    io_icsp_send(CMD_CFG_ID_MEM, ID_CMD);
}

static inline int32_t cmd_read_mem(uint8_t* buf, int32_t err)
{
    uint8_t crc4;
    uint8_t crc4_recv;

    io_icsp_da_set_out();

    io_icsp_send(CMD_READ_DATA_FROM_PROG_MEM, ID_CMD);
    io_icsp_send_dummy_cycle(1);

    io_icsp_da_set_in();
    buf[0]      = io_icsp_read(ID_DATA);
    buf[1]      = io_icsp_read(ID_DATA);
    crc4_recv   = io_icsp_read(ID_CRC4);
    crc4 = crc4_sw_calc((buf[0] << 8) | buf[1]);
    if (crc4_recv != crc4) {
        err_print("crc4(0x%x) != crc4(0x%x)\n", crc4_recv, crc4);
        return ERR_DRV_ICSP_CRC4 | err;
    }

    return ERR_OK;
}

static inline int32_t cmd_begin_program(int32_t err)
{
    uint8_t ack6;

    io_icsp_da_set_out();

    io_icsp_send(CMD_BEGIN_PROG, ID_CMD);
    io_icsp_da_set_in();
    ack6 = io_icsp_read(ID_ACK6);
    if (ack6 != ACK6) {
        err_print("ack6(0x%x) != ACK6(0x15)\n", ack6);
        return ERR_DRV_ICSP_ACK6 | err;
    }

    return ERR_OK;
}

static inline void cmd_end_program(void)
{
    io_icsp_da_set_out();
    io_icsp_send(CMD_END, ID_CMD);
    io_icsp_delay_us(12);
}

static inline int32_t cmd_program_mem(uint8_t* buf, int32_t err)
{
    uint8_t ack4;
    uint8_t crc4;

    crc4 = crc4_sw_calc(buf[0] << 8 | buf[1]) << 4;

    io_icsp_da_set_out();
    io_icsp_send(CMD_LOAD_DATA_FOR_PROG_MEM, ID_CMD);

    io_icsp_send(buf[0], ID_DATA);
    io_icsp_send(buf[1], ID_DATA);
    io_icsp_send(crc4,   ID_CRC4);

    io_icsp_da_set_in();
    ack4 = io_icsp_read(ID_ACK4);
    if (ack4 != ACK4) {
        err_print("ack4(0x%x) != ACK4(0x09)\n", ack4);
        return ERR_DRV_ICSP_ACK4 | err;
    }

    io_icsp_delay_us(110);

    return ERR_OK;
}

static inline int32_t program_rolling_code(struct tlv_rolling_code* roll, size_t* size)
{
    int32_t err;
    uint8_t buf[2];

    *size = 0;
    switch (roll->width) {
    case 8:
        /* 8 bits rolling code */
        buf[0] = 0x00;
        buf[1] = roll->value & 0xFF;
        err = cmd_program_mem(buf, ERR_PROGRAM);
        if (err) {
            return err;
        }
        return ERR_OK;
    case 16:
        /* 16 bits rolling code */
        buf[0] = (roll->value & 0xFF00) >> 8;
        buf[1] = roll->value & 0xFF;
        err = cmd_program_mem(buf, ERR_PROGRAM);
        if (err) {
            return err;
        }
        return ERR_OK;
    case 32:
        /* 32 bits rolling code */
        buf[0] = (roll->value & 0xFF000000) >> 24;
        buf[1] = (roll->value & 0xFF0000)>> 16;
        err = cmd_program_mem(buf, ERR_PROGRAM);
        if (err) {
            return err;
        }
        buf[0] = (roll->value & 0xFF00) >> 8;
        buf[1] = (roll->value & 0xFF);
        err = cmd_program_mem(buf, ERR_PROGRAM);
        if (err) {
            return err;
        }
        return ERR_OK;
    default:
        return ERR_OK;
    }

    return ERR_OK;
}


static inline int32_t program_without_rolling_code(struct tlv_file* hex)
{
    int32_t i;
    int32_t index = 0;
    int32_t err;
    int32_t addr;
    int32_t offset;
    size_t  size;
    uint8_t buf[256];
    void*            fp;
    struct tlv_code* code;

    dbg_print("[CSU8RP3228] program without rolling code\n");

    /* Got the first code descriptor */
    code = file_hex_get_code(index++);
    if (code == NULL) {
        err_print("file_hex_get_code() failed\n");
        return ERR_FILE;
    }

    for (addr = 0; code != NULL; ) {
        /*
         * If program start address of the code descriptor is not start at 0,
         *  we program 0xFF to skip them.
         */
        dbg_print("[CSU8RP3228] 0x0000~0x%04x program stuff 0xFFFF\n", code->addr);
        fp = file_code_open(TLV_FILE_ID_HEX, code);
        for (; addr < code->addr; addr += 2) {
            buf[0] = 0xFF;
            buf[1] = 0xFF;
            err = cmd_program_mem(buf, ERR_PROGRAM);
            if (err) {
                err_print("cmd_program_mem() failed\n");
                return err;
            }
        }

        dbg_print("[CSU8RP3228] Program code space:0x%04x~0x%04x\n", code->start, code->end);
        for (offset = code->start; offset < code->end; offset += size) {
            memset(buf, 0xFF, sizeof(buf));
            size = file_read(buf, sizeof(buf), fp);
            for (i = 0;  i < size; i += 2, addr += 2) {
                err = cmd_program_mem(&buf[i], ERR_PROGRAM);
                if (err) {
                    err_print("cmd_program_mem() failed\n");
                    return err;
                }
            }
        }
        file_close(fp);

        /* Got the next code desciptor */
        code = file_hex_get_code(index++);
    }
    return ERR_OK;
}

static inline int32_t program_with_rolling_code(struct tlv_file* hex, struct tlv_rolling_code* roll)
{
    int32_t i;
    int32_t index = 0;
    int32_t err;
    int32_t addr;
    int32_t offset;
    size_t  size;
    uint8_t buf[256];
    struct tlv_code* code;
    uint32_t start;
    void* fp;

    dbg_print("[CSU8RP3228] program with rolling code\n");

    /* Got the first code descriptor */
    code = file_hex_get_code(index++);
    if (code == NULL) {
        err_print("file_hex_get_code() failed\n");
        return ERR_FILE;
    }

    for (addr = 0; code != NULL; ) {
        /*
         * If program start address of the code descriptor is not start at 0,
         *  we program 0xFF to skip them.
         */
        if (roll->addr < code->addr) {
            dbg_print("[CSU8RP3228] 0x0000~0x%04x program stuff 0xFFFF\n",
                        roll->addr);
            for (; addr < roll->addr; addr += 2) {
                buf[0] = 0xFF;
                buf[1] = 0xFF;
                err = cmd_program_mem(buf, ERR_PROGRAM);
                if (err) {
                    return err;
                }
            }
            err = program_rolling_code(roll, (size_t*)&size);
            if (err) {
                dbg_print("program_rolling_code() failed\n");
                return err;
            }
            addr += size;
        }

        fp = file_code_open(TLV_FILE_ID_HEX, code);

        dbg_print("[CSU8RP3228] Program code space:0x%04x~0x%04x\n",
                        code->addr, roll->addr);
        for (offset = code->start; offset < code->end; offset += size) {
            memset(buf, 0xFF, sizeof(buf));
            size = file_read(buf, sizeof(buf), fp);
            for (i = 0;  i < size; ) {
                if (addr == roll->addr) {
                    err = program_rolling_code(roll, (size_t*)&size);
                    if (err) {
                        dbg_print("program_rolling_code() failed\n");
                        return err;
                    }
                    addr += size;
                    i += size;
                } else {
                    err = cmd_program_mem(&buf[i], ERR_PROGRAM);
                    if (err) {
                        return err;
                    }
                    i += 2;
                    addr += 2;
                }
            }
        }
        file_close(fp);
        /* Got the next code desciptor */
        code = file_hex_get_code(index++);
    }
    return ERR_OK;
}

void setup_option(uint8_t* checksum, uint8_t* option)
{
    int32_t i;

    memset(ext_rom_buf, 0xFF, sizeof(ext_rom_buf));

    ext_rom_buf[32] = option[1];
    ext_rom_buf[33] = option[0];
    ext_rom_buf[34] = option[2];
    ext_rom_buf[35] = best_trim_osc;
    ext_rom_buf[37] = best_trim_wdt;
    ext_rom_buf[39] = checksum[1];
    ext_rom_buf[41] = checksum[0];
}

int32_t program_option(uint8_t* options, size_t size)
{
    uint32_t i;
    int32_t  err;

    for (i = 0; i < (EXT_ROM_BYTE - 6); i += 2) {
        err = cmd_program_mem(&ext_rom_buf[i], ERR_PROGRAM);
        if (err) {
            return err;
        }
    }
    return ERR_OK;
}

static inline int32_t verify_without_rolling_code(struct tlv_file* hex)
{
    int32_t i;
    int32_t index = 0;
    int32_t err;
    int32_t addr;
    int32_t offset;
    size_t  size;
    uint8_t hex_buf[256];
    uint8_t chip_buf[2];
    struct tlv_code* code;
    void* fp;

    dbg_print("[CSU8RP3228] Verify without rolling code\n");

    /* Got the first code descriptor */
    code = file_hex_get_code(index++);
    if (code == NULL) {
        err_print("file_hex_get_code() failed\n");
        return ERR_FILE;
    }

    for (addr = 0; code != NULL; ) {
        fp = file_code_open(TLV_FILE_ID_HEX, code);
        dbg_print("[CSU8RP3228] skip 0x0000~0x%04x to Verify\n", code->addr);
        for (; addr < code->addr; addr += 2) {
            err = cmd_read_mem(chip_buf, ERR_VERIFY);
            if (err) {
                err_print("cmd_program_mem() failed\n");
                return err;
            }
        }
        dbg_print("[CSU8RP3228] Verify code space:0x%04x~0x%04x\n", code->start, code->end);
        for (offset = code->start; offset < code->end; offset += size) {
            memset(hex_buf, 0xFF, sizeof(hex_buf));
            size = file_read(hex_buf, sizeof(hex_buf), fp);
            for (i = 0;  i < size; i += 2, addr += 2) {
                err = cmd_read_mem(&chip_buf[i], ERR_VERIFY);
                if (err) {
                    err_print("cmd_program_mem() failed\n");
                    return err;
                }
                if (chip_buf[0] != hex_buf[i] || chip_buf[0] != hex_buf[i]) {
                    return ERR_VERIFY;
                }
            }
        }
        file_close(fp);
        /* Got the next code desciptor */
        code = file_hex_get_code(index++);
    }

    return ERR_OK;
}

static inline int32_t verify_with_rolling_code(struct tlv_file* hex, struct tlv_rolling_code* roll)
{
    int32_t i;
    int32_t index = 0;
    int32_t err;
    int32_t addr;
    int32_t offset;
    size_t  size;
    uint8_t buf[256];
    struct tlv_code* code;
    uint32_t start;
    void* fp;

    dbg_print("[CSU8RP3228] program with rolling code\n");

    /* Got the first code descriptor */
    code = file_hex_get_code(index++);
    if (code == NULL) {
        err_print("file_hex_get_code() failed\n");
        return ERR_FILE;
    }

    if (roll->addr >= code->addr) {
        start = code->addr;
    } else {
        start = roll->addr;
    }

    for (addr = 0; code != NULL; ) {
        /*
         * If program start address of the code descriptor is not start at 0,
         *  we program 0xFF to skip them.
         */
        if (roll->addr < code->addr) {
            dbg_print("[CSU8RP3228] 0x0000~0x%04x program stuff 0xFFFF\n", roll->addr);
            for (; addr < roll->addr; addr += 2) {
                buf[0] = 0xFF;
                buf[1] = 0xFF;
                err = cmd_program_mem(buf, ERR_PROGRAM);
                if (err) {
                    return err;
                }
            }
            err = program_rolling_code(roll, (size_t*)&size);
            if (err) {
                dbg_print("program_rolling_code() failed\n");
                return err;
            }
            addr += size;
        }

        fp = file_code_open(TLV_FILE_ID_HEX, code);

        dbg_print("[CSU8RP3228] Program code space:0x%04x~0x%04x\n", code->addr, roll->addr);
        for (offset = code->start; offset < code->end; offset += size) {
            memset(buf, 0xFF, sizeof(buf));
            size = file_read(buf, sizeof(buf), fp);
            for (i = 0;  i < size; ) {
                if (addr == roll->addr) {
                    err = program_rolling_code(roll, (size_t*)&size);
                    if (err) {
                        dbg_print("program_rolling_code() failed\n");
                        return err;
                    }
                    addr += size;
                    i += size;
                } else {
                    err = cmd_program_mem(&buf[i], ERR_PROGRAM);
                    if (err) {
                        return err;
                    }
                    i += 2;
                    addr += 2;
                }
            }
        }
        file_close(fp);
        /* Got the next code desciptor */
        code = file_hex_get_code(index++);
    }
    return ERR_OK;
}


static int32_t csu8rp3228_is_matched(const char* name, uint8_t* id, size_t id_len)
{
    int32_t i;
    int32_t err;

    dbg_print("[Program] CSU8RP3228 Serail\n");

    for (i = 0; i < CHIP_NUM; i++) {
        err = strncmp(name, csu8rp3228_name_list[i], 10);
        if (err == 0) {
            break;
        }
    }

    if (i == CHIP_NUM) {
        return 0;
    }

    io_icsp_init();

    return 1;
}

static int32_t csu8rp3228_hande_shake(struct tlv_file* hex)
{
    int32_t err;

    dbg_print("[CSU8RP3228] Hande Shake ...");

    io_icsp_da_set_out();
    io_icsp_da_low();
    io_icsp_clk_low();

    err = chip_program_power_on(0, 150);
    if (err) {
        err_print("Power on failed\n");
        return err;
    }

    dbg_print("OK\n");

    return ERR_OK;
}

int32_t csu8rp3228_get_id(struct tlv_file* hex)
{
    int32_t             addr;
    int32_t             err;
    struct tlv_chip*    chip;

    dbg_print("[CSU8RP3228] get_id()\n");

    chip = file_hex_get_chip(0);
    if (chip == NULL) {
        err_print("file_hex_get_chip() failed\n");
        return ERR_FILE;
    }

    cmd_verify();

    err = cmd_config_ptm(0, 0, ERR_GET_CHIPID);
    if (err) {
        return err;
    }

    cmd_id_mem();
    for (addr = 0; addr < EXT_ROM_BYTE; addr += 2) {
        err = cmd_read_mem(&ext_rom_buf[addr], ERR_GET_CHIPID);
        if (err) {
            return err;
        }
    }

    if (chip->id[1] == 0xFA && chip->id[3] == 0xFA && chip->id[5] == 0xFA) {
        /* Don't compare the ID */
        dbg_print("[CSU8RP3228] Skip to compare Chip ID, ID:0x%02x,0x%02x,0x%02x\n",
                            chip->id[1], chip->id[3], chip->id[5]);
        return ERR_OK;
    }

    if (ext_rom_buf[59] == chip->id[1]
     && ext_rom_buf[61] == chip->id[3]
     && ext_rom_buf[63] == chip->id[5]) {
        dbg_print("[CSU8RP3228] Chip ID:0x%02x,0x%02x,0x%02x\n",
                                    chip->id[1], chip->id[3], chip->id[5]);
        return ERR_OK;
    }

    err_print("Chip ID mismatch:%02x,%02x,%02x(%02x,%02x,%02x)",
            ext_rom_buf[59], ext_rom_buf[61], ext_rom_buf[63],
                chip->id[1], chip->id[3], chip->id[5]);

    return ERR_GET_CHIPID;
}

int32_t csu8rp3228_osc_cali(struct tlv_file* hex)
{
    int32_t err;
    uint8_t ick_sel;
    uint32_t trim;
    int32_t count;
    int32_t err_best;
    int32_t err_limit;
    int32_t err_value;
    struct tlv_option* option;
    struct tlv_osc* osc;

    dbg_print("[CSU8RP3228] osc_cali()\n");

    osc = file_hex_get_osc(TLV_DESC_OSC_TYPE_OSC);
    if (osc == NULL) {
        err_print("file_hex_get_osc(osc) failed\n");
        return ERR_FILE;
    }

    dbg_print("[CSU8RP3228] osc_cali(), osc: %d Hz, error=%d%%\n", osc->value, osc->offset);
    option = file_hex_get_option(0);
    if (option == NULL) {
        err_print("file_hex_get_option() failed\n");
        return ERR_FILE;
    }

    err_limit      = CSU8RP3228_OSC_STD_COUNT / 100 * osc->offset;
    best_trim_osc  = 0xFFFF;
    err_best       = 0xFFFF;

    cmd_verify();
    err = cmd_config_ptm(0, 0, ERR_16M);
    if (err) {
        return err;
    }
    cmd_reset();

    cmd_id_mem();

    if (ext_rom_buf[35] == 0xFF) {
        /*Had not been trimed*/
        ick_sel = (option->bytes[1] & 0xE0) >> 5;

        err = cmd_osc_sel(ick_sel, ERR_16M);
        if (err) {
            return err;
        }

        for (trim = 0; trim < 256; trim++) {
            err = cmd_osc_cali(CMD_CAL_OSC_16M, (uint8_t)trim, ERR_16M);
            if (err) {
                return err;
            }

            count = measure_clk(CSU8RP3228_MEASURE_CYCLE_NUM);
            err_value = count - CSU8RP3228_OSC_STD_COUNT;
            err_value = err_value > 0 ? err_value : -err_value;
            if (err_best > err_value) {
                err_best = err_value;
                best_trim_osc = trim;
            }

            cmd_clock_stop();
        }
    }

    if (err_best > err_limit) {
        err_print("OSC trim failed, best=%d(limit=%d)\n", err_best, err_limit);
        return ERR_16M;
    }

    /* Verify the best trim value again */
    err = cmd_osc_cali(CMD_CAL_OSC_16M, (uint8_t)best_trim_osc, ERR_16M);
    if (err) {
        return err;
    }
    count = measure_clk(CSU8RP3228_MEASURE_CYCLE_NUM);
    err_value = count - CSU8RP3228_OSC_STD_COUNT;
    err_value = err_value > 0 ? err_value : -err_value;
    if (err_value > err_limit) {
        err_print("Verify OSC trim(0x%x)failed, best=%d(limit=%d)\n", best_trim_osc, err_value, err_limit);
        return ERR_16M;
    }

    cmd_clock_stop();

    dbg_print("[CSU8RP3228] OSC Best Trim=0x%02x, Best Error=%d\n", best_trim_osc, err_value);

    return ERR_OK;
}

int32_t csu8rp3228_wdt_cali(struct tlv_file* hex)
{
    int32_t err;
    uint32_t trim;
    int32_t count;
    int32_t err_best;
    int32_t err_limit;
    int32_t err_value;
    struct tlv_osc* osc;

    dbg_print("[CSU8RP3228] wdt_cali()\n");

    osc = file_hex_get_osc(TLV_DESC_OSC_TYPE_WDT);
    if (osc == NULL) {
        err_print("file_hex_get_osc(wdt) failed\n");
        return ERR_FILE;
    }

    best_trim_wdt  = 0xFFFF;
    err_best       = 0xFFFF;
    err_limit      = CSU8RP3228_WDT_STD_COUNT / 100 * osc->offset;

    dbg_print("[CSU8RP3228] wdt_cali(), wdt: %d Hz, error=%d%%\n", osc->value, osc->offset);

    cmd_verify();

    err = cmd_config_ptm(0, 0, ERR_WDT);
    if (err) {
        return err;
    }

    cmd_reset();
    cmd_id_mem();

    if (ext_rom_buf[37] == 0xFF) {
        /*Had not been trimed*/
        for (trim = 0; trim < 256; trim++) {
            err = cmd_osc_cali(CMD_CAL_WDT, (uint8_t)trim, ERR_WDT);
            if (err) {
                return err;
            }

            count = measure_clk(CSU8RP3228_MEASURE_CYCLE_NUM);
            err_value = count - CSU8RP3228_WDT_STD_COUNT;
            err_value = err_value > 0 ? err_value : -err_value;
            if (err_best > err_value) {
                err_best = err_value;
                best_trim_wdt = trim;
            }

            cmd_clock_stop();
        }
    }

    if (err_best > err_limit) {
        err_print("OSC trim failed, best=%d(limit=%d)\n", err_best, err_limit);
        return ERR_WDT;
    }

    /* Verify the best trim value again */
    err = cmd_osc_cali(CMD_CAL_WDT, (uint8_t)best_trim_wdt, ERR_WDT);
    if (err) {
        return err;
    }

    count = measure_clk(CSU8RP3228_MEASURE_CYCLE_NUM);
    err_value = count - CSU8RP3228_WDT_STD_COUNT;
    err_value = err_value > 0 ? err_value : -err_value;
    if (err_value > err_limit) {
        err_print("Verify OSC trim(0x%x)failed, best=%d(limit=%d)\n", best_trim_osc, err_value, err_limit);
        return ERR_WDT;
    }

    cmd_clock_stop();

    dbg_print("[CSU8RP3228] WDT Best Trim=0x%02x, Best Error=%d\n", best_trim_wdt, err_value);

    return ERR_OK;
}

int32_t csu8rp3228_adc_cali(struct tlv_file* hex)
{
    return ERR_OK;
}

int32_t csu8rp3228_blank_check(struct tlv_file* hex)
{
    int32_t addr;
    int32_t err;
    uint8_t buf[2];

    dbg_print("[CSU8RP3228] blank_check()\n");

    cmd_verify();

    err = cmd_config_ptm(0, 0, ERR_BLANK);
    if (err) {
        return err;
    }

    cmd_reset();
    for (addr = 0; addr < ROM_FREE_SIZE; addr += 2) {
        err = cmd_read_mem(buf, ERR_BLANK);
        if (err) {
            return err;
        }

        if (buf[0] != 0xFF || buf[1] != 0xFF) {
            err_print("Failed, Not blank addr=0x%04x(0x%04x)\n", addr, buf[0] << 8 | buf[1]);
            return ERR_BLANK;
        }
    }

    dbg_print("[CSU8RP3228] 0x0000~0x%04x is blank\n", ROM_FREE_SIZE);

    return ERR_OK;
}

int32_t csu8rp3228_erase(struct tlv_file* hex)
{
    return ERR_OK;
}

int32_t csu8rp3228_program(struct tlv_file* hex)
{
    int32_t index = 0;
    int32_t err;
    uint8_t buf[2];
    struct tlv_rolling_code* rolling_code;

    cmd_verify();
    err = cmd_config_ptm(0, 0, ERR_PROGRAM);
    if (err) {
        return err;
    }

    cmd_reset();
    err = cmd_read_mem(buf, ERR_PROGRAM);
    if (err) {
        return err;
    }
    cmd_reset();

    err = cmd_begin_program(ERR_PROGRAM);
    if (err) {
        return err;
    }

    rolling_code = cswrite_cfg_get_rolling_code(1);
    if (rolling_code == NULL) {
        err = program_without_rolling_code(hex);
        if (err) {
            return err;
        }
    } else {
        err = program_with_rolling_code(hex, rolling_code);
        if (err) {
            return err;
        }
    }
    cmd_end_program();

    cmd_reset();
    err = cmd_begin_program(ERR_PROGRAM);
    if (err) {
        return err;
    }

    cmd_id_mem();

    program_option(ext_rom_buf, EXT_ROM_BYTE - 6);

    cmd_end_program();

    return ERR_OK;
}

int32_t csu8rp3228_protect(struct tlv_file* hex)
{
    return ERR_OK;
}

int32_t csu8rp3228_verify(struct tlv_file* hex)
{
    struct tlv_rolling_code* rolling_code;

    uint8_t buf[256];
    int32_t len;
    int32_t err;
    int32_t offset;

    dbg_print("[CSU8RP3228] Verify\n");

    rolling_code = cswrite_cfg_get_rolling_code(0);
    if (rolling_code == NULL) {
        err = verify_without_rolling_code(hex);
        if (err) {
            return err;
        }
    } else {
        err = verify_with_rolling_code(hex, rolling_code);
        if (err) {
            return err;
        }
    }

    return ERR_OK;
}

int32_t csu8rp3228_readback(struct tlv_file* hex)
{
    int32_t          err;
    uint32_t         addr;
    uint8_t          buf[2];
    struct tlv_code* code;

    code = file_hex_get_code(0);
    if (code == NULL) {
        return ERR_FILE;
    }

    cmd_verify();

    err = cmd_config_ptm(0, 0, ERR_READ);
    if (err) {
        return err;
    }

    cmd_reset();

    for (addr = code->start; addr < code->end; addr += 2) {
        err = cmd_read_mem(buf, ERR_READ);
        if (err) {
            return err;
        }
        dbg_print("0x%02x%02x\n", buf[0], buf[1]);
        file_ic_code_write(addr, buf, 2);
    }

    return ERR_OK;
}


int32_t csu8rp3228_get_size(uint32_t* size)
{
    *size = CSU8RP3228_SIZE(ROM_SIZE, EXT_ROM_BYTE);
    return ERR_OK;
}

const struct chip_operation csurp3228_ops = {
    .is_matched  = csu8rp3228_is_matched,
    .hande_shake = csu8rp3228_hande_shake,
    .get_id      = csu8rp3228_get_id,
    .osc_cali    = csu8rp3228_osc_cali,
    .wdt_cali    = csu8rp3228_wdt_cali,
    .adc_cali    = csu8rp3228_adc_cali,
    .blank_check = csu8rp3228_blank_check,
    .erase       = csu8rp3228_erase,
    .program     = csu8rp3228_program,
    .protect     = csu8rp3228_protect,
    .verify      = csu8rp3228_verify,
    .readback    = csu8rp3228_readback,
    .get_size    = csu8rp3228_get_size,
} ;

