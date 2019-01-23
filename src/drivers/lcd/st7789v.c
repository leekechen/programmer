#include <cmsis_os2.h>
#include <stm32f4xx_hal.h>
#include <Driver_SPI.h>
#include <RTX_Config.h>
#include <MX_Device.h>
#include "error.h"
#include "dbg_print.h"
#include "st7789v.h"

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
#define LCD_CMD_RDID_SPECICAL			(0xD4)
#define LCD_CMD_MEM_READ				(0x2E)


#define LCD_PIN_RESET_SET(f)  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, (GPIO_PinState)(f))
#define LCD_PIN_CS_SET(f)     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, (GPIO_PinState)(f))
#define LCD_PIN_DCX_SET(f)    HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, (GPIO_PinState)(f))


extern ARM_DRIVER_SPI Driver_SPI2;

static osTimerId_t timer;
static GPIO_PinState lcd_back_light_state;
static ARM_DRIVER_SPI* spi = &Driver_SPI2;
uint32_t UCGUI_LCD_CMD  = 0x6007FFFE;
uint32_t UCGUI_LCD_DATA = 0x60080000;

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

#if		1
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
#endif


int32_t st7789v_init(void)
{
    int32_t err;    
   
    lcd_back_light_set(1, LCD_BACK_LIGHT_TOGGLE_INFINTE);
    
    NVIC_ClearPendingIRQ(SPI2_IRQn);
    NVIC_EnableIRQ(SPI2_IRQn);
    
    err = spi->Initialize(NULL);
    if (err) {
        return err;
    }
    
    err = spi->PowerControl(ARM_POWER_FULL);
    if (err) {
        return err;
    }
    
    err = spi->Control(LCD_SPI_BUS_FLAG, 21000000);
    if (err) {
        return err;
    }
    
    lcd_ctrl_reset();
    
    err = lcd_ctrl_sleep_out();
    if (err) {
        return err;
    }
    
    err = lcd_ctrl_power();
    if (err) {
        return err;
    }
    
    err = lcd_ctrl_pixel_format();
    if (err) {
        return err;
    }

    err = lcd_ctrl_vcom_set();
    if (err) {
        return err;
    }

    err = lcd_ctrl_positive_vol_gamma_ctrl();
    if (err) {
        return err;
    }
    
    err = lcd_ctrl_negative_vol_gamma_ctrl();
    if (err) {
        return err;
    }

	#if  defined(VERTICAL_ENABLE)
	#else
		/* Horizontal screen display */
		err = lcd_io_write_cmd(0x36);
		if (err) {
			return err;
		}
		err = lcd_io_write_data(0x60);
		if (err) {
			return err;
		}
	#endif

    err = lcd_ctrl_display_on();   
    if (err) {
        return err;
    } 
    
    err = lcd_ctrl_view_port_enable();  
    if (err) {
        return err;
    } 

    err = lcd_ctrl_read_id();  
    if (err) {
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





#if		1
/**
  * @brief  Set Cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @retval None
  */
int32_t st7789v_set_cursor(uint16_t Xpos, uint16_t Ypos)
{
    int32_t err;
    
    err = lcd_io_write_cmd(LCD_CMD_COL_ADDR_SET);  
    if (err) {
        return err;
    }

    err = lcd_io_write_data(Xpos >> 8); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(Xpos & 0xFF); 
    if (err) {
        return err;
    } 
    
    err = lcd_io_write_cmd(LCD_CMD_ROW_ADDR_SET);  
    if (err) {
        return err;
    }

    err = lcd_io_write_data(Ypos >> 8); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(Ypos & 0xFF); 
    if (err) {
        return err;
    }

    return err; 
}

/**
  * @brief  Set up display window.
  * @param  x_start: x starting position.
  * @param  y_start: y starting position.
  * @param  x_end: x end position.
  * @param  y_end: y end position.
  * @retval None
  */
int32_t st7789v_set_window(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    int32_t err;

    err = lcd_io_write_cmd(LCD_CMD_COL_ADDR_SET);  
    if (err) {
        return err;
    }

    err = lcd_io_write_data(x_start >> 8); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(x_start & 0xFF); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(x_end >> 8); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(x_end & 0xFF); 
    if (err) {
        return err;
    }  
    
    err = lcd_io_write_cmd(LCD_CMD_ROW_ADDR_SET);  
    if (err) {
        return err;
    }


    err = lcd_io_write_data(y_start >> 8); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(y_start & 0xFF); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(y_end >> 8); 
    if (err) {
        return err;
    }

    err = lcd_io_write_data(y_end & 0xFF); 
    if (err) {
        return err;
    }

    return err; 
}

/**
  * @brief Sets the index of the given pixel.
  */
int32_t st7789v_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    int32_t err;
    struct lcd_rect rect;
	uint8_t color_high, color_low;
	
	rect.start.x = x;
	rect.start.y = y;
	rect.end.x = x + 1;
	rect.end.y = y + 1;
	err = st7789v_set_window(rect.start.x, rect.start.y, rect.end.x, rect.end.y);
    if (err)
	{
        return err;
    }
	
    err = lcd_io_write_cmd(LCD_CMD_MEM_WRITE);  
    if (err)
	{
        return err;
    }
	color_high = (uint8_t)(color >> 8);
	color = (color << 8) + color_high;
    err = lcd_io_mem_write(&color, 1);
    
    return err; 
}

/**
  * @brief Returns the index of the given pixel.
  */
int32_t st7789v_read_point(uint16_t x, uint16_t y, uint16_t *pixel)
{
    int32_t err;
    struct  lcd_rect rect;
    uint8_t pixel_p[5];

	rect.start.x = x;
	rect.start.y = y;
	rect.end.x = x + 1;
	rect.end.y = y + 1;
	err = st7789v_set_window(rect.start.x, rect.start.y, rect.end.x, rect.end.y);
    if (err)
	{
        return err;
    }
	
	err = lcd_io_transfer(LCD_CMD_MEM_READ, &pixel_p[0], 5);
    if (err)
	{
        return err;
    }
    pixel_p[2] = pixel_p[2] >> 3;
	pixel_p[3] = pixel_p[3] >> 2;
	pixel_p[4] = pixel_p[4] >> 3;
	*pixel =  pixel_p[2];
	*pixel = (*pixel << 6) + pixel_p[3];
	*pixel = (*pixel << 5) + pixel_p[4];
    
    return err;
}

/**
  * @brief Fill color.
  */
int32_t st7789v_fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    int32_t  err;
    struct   lcd_rect rect;
	uint8_t  color_high;
	uint16_t col_length, row_length;
    uint16_t col, row;

    rect.start.x = x_start;
    rect.start.y = y_start;
    rect.end.x   = x_end;
    rect.end.y   = y_end;
	err = st7789v_set_window(rect.start.x, rect.start.y, rect.end.x, rect.end.y);
    if (err)
	{
        return err;
    }

    err = lcd_io_write_cmd(LCD_CMD_MEM_WRITE);  
    if (err)
	{
        return err;
    }
    color_high = (uint8_t)(color >> 8);
	color = (color << 8) + color_high;
	col_length = x_end - x_start + 1;
	row_length = y_end - y_start + 1;
    for (row = 0; row < row_length; row++)
	{
        for (col = 0; col < col_length; col++)
		{
            err = lcd_io_mem_write(&color, 1);
        }
    }

    return err;
}

