#ifndef __LCD_H__
#define __LCD_H__

#include <stdint.h>


#define LCD_COLOR_WHITE         0xFFFF
#define LCD_COLOR_BLACK         0x0000    
#define LCD_COLOR_BLUE          0x1F00  
#define LCD_COLOR_BRED          0XF81F
#define LCD_COLOR_GRED          0XFFE0
#define LCD_COLOR_GBLUE         0X07FF
#define LCD_COLOR_RED           0x00F8
#define LCD_COLOR_MAGENTA       0xF81F
#define LCD_COLOR_GREEN         0x003F
#define LCD_COLOR_CYAN          0x7FFF
#define LCD_COLOR_YELLOW        0xFFE0
#define LCD_COLOR_BROWN         0XBC40 
#define LCD_COLOR_BRRED         0XFC07 
#define LCD_COLOR_GRAY          0X8430 




#define LCD_ROW_MAX               (320)
#define LCD_COL_MAX               (240)

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


int32_t lcd_init(void);
int32_t lcd_view_port_show(struct lcd_rect* rect, uint16_t* pixel);
int32_t lcd_pixel_show(struct lcd_point* p, uint16_t color);
int32_t lcd_back_light_set(uint32_t flag, int32_t toggle_time);
int32_t lcd_color_show(uint16_t color);

#endif /* __LCD_H__ */
