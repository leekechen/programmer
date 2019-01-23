#include "error.h"
#include "power.h"
#include "file_hex.h"
#include "io_spi.h"
#include "delay.h"

static int32_t power_is_valid(void)
{
    int32_t err;
    uint32_t mv;
    struct tlv_voltage vol;
    
    err = file_hex_get_voltage(POWER_TYPE_VDD, &vol);
    if (err) {
        return ERR_VDD;
    }

    err = power_voltage_get(POWER_TYPE_VDD, &mv);
    if (err) {
        return ERR_VDD;
    }

    err = power_voltage_is_valid(&vol, mv);
    if (!err) {
        return ERR_VDD;
    }

    err = file_hex_get_voltage(POWER_TYPE_VPP, &vol);
    if (err) {
        return ERR_VPP;
    }

    err = power_voltage_get(POWER_TYPE_VPP, &mv);
    if (err) {
        return ERR_VPP;
    }

    err = power_voltage_is_valid(&vol, mv);
    if (!err) {
        return ERR_VPP;
    }

    return ERR_OK;
}

static int32_t power_enter_program(uint32_t vpp_hh)
{
    int32_t         err;

    err = power_voltage_switch(POWER_TYPE_VDD, POWER_FLAG_ON);
    if (err) {
        return ERR_VPP;
    }

    err = power_voltage_switch(POWER_TYPE_VPP, POWER_FLAG_ON);
    if (err) {
        return ERR_VPP;
    }
    
    delay_us(25000);
    
    return ERR_OK;
}


int32_t hande_shake(void)
{
    int32_t err;
    
    err = file_hex_is_valid();
    if (err) {
    }

    err = power_is_valid();
    if (err) {
    }
    
    power_enter_program(4600);
    
    return ERR_OK;
}

int32_t get_id(void)
{
    uint8_t id[5];
    size_t  size;

    size = sizeof(id);
    file_hex_get_chip_id(id, &size);
    
    return ERR_OK;
}

int32_t osc_cali(void)
{
//    file_hex_get_osc(OSC_TYPE);
    return ERR_OK;
}

int32_t wdt_cali(void)
{
//    file_hex_get_osc(WDT_TYPE);
    return ERR_OK;
}

int32_t adc_cali(void)
{
    return ERR_OK;
}

int32_t blank_check(void)
{
    return ERR_OK;
}

int32_t erase(void)
{
    return ERR_OK;
}

int32_t program(void)
{
//    uint16_t word;
//    uint16_t addr;
    
//    file_hex_get_code_num(uint32_t * num);

//    for (addr = ; addr <; addr++) {
//        file_hex_code_read(, addr, &word, sizeof(word));
//        io_spi_write_word();
//    }
    
    return ERR_OK;
}

int32_t protect(void)
{
    return ERR_OK;
}

int32_t verify(void)
{
    return ERR_OK;
}

int32_t readback(void)
{
//    for (addr = ;  addr < ; addr++) {
//        //io_spi_read_word(&word);
//        //file_ic_write(addr, &word, 2);
//    }

    return ERR_OK;
}