/**
  * @brief Drawing of 16bpp high color bitmaps.
  */
int32_t st7789v_draw_16bit_bitmap(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t* pixel)
{
    return 0;
}

/**
  * @brief Testing the driver of LCD.
  */
#if		0
int32_t lcd_color_show(uint16_t color)
{
	int32_t err;
	uint16_t Xpos_i1, Ypos_i1;
	uint16_t lcs;
	
	for(Ypos_i1 = 0; Ypos_i1 < 80; Ypos_i1++)
	{
		for(Xpos_i1 = 0; Xpos_i1 < 40; Xpos_i1++)
		{
			st7789v_draw_point(Xpos_i1, Ypos_i1, LCD_COLOR_RED);
		}
	}
	for(Ypos_i1 = 80; Ypos_i1 < 160; Ypos_i1++)
	{
		for(Xpos_i1 = 0; Xpos_i1 < 40; Xpos_i1++)
		{
			st7789v_draw_point(Xpos_i1, Ypos_i1, LCD_COLOR_GREEN);
		}
	}
	for(Ypos_i1 = 160; Ypos_i1 < 240; Ypos_i1++)
	{
		for(Xpos_i1 = 0; Xpos_i1 < 40; Xpos_i1++)
		{
			st7789v_draw_point(Xpos_i1, Ypos_i1, LCD_COLOR_BLUE);
		}
	}
	
    err = st7789v_read_point(0, 79, &lcs);
	err = st7789v_read_point(0, 80, &lcs);
	err = st7789v_read_point(0, 159, &lcs);
	err = st7789v_read_point(0, 160, &lcs);
	err = st7789v_read_point(0, 239, &lcs);
	err = st7789v_read_point(0, 240, &lcs);
	
	st7789v_fill(40, 0, 79, 319, LCD_COLOR_YELLOW);
	
	return err;
}
#endif
#endif
