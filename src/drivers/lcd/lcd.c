#include <cmsis_os2.h>
#include <stm32f4xx_hal.h>
#include <Driver_SPI.h>
#include <RTX_Config.h>
#include <MX_Device.h>
#include "error.h"
#include "lcd.h"

#define MODULE_DEBUG                1
#define MODULE_ERROR                1

#include "dbg_print.h"


#define LCD_SPI_BUS_FLAG            (ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0  \
                                    | ARM_SPI_DATA_BITS(8) | ARM_SPI_MSB_LSB    \
                                    | ARM_SPI_SS_MASTER_UNUSED)
//#define LCD_SPI_BUS_FLAG            (ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0  \
//                                | ARM_SPI_DATA_BITS(8) | ARM_SPI_MSB_LSB    \
//                                | ARM_SPI_SS_MASTER_SW)                                

#define LCD_ID_1                        (0x85)
#define LCD_ID_2                        (0x85)
#define LCD_ID_3                        (0x52)


#define LCD_CMD_RDDID                   (0x04) 

#define LCD_CMD_SLEEP_OUT               (0x11)
#define LCD_CMD_DISPLAY_OFF             (0X28)
#define LCD_CMD_DISPLAY_ON              (0X29)
#define LCD_CMD_COL_ADDR_SET            (0x2A)
#define LCD_CMD_ROW_ADDR_SET            (0x2B)
#define LCD_CMD_MEM_WRITE               (0X2C)
#define LCD_CMD_INTERFACE_PIXEL_FORMAT  (0x3A)
#define LCD_CMD_VCOM_SET                (0XBB)
#define LCD_CMD_POWER_CTRL_1            (0xD0)
#define LCD_CMD_RDID_1                  (0xDA) 
#define LCD_CMD_RDID_2                  (0xDB)  
#define LCD_CMD_RDID_3                  (0xDC)                 
#define LCD_CMD_POSITIVE_VOL_GAMMA_CTRL (0XE0)
#define LCD_CMD_NEGATIVE_VOL_GAMMA_CTRL (0XE1)


#define LCD_PIN_RESET_SET(f)  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, (GPIO_PinState)(f))
#define LCD_PIN_CS_SET(f)     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, (GPIO_PinState)(f))
#define LCD_PIN_DCX_SET(f)    HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, (GPIO_PinState)(f))


extern ARM_DRIVER_SPI Driver_SPI2;

static osTimerId_t timer;
static GPIO_PinState lcd_back_light_state;
static ARM_DRIVER_SPI* spi = &Driver_SPI2;

static void lcd_back_light_timer_callback(void* arg)
{
    HAL_GPIO_WritePin(BACK_LIGHT_GPIO_Port, BACK_LIGHT_Pin, ~lcd_back_light_state);
}   


static int32_t lcd_io_wait_until_timeout(ARM_DRIVER_SPI* spi, uint32_t length, uint32_t timeout)
{
    uint32_t cur;
    uint32_t end;
    
    end = osKernelGetTickCount() + timeout;
   
    while (spi->GetDataCount() != length) {
        cur = osKernelGetTickCount();   
        if (cur > end) {            
            spi->Control(ARM_SPI_ABORT_TRANSFER, 0);
            err_print("spi->GetDataCount() timeout!\n");
            return ARM_DRIVER_ERROR_TIMEOUT;
        }
    }

    return ARM_DRIVER_OK;
}


static int32_t lcd_io_write_cmd(uint8_t cmd)
{
    int32_t  err;   
    
    LCD_PIN_CS_SET(0);
    LCD_PIN_DCX_SET(0);
    
    err = spi->Send(&cmd, 1);
    if (err) {
        return err;
    }
    
    err = lcd_io_wait_until_timeout(spi, 1, 100);

    LCD_PIN_CS_SET(1);
   
    return err;    
}

static int32_t lcd_io_write_data(uint8_t data)
{
    int32_t  err;   
    
    LCD_PIN_CS_SET(0);
    LCD_PIN_DCX_SET(1);
    
    err = spi->Send(&data, 1);
    if (err) {
        return err;
    }
    
    err = lcd_io_wait_until_timeout(spi, 1, 100);

    LCD_PIN_CS_SET(1);
   
    return err;  
}

static int32_t lcd_io_transfer(uint8_t cmd, uint8_t* buf, size_t size)
{
    int32_t  err;   
    
    LCD_PIN_CS_SET(0);
    LCD_PIN_DCX_SET(0);

    err = spi->Send(&cmd, 1);
    if (err) {
        return err;
    }
    
    err = lcd_io_wait_until_timeout(spi, 1, 100);
    if (err) {
        return err;
    }
    
    LCD_PIN_DCX_SET(1);
    
    err = spi->Receive(buf, size);
    if (err) {
        return err;
    }
    
    err = lcd_io_wait_until_timeout(spi, size, 100);

    LCD_PIN_CS_SET(1);
   
    return err;  
}


