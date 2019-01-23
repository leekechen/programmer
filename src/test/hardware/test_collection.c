#include <stdlib.h>
#include <string.h>
#include "lcd.h"
#include "key.h"
#include "w25qxx.h"
#include "led.h"
#include "stm32f4xx_ll_gpio.h"
#include "io_icsp.h"
#include "buzzer.h"
#include "mb85rcxx.h"
#include "power.h"
#include "delay.h"
#include "error.h"
#include "vs.h"

#define MODULE_DEBUG       1 
#define MODULE_ERROR       1

#include "dbg_print.h"

#define TEMP_SCL                         LL_GPIO_PIN_10
#define TEMP_SDA                         LL_GPIO_PIN_11
#define TEMP_PORT                        GPIOB

#define EXT_D0                           LL_GPIO_PIN_0
#define EXT_D1                           LL_GPIO_PIN_1
#define EXT_D2                           LL_GPIO_PIN_2
#define EXT_D3                           LL_GPIO_PIN_3 
#define EXT_D4                           LL_GPIO_PIN_8
#define EXT_D5                           LL_GPIO_PIN_9  
#define EXT_D6                           LL_GPIO_PIN_8 
#define EXT_D7                           LL_GPIO_PIN_9 

#define EXT_D012367_PORT                 GPIOD
#define EXT_D4_PORT                      GPIOA
#define EXT_D5_PORT                      GPIOC


#define FRAM_SIZE                       (1024 * 2)


int32_t wait_key_press(uint32_t timeout)
{
    int32_t ret;
    struct key_event evt;

    while (1) {
        ret = key_waiting(&evt, timeout);
        if (evt.code == KEY_CODE_PROGRAM && evt.state == KEY_STATE_PRESS) {
            dbg_print("\t Passed\n");
            buzzer_beep(100);
            return 1;
        } else if (evt.code == KEY_CODE_FUNC && evt.state == KEY_STATE_PRESS) {
            dbg_print("\t Failed\n");
            buzzer_beep(100);
            return 1;
        }
        if (ret == ERR_SYS_TIMEOUT) {
            return 0;
        }
    }

}

static int32_t vs_ad_vs_filter(void)
{
    int32_t  i;
    uint32_t vol = 0;
    uint32_t adc;
    
    for (i = 0; i < 16; i++) {
        vs_cali_read(VS_CALI_MODE_OUTPUT_2500_MV, &adc);
        vol += adc;
    }
    return vol / 16;
}

static int32_t vs_ad_vs_b_filter(void)
{
    int32_t  i;
    uint32_t vol = 0;
    uint32_t adc;
    
    for (i = 0; i < 16; i++) {
        vs_cali_read(VS_CALI_MODE_INPUT, &adc);
        vol += adc;
    }
    return vol / 16;
}


void test_lcd(void)
{
    
    dbg_print("[Test] Turn on back light of LCD, and LCD show solid red ...\n");    
    lcd_color_show(LCD_COLOR_RED);
    wait_key_press(osWaitForever);

    dbg_print("[Test] Turn off back light of LCD ...\n");
    lcd_back_light_set(0, LCD_BACK_LIGHT_TOGGLE_INFINTE);
    wait_key_press(osWaitForever);

    dbg_print("[Test] Turn on back light of LCD, and LCD show solid green ...\n");
    lcd_back_light_set(1, LCD_BACK_LIGHT_TOGGLE_INFINTE);    
    lcd_color_show(LCD_COLOR_GREEN);
    wait_key_press(osWaitForever);
    

    dbg_print("[Test] LCD show solid Blue ...\n");    
    lcd_color_show(LCD_COLOR_BLUE);
    wait_key_press(osWaitForever);

}


