#ifndef __ST7789V_H__
#define __ST7789V_H__

#include <stdint.h>

#define LCD_COLOR_WHITE         0xFFFF
#define LCD_COLOR_BLACK         0x0000    
#define LCD_COLOR_BLUE          0x001F  
#define LCD_COLOR_BRED          0XF81F
#define LCD_COLOR_GRED          0XFFE0
#define LCD_COLOR_GBLUE         0X07FF
#define LCD_COLOR_RED           0xF800
#define LCD_COLOR_MAGENTA       0xF81F
#define LCD_COLOR_GREEN         0x07E0
#define LCD_COLOR_CYAN          0x7FFF
#define LCD_COLOR_YELLOW        0xFFE0
#define LCD_COLOR_BROWN         0XBC40 
#define LCD_COLOR_BRRED         0XFC07 
#define LCD_COLOR_GRAY          0X8430 


//#define VERTICAL_ENABLE
#if  defined(VERTICAL_ENABLE)
	/* Vertical screen display */
	#define LCD_ROW_MAX         (320)
	#define LCD_COL_MAX         (240)
#else
	/* Horizontal screen display */
	#define LCD_ROW_MAX         (240)
	#define LCD_COL_MAX         (320)
#endif


#define LCD_FRAME_BUF_SIZE_MAX    (LCD_ROW_MAX * LCD_COL_MAX)

#define LCD_BACK_LIGHT_TOGGLE_INFINTE             -1

struct lcd_point {
    uint16_t x;
    uint16_t y;
} ;

struct lcd_rect {
    struct lcd_point    start;
    struct lcd_point    end;
} ;



extern uint32_t UCGUI_LCD_CMD;
extern uint32_t UCGUI_LCD_DATA;


int32_t st7789v_init(void);
int32_t lcd_view_port_show(struct lcd_rect* rect, uint16_t* pixel);
int32_t lcd_pixel_show(struct lcd_point* p, uint16_t color);
int32_t lcd_back_light_set(uint32_t flag, int32_t toggle_time);
int32_t lcd_color_show(uint16_t color);

int32_t st7789v_set_cursor(uint16_t Xpos, uint16_t Ypos);
int32_t st7789v_set_window(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);
int32_t st7789v_draw_point(uint16_t x, uint16_t y, uint16_t color);
int32_t st7789v_read_point(uint16_t x, uint16_t y, uint16_t *pixel);
int32_t st7789v_fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
int32_t st7789v_draw_16bit_bitmap(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t* pixel);

#endif /* __LCD_H__ */