static int32_t lcd_io_mem_write(uint16_t* pixel, size_t size)
{ 
    int32_t  err; 
    int32_t  i;
    
    LCD_PIN_CS_SET(0);
    LCD_PIN_DCX_SET(1);

    err = spi->Send(pixel, size * 2);
    if (err) {
        return err;
    }
    
    err = lcd_io_wait_until_timeout(spi, size * 2, 100);

    LCD_PIN_CS_SET(1);
   
    return err;  
}

static void lcd_ctrl_reset(void)
{
    LCD_PIN_RESET_SET(1);
    osDelay(50);
    LCD_PIN_RESET_SET(0);
    osDelay(100);
    LCD_PIN_RESET_SET(1);
    osDelay(50);

    return;
}


static int32_t lcd_ctrl_sleep_out(void)
{
    int32_t err;

    err = lcd_io_write_cmd(LCD_CMD_SLEEP_OUT);
    osDelay(120);

    return err;
}

static int32_t lcd_ctrl_power(void)
{
    int32_t err;
    
    err = lcd_io_write_cmd(LCD_CMD_POWER_CTRL_1); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0xa4); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x81);

    return err;    
}

static int32_t lcd_ctrl_read_id(void)
{
    int32_t err;
    uint8_t id[4];

    err = lcd_io_transfer(LCD_CMD_RDDID, id, 4); 
    if (err) {
        return err;
    }
   
    err = lcd_io_transfer(LCD_CMD_RDID_1, &id[1], 1); 
    if (err) {
        return err;
    }
    
    err = lcd_io_transfer(LCD_CMD_RDID_2, &id[2], 1); 
    if (err) {
        return err;
    }  
    
    err = lcd_io_transfer(LCD_CMD_RDID_3, &id[3], 1); 
    
    return err;
}

static int32_t lcd_ctrl_pixel_format(void)
{
    int32_t err;
    
    err = lcd_io_write_cmd(LCD_CMD_INTERFACE_PIXEL_FORMAT);
    if (err) {
        return err;
    }
    
    err = lcd_io_write_data(0x05);

    return err; 

}

static int32_t lcd_ctrl_vcom_set(void)
{
    int32_t err;
    
    err = lcd_io_write_cmd(LCD_CMD_VCOM_SET); 
    if (err) {
        return err;
    }
    
    err = lcd_io_write_data(0x35);


    return err; 
}


static int32_t lcd_ctrl_positive_vol_gamma_ctrl(void)
{
    int32_t err;
    
    err = lcd_io_write_cmd(LCD_CMD_POSITIVE_VOL_GAMMA_CTRL); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0xd0); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x00); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x02); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x07); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x0b); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x1a); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x31);
    if (err) {
        return err;
    } 
    err = lcd_io_write_data(0x54); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x40); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x29); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x12);
    if (err) {
        return err;
    } 
    err = lcd_io_write_data(0x12); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x12); 
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x17);


    return err; 
}

static int32_t lcd_ctrl_negative_vol_gamma_ctrl(void)
{
    int32_t err;

    err = lcd_io_write_cmd(LCD_CMD_NEGATIVE_VOL_GAMMA_CTRL);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0xd0);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x00);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x02);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x07);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x05);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x25);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x2d);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x44);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x45);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x1c);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x18);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x16);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x1c);  
    if (err) {
        return err;
    }
    err = lcd_io_write_data(0x1d);  
    if (err) {
        return err;
    }


    return err;
}

static int32_t lcd_ctrl_display_on(void)
{
    return lcd_io_write_cmd(LCD_CMD_DISPLAY_ON); 
}

static int32_t lcd_ctrl_display_off(void)
{
    return lcd_io_write_cmd(LCD_CMD_DISPLAY_OFF); 
}


static int32_t lcd_ctrl_view_port_enable(void)
{
    return lcd_io_write_cmd(LCD_CMD_MEM_WRITE);  
}

static int32_t lcd_ctrl_view_port_set(struct lcd_rect* rect)
{
    int32_t err;
    
    err = lcd_io_write_cmd(LCD_CMD_COL_ADDR_SET);  
    if (err) {
        return err;
    }

    err = lcd_io_write_data(rect->start.x >> 8); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(rect->start.x & 0xFF); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(rect->end.x >> 8); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(rect->end.x & 0xFF); 
    if (err) {
        return err;
    }  
    
    err = lcd_io_write_cmd(LCD_CMD_ROW_ADDR_SET);  
    if (err) {
        return err;
    }

    err = lcd_io_write_data(rect->start.y >> 8); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(rect->start.y & 0xFF); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(rect->end.y >> 8); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(rect->end.y & 0xFF); 
    if (err) {
        return err;
    }  

    err = lcd_ctrl_view_port_enable();

    return err; 
}