void test_flash(void)
{
    int32_t i;
    int32_t err;
    int32_t dev;
    uint32_t data[64];
    uint32_t addr;
    size_t   size = sizeof(data);

    dbg_print("[Test] Test Flash Init ...\n");
    
    w25qxx_init();
    dev = w25qxx_open(DEV_SPI1, FLASH_8M_SIZE);
    if (dev < 0) {
        dbg_print("\t w25qxx_open() failed\n");
        return;
    }
    
    err = w25qxx_power_control(dev, ARM_POWER_FULL, 42000000);
    if (err) {
        dbg_print("\t w25qxx_power_control() failed\n");
        return;
    }

    dbg_print("[Test] Flash Read ... ");
    for (addr = 0; addr < FLASH_8M_SIZE; addr += size) {
        memset(data, 0, size);
        if ((addr % 0x400) == 0) {
            dbg_print(".");
        }
        err = w25qxx_read(dev, addr, (uint8_t*)data, size);
        if (err != size) {
            dbg_print("\t w25qxx_read() failed\n"); 
            return;
        }
    }
    dbg_print("\t Passed\n");   

    dbg_print("[Test] Test Flash Erase ...\n");

    err = w25qxx_erase(dev, 0, FLASH_8M_SIZE);
    if (err) {
        dbg_print("\t w25qxx_erase() failed\n");
        return;
    }
    dbg_print("\t Passed\n"); 
    
    dbg_print("[Test] Verif erase ...");
    for (addr = 0; addr < FLASH_8M_SIZE; addr += size) {
        memset(data, 0, size);
        if ((addr % 0x400) == 0) {
            dbg_print(".");
        }
        err = w25qxx_read(dev, addr, (uint8_t*)data, size);
        if (err != size) {
            dbg_print("\t w25qxx_read() failed\n"); 
            return;
        }

        for (i = 0; i < 64; i++) {
            if (data[i] != 0xFFFFFFFF) {
                dbg_print("\t Failed, 0x%x=%x\n", addr + (i*4), data[i]);
                return;
            }
        }
    }
    dbg_print("\t Passed\n");

    for (i = 0; i < 64; i++) {
        data[i] = 0xAA55AA55;
    }


    dbg_print("[Test] Flash Program ...");

    for (addr = 0; addr < FLASH_8M_SIZE; addr += size) {
        err= w25qxx_program(dev, addr, (uint8_t*)data, size);
        if (err != size) {
            dbg_print("\t w25qxx_program() failed\n");
            return;
        }
        
        if ((addr % 0x400) == 0) {
            dbg_print(".");
        }
    }
    dbg_print("\t Passed\n");

    dbg_print("[Test] Verif program ...");

    for (addr = 0; addr < FLASH_8M_SIZE; addr += size) {
        memset(data, 0, size);
        
        if ((addr % 0x400) == 0) {
            dbg_print(".");
        }
        err = w25qxx_read(dev, addr, (uint8_t*)data, size);
        if (err != size) {
            dbg_print("\t w25qxx_read() failed\n"); 
            return;
        }

        for (i = 0; i < 64; i++) {
            if (data[i] != 0xAA55AA55) {
                dbg_print("\t Failed, 0x%x=%x\n", addr + (i*4), data[i]);
                return;
            }
        }
    }
    dbg_print("\t Passed\n");
}


void test_fram(void)
{ 
    int32_t i;
    int32_t err;
    uint32_t data[64];
    uint32_t addr;
    size_t   size = sizeof(data);   
    
    dbg_print("[Test] Test fram write 0xAA55AA55 ...\n");

    for (i = 0; i < 64; i++) {
        data[i] = 0xAA55AA55;
    }

    for (addr = 0; addr < FRAM_SIZE; addr += size) {        
        err = fram_write(addr, (uint8_t*)data, size);
        if (err) {
            dbg_print("\t fram_write() failed\n"); 
            return;
        }
        dbg_print(".");
    }
    dbg_print("\t Passed\n");

    dbg_print("[Test] Verify write ...\n");
    for (addr = 0; addr < FRAM_SIZE; addr += size) { 
        memset(data, 0, size);
        err = fram_read(addr, (uint8_t*)data, size);
        if (err) {
            dbg_print("\t fram_write() failed\n"); 
            return; 
        }
        dbg_print(".");
        for (i = 0; i < 64; i++) {
            if (data[i] != 0xAA55AA55) {
                dbg_print("\t verif erase failed, 0x%x=%x\n", addr + (i*4), data[i]); 
                return;
            }
        }
    }
    dbg_print("\t Passed\n");

    dbg_print("[Test] Test fram write 0x55AA55AA ...\n");

    for (i = 0; i < 64; i++) {
        data[i] = 0x55AA55AA;
    }

    for (addr = 0; addr < FRAM_SIZE; addr += size) {        
        err = fram_write(addr, (uint8_t*)data, size);
        if (err) {
            dbg_print("\t fram_write() failed\n"); 
            return;
        }
        dbg_print(".");
    }
    dbg_print("\t Passed\n");
    
    dbg_print("[Test] Verify write ...\n");
    
    for (addr = 0; addr < FRAM_SIZE; addr += size) { 
        memset(data, 0, size);
        err = fram_read(addr, (uint8_t*)data, size);
        if (err) {
            dbg_print("\t fram_write() failed\n"); 
            return; 
        }
        dbg_print(".");
        for (i = 0; i < 64; i++) {
            if (data[i] != 0x55AA55AA) {
                dbg_print("\t verif erase failed, 0x%x=%x\n", addr + (i*4), data[i]);
                return;
            }
        }
    }
    dbg_print("\t Passed\n");
}