int32_t lcd_color_show(uint16_t color)
{
    int32_t  err;
    uint16_t row;
    uint16_t col;
    struct   lcd_rect rect;

    rect.start.x = 0;
    rect.start.y = 0;
    rect.end.x   = LCD_COL_MAX;
    rect.end.y   = LCD_ROW_MAX;

    err = lcd_ctrl_view_port_set(&rect); 
    if (err) {
        return err;
    } 

    LCD_PIN_CS_SET(0);
    LCD_PIN_DCX_SET(1);

    for (row = 0; row < LCD_ROW_MAX; row++) {
        for (col = 0; col < LCD_COL_MAX; col++) {
            err = lcd_io_mem_write(&color, 1);
        }
    }

    LCD_PIN_CS_SET(1);

    return err;
}



int32_t lcd_init(void)
{
    int32_t err;    
   
    lcd_back_light_set(1, LCD_BACK_LIGHT_TOGGLE_INFINTE);
    
    NVIC_ClearPendingIRQ(SPI2_IRQn);
    NVIC_EnableIRQ(SPI2_IRQn);
    
    err = spi->Initialize(NULL);
    if (err) {
        err_print("spi->Initializl() failed\n");
        return err;
    }
    
    err = spi->PowerControl(ARM_POWER_FULL);
    if (err) {
        err_print("spi->PowerControl() failed\n");
        return err;
    }
    
    err = spi->Control(LCD_SPI_BUS_FLAG, 21000000);
    if (err) {
        err_print("spi->Control() failed\n");
        return err;
    }
    
    lcd_ctrl_reset();
    
    err = lcd_ctrl_sleep_out();
    if (err) {
        err_print("lcd_ctrl_sleep_out() failed\n");
        return err;
    }
    
    err = lcd_ctrl_power();
    if (err) {
        err_print("lcd_ctrl_power() failed\n");
        return err;
    }
    
    err = lcd_ctrl_pixel_format();
    if (err) {
        err_print("lcd_ctrl_pixel_format() failed\n");
        return err;
    }

    err = lcd_ctrl_vcom_set();
    if (err) {
        err_print("lcd_ctrl_vcom_set() failed\n");
        return err;
    }

    err = lcd_ctrl_positive_vol_gamma_ctrl();
    if (err) {
        err_print("lcd_ctrl_positive_vol_gamma_ctrl() failed\n");
        return err;
    }
    
    err = lcd_ctrl_negative_vol_gamma_ctrl();
    if (err) {
        err_print("lcd_ctrl_negative_vol_gamma_ctrl() failed\n");
        return err;
    }

    err = lcd_ctrl_display_on();   
    if (err) {
        err_print("lcd_ctrl_display_on() failed\n");
        return err;
    } 
    
    err = lcd_ctrl_view_port_enable();  
    if (err) {
        err_print("lcd_ctrl_view_port_enable() failed\n");
        return err;
    } 

    err = lcd_ctrl_read_id();  
    if (err) {
        err_print("lcd_ctrl_read_id() failed\n");
        return err;
    } 

    timer = osTimerNew(lcd_back_light_timer_callback, osTimerOnce, NULL, NULL);
    if (timer == NULL) {
        err_print("%s(), osTimerNew() failed\n", __func__);
        return ERR_SYS_OS;
    }   
    
    return ERR_OK;
}

int32_t lcd_view_port_show(struct lcd_rect* rect, uint16_t* pixel)
{
    int32_t  err;
    uint16_t row;
    uint16_t col;
    
    col = rect->end.x - rect->start.x;
    row = rect->end.y - rect->start.y;

    err = lcd_ctrl_view_port_set(rect); 
    if (err) {
        return err;
    } 

    LCD_PIN_CS_SET(0);
    LCD_PIN_DCX_SET(1);
    
    err = lcd_io_mem_write(pixel, col * row);

    LCD_PIN_CS_SET(1);

    return err;
}


int32_t lcd_back_light_set(uint32_t flag, int32_t toggle_time)
{
    HAL_GPIO_WritePin(BACK_LIGHT_GPIO_Port, BACK_LIGHT_Pin, (GPIO_PinState)flag);
    lcd_back_light_state = flag;
    
    if (toggle_time != LCD_BACK_LIGHT_TOGGLE_INFINTE) {
        osTimerStart(timer, toggle_time);
    }
    
    return ERR_OK;
}

int32_t lcd_pixel_show(struct lcd_point* p, uint16_t color)
{
    int32_t err;
    struct lcd_rect rect;

    rect.start = *p;
    rect.end.x = rect.start.x + 1;
    rect.end.y = rect.start.y + 1;

    err = lcd_view_port_show(&rect, &color);
    return err;    
}