void test_led(void)
{
    int ret;
    struct key_event evt;
    
    dbg_print("[Test] Test LED, trun on Busy,NG,OK ...\n");

    led_set(LED_TYPE_OK, LED_FLAG_ON);
    led_set(LED_TYPE_FAIL, LED_FLAG_ON);
    led_set(LED_TYPE_BUSY, LED_FLAG_ON);    
    
    wait_key_press(osWaitForever);

    dbg_print("[Test] Test LED, trun off Busy,NG,OK ...\n");

    led_set(LED_TYPE_OK, LED_FLAG_OFF);
    led_set(LED_TYPE_FAIL, LED_FLAG_OFF);
    led_set(LED_TYPE_BUSY, LED_FLAG_OFF);    

    wait_key_press(osWaitForever);  

    while (1) {
        dbg_print("[Test] trun on Busy,NG,OK 500 ms...\n");
        led_set(LED_TYPE_OK, LED_FLAG_ON);
        led_set(LED_TYPE_FAIL, LED_FLAG_ON);
        led_set(LED_TYPE_BUSY, LED_FLAG_ON); 
        osDelay(500);
        
        dbg_print("[Test] trun off Busy,NG,OK 50 ms...\n"); 
        led_set(LED_TYPE_OK, LED_FLAG_OFF);
        led_set(LED_TYPE_FAIL, LED_FLAG_OFF);
        led_set(LED_TYPE_BUSY, LED_FLAG_OFF);
        osDelay(50);
        
        ret = wait_key_press(1);
        if (ret) {
            break;
        }
    }    

    while (1) {
        dbg_print("[Test] trun on Busy,NG,OK 1 ms...\n");
        led_set(LED_TYPE_OK, LED_FLAG_ON);
        led_set(LED_TYPE_FAIL, LED_FLAG_ON);
        led_set(LED_TYPE_BUSY, LED_FLAG_ON); 
        osDelay(1);
        
        dbg_print("[Test] trun off Busy,NG,OK 50 ms...\n"); 
        led_set(LED_TYPE_OK, LED_FLAG_OFF);
        led_set(LED_TYPE_FAIL, LED_FLAG_OFF);
        led_set(LED_TYPE_BUSY, LED_FLAG_OFF);
        osDelay(50);

        ret = wait_key_press(1);
        if (ret) {
            break;
        }
    }    
    
}

void test_temp(void)
{ 
    int32_t ret;
    struct key_event evt;
    
    dbg_print("[Test] Test temperature interface, SCL SDA High level ...\n");
    
    LL_GPIO_SetPinMode(TEMP_PORT, TEMP_SCL, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(TEMP_PORT, TEMP_SDA, LL_GPIO_MODE_OUTPUT);
    
    LL_GPIO_SetPinOutputType(TEMP_PORT, TEMP_SCL | TEMP_SDA, LL_GPIO_OUTPUT_PUSHPULL);
    
    LL_GPIO_SetPinSpeed(TEMP_PORT, TEMP_SCL,  LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinSpeed(TEMP_PORT, TEMP_SDA,  LL_GPIO_SPEED_FREQ_LOW);


    LL_GPIO_SetPinPull(TEMP_PORT, TEMP_SCL, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinPull(TEMP_PORT, TEMP_SDA, LL_GPIO_PULL_UP); 

    LL_GPIO_SetOutputPin(TEMP_PORT, TEMP_SCL);
    LL_GPIO_SetOutputPin(TEMP_PORT, TEMP_SDA);
    
    wait_key_press(osWaitForever);

    
    dbg_print("[Test] Test temperature interface, SCL SDA Low level ...\n");

    LL_GPIO_ResetOutputPin(TEMP_PORT, TEMP_SCL);
    LL_GPIO_ResetOutputPin(TEMP_PORT, TEMP_SDA);
    

    wait_key_press(osWaitForever);
}

void test_ext(void)
{
    struct key_event evt;
    
    dbg_print("[Test] Test extent interface, D0~D7 High level ...\n");
    
    LL_GPIO_SetPinMode(EXT_D012367_PORT,    EXT_D0, LL_GPIO_MODE_OUTPUT); 
    LL_GPIO_SetPinMode(EXT_D012367_PORT,    EXT_D1, LL_GPIO_MODE_OUTPUT); 
    LL_GPIO_SetPinMode(EXT_D012367_PORT,    EXT_D2, LL_GPIO_MODE_OUTPUT); 
    LL_GPIO_SetPinMode(EXT_D012367_PORT,    EXT_D3, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(EXT_D4_PORT,         EXT_D4, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(EXT_D5_PORT,         EXT_D5, LL_GPIO_MODE_OUTPUT); 
    LL_GPIO_SetPinMode(EXT_D012367_PORT,    EXT_D6, LL_GPIO_MODE_OUTPUT); 
    LL_GPIO_SetPinMode(EXT_D012367_PORT,    EXT_D7, LL_GPIO_MODE_OUTPUT); 

    LL_GPIO_SetPinOutputType(EXT_D4_PORT,         EXT_D4, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinOutputType(EXT_D5_PORT,         EXT_D5, LL_GPIO_OUTPUT_PUSHPULL); 
    LL_GPIO_SetPinOutputType(EXT_D012367_PORT,    EXT_D0 | EXT_D1 | EXT_D2 | EXT_D3 | EXT_D6 | EXT_D7, LL_GPIO_OUTPUT_PUSHPULL);
    
    LL_GPIO_SetPinSpeed(EXT_D012367_PORT,    EXT_D0, LL_GPIO_SPEED_FREQ_LOW); 
    LL_GPIO_SetPinSpeed(EXT_D012367_PORT,    EXT_D1, LL_GPIO_SPEED_FREQ_LOW); 
    LL_GPIO_SetPinSpeed(EXT_D012367_PORT,    EXT_D2, LL_GPIO_SPEED_FREQ_LOW); 
    LL_GPIO_SetPinSpeed(EXT_D012367_PORT,    EXT_D3, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinSpeed(EXT_D4_PORT,         EXT_D4, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinSpeed(EXT_D5_PORT,         EXT_D5, LL_GPIO_SPEED_FREQ_LOW); 
    LL_GPIO_SetPinSpeed(EXT_D012367_PORT,    EXT_D6, LL_GPIO_SPEED_FREQ_LOW); 
    LL_GPIO_SetPinSpeed(EXT_D012367_PORT,    EXT_D7, LL_GPIO_SPEED_FREQ_LOW); 

    LL_GPIO_SetPinPull(EXT_D012367_PORT,    EXT_D0, LL_GPIO_PULL_UP); 
    LL_GPIO_SetPinPull(EXT_D012367_PORT,    EXT_D1, LL_GPIO_PULL_UP); 
    LL_GPIO_SetPinPull(EXT_D012367_PORT,    EXT_D2, LL_GPIO_PULL_UP); 
    LL_GPIO_SetPinPull(EXT_D012367_PORT,    EXT_D3, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinPull(EXT_D4_PORT,         EXT_D4, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinPull(EXT_D5_PORT,         EXT_D5, LL_GPIO_PULL_UP); 
    LL_GPIO_SetPinPull(EXT_D012367_PORT,    EXT_D6, LL_GPIO_PULL_UP); 
    LL_GPIO_SetPinPull(EXT_D012367_PORT,    EXT_D7, LL_GPIO_PULL_UP); 

    LL_GPIO_SetOutputPin(EXT_D012367_PORT,    EXT_D0); 
    LL_GPIO_SetOutputPin(EXT_D012367_PORT,    EXT_D1); 
    LL_GPIO_SetOutputPin(EXT_D012367_PORT,    EXT_D2); 
    LL_GPIO_SetOutputPin(EXT_D012367_PORT,    EXT_D3);
    LL_GPIO_SetOutputPin(EXT_D4_PORT,         EXT_D4);
    LL_GPIO_SetOutputPin(EXT_D5_PORT,         EXT_D5); 
    LL_GPIO_SetOutputPin(EXT_D012367_PORT,    EXT_D6); 
    LL_GPIO_SetOutputPin(EXT_D012367_PORT,    EXT_D7);
     
    wait_key_press(osWaitForever);
    
    dbg_print("[Test] Test extent interface, D0~D7 Low level ...\n");

    LL_GPIO_ResetOutputPin(EXT_D012367_PORT,    EXT_D0); 
    LL_GPIO_ResetOutputPin(EXT_D012367_PORT,    EXT_D1); 
    LL_GPIO_ResetOutputPin(EXT_D012367_PORT,    EXT_D2); 
    LL_GPIO_ResetOutputPin(EXT_D012367_PORT,    EXT_D3);
    LL_GPIO_ResetOutputPin(EXT_D4_PORT,         EXT_D4);
    LL_GPIO_ResetOutputPin(EXT_D5_PORT,         EXT_D5); 
    LL_GPIO_ResetOutputPin(EXT_D012367_PORT,    EXT_D6); 
    LL_GPIO_ResetOutputPin(EXT_D012367_PORT,    EXT_D7);
    
    wait_key_press(osWaitForever);    
}

void test_program_power(void)
{
    int32_t err;
    struct key_event evt;
    struct voltage     vdd;
    struct voltage     vpp;
    
    io_icsp_init();

    vdd.rated = 0;
    vpp.rated = 0;

    vpp.error = 300;
    vdd.error = 300;

    while (1) {
        dbg_print("[Test] Set Vdd to: %d, Vpp to:%d ...\n", vdd.rated, vpp.rated);
        err = power_voltage_set(POWER_TYPE_VDD, &vdd);
        if (err) {
            dbg_print("\t power_voltage_set(vdd) Failed\n");
            return;
        }
        err = power_voltage_set(POWER_TYPE_VPP, &vpp);
        if (err) {
            dbg_print("\t power_voltage_set(vpp) Failed\n");
            return;
        }
        
        dbg_print("[Test] Turn on Vdd, Vpp\n");
        err = power_voltage_switch(POWER_TYPE_VDD, POWER_FLAG_ON);
        if (err) {
            dbg_print("\t power_voltage_switch(vdd) Failed\n");
            return;
        }
        
        err = power_voltage_switch(POWER_TYPE_VPP, POWER_FLAG_ON);
        if (err) {
            dbg_print("\t power_voltage_switch(vpp) Failed\n");
            return;
        }
                   
        wait_key_press(osWaitForever);
            
        if (vdd.rated < 5500) {
            vdd.rated += 500;
        }
        if (vpp.rated < 9000) {
            vpp.rated += 500;
        }
        
        dbg_print("[Test] Turn off Vdd, Vpp\n");
        err = power_voltage_switch(POWER_TYPE_VDD, POWER_FLAG_OFF);
        if (err) {
            dbg_print("\t power_voltage_switch(vdd) Failed\n");
            return;
        }
        
        err = power_voltage_switch(POWER_TYPE_VPP, POWER_FLAG_OFF);
        if (err) {
            dbg_print("\t power_voltage_switch(vpp) Failed\n");
            return;
        }

        if (vdd.rated >= 5500 && vpp.rated >= 9000) { 
            
            wait_key_press(osWaitForever);
            return;
        }
    }
}

void test_programing(void)
{
    int32_t err;
    int32_t i;
    struct voltage     vdd;
    struct voltage     vpp;


    io_icsp_init();

    vdd.rated = 5000;
    vpp.rated = 8500;

    vpp.error = 300;
    vdd.error = 300;

    dbg_print("[Test] Programming test\n");
    dbg_print("[Test] Set Vdd to: %d, Vpp to: %d\n", vdd.rated, vpp.rated);
    err = power_voltage_set(POWER_TYPE_VDD, &vdd);
    if (err) {
        dbg_print("\t power_voltage_set(vdd) Failed\n");
        return;
    }
    err = power_voltage_set(POWER_TYPE_VPP, &vpp);
    if (err) {
        dbg_print("\t power_voltage_set(vpp) Failed\n");
        return;
    }  

    while (1) {
            
        dbg_print("[Test] Turn on Vdd and Vpp...\n");
        err = power_voltage_switch(POWER_TYPE_VDD, POWER_FLAG_ON);
        if (err) {
            dbg_print("\t power_voltage_switch(vdd) Failed\n");
            return;
        }
        err = power_voltage_switch(POWER_TYPE_VPP, POWER_FLAG_ON);
        if (err) {
            dbg_print("\t power_voltage_switch(vpp) Failed\n");
            return;
        }

        delay_us(500);
        dbg_print("[Test] Programming ...\n");
        for (i = 0; i < 100; i++) {            
            dbg_print("[Test] io_icsp_send(0xAA)\n");
            io_icsp_send(0xAA, 8);
            dbg_print("[Test] io_icsp_send(0x55)\n");
            io_icsp_send(0x55, 8);
            delay_us(20);
        }
        
        dbg_print("[Test] Turn off Vdd, Vpp\n");
        err = power_voltage_switch(POWER_TYPE_VDD, POWER_FLAG_OFF);
        if (err) {
            dbg_print("\t power_voltage_switch(vdd) Failed\n");
            return;
        }
        
        err = power_voltage_switch(POWER_TYPE_VPP, POWER_FLAG_OFF);
        if (err) {
            dbg_print("\t power_voltage_switch(vpp) Failed\n");
            return;
        }

        delay_ms(500);
        
        err = wait_key_press(1);
        if (err) {
            break;
        }
    }
}

void test_program(void)
{
    test_program_power();
    test_programing();
}

void test_vs(void)
{
    int32_t err;
    int32_t ad_vs;
    int32_t ad_vs_b;
    
    dbg_print("%s", "[Test] VS config to output:\n");
    err = vs_cali_config(VS_CALI_MODE_OUTPUT_2500_MV);

    while (1) {
        dbg_print("%s", "[Test] VS Output, any key press quit\n");
        ad_vs = vs_ad_vs_filter();
        ad_vs_b = vs_ad_vs_b_filter();
        dbg_print("\t AD-VS: %d mV\n", ad_vs);
        dbg_print("\t AD-VS-B: %d mV\n", ad_vs_b);
        err = wait_key_press(1);
        if (err) {
            break;
        }
    }
    delay_ms(10);
    err = vs_cali_config(VS_CALI_MODE_INPUT);
    while (1) {
        dbg_print("%s", "[Test] VS Input, any key press quit\n");
        ad_vs = vs_ad_vs_filter();
        ad_vs_b = vs_ad_vs_b_filter();
        dbg_print("\t AD-VS: %d mV\n", ad_vs);
        dbg_print("\t AD-VS-B: %d mV\n", ad_vs_b);
        err = wait_key_press(1);
        if (err) {
            break;
        }
    }
           
}


void test_collection(void)
{  


    dbg_init();
    delay_init();
    
    dbg_print("CSWrite3.0 Test Collection, Build at %s %d\n", __DATE__, __LINE__);
 
    lcd_init();
    key_init();
    buzzer_init();
    fram_init();

    test_lcd();
    test_flash();
    test_fram();
    test_led();
    test_temp();
    test_ext();
    test_vs();
    test_program();
    
}
