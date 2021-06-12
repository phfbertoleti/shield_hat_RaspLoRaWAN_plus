/* Header file - display OLED */

#ifndef OLED_DEFS_H
#define OLED_DEFS_H

/* Definições - OLED */
#define OLED_CLEAR_LINE      "                     "
#define OLED_LINE_MAX_SIZE   21
#define OLED_SDA_PIN         4
#define OLED_SCL_PIN         15
#define OLED_ADDRESS         0x3C
#define OLED_RESET           16 
#define OLED_SCREEN_HEIGHT   64
#define OLED_SCREEN_WIDTH    128
#define OLED_LINHA_1         0
#define OLED_LINHA_2         10
#define OLED_LINHA_3         20
#define OLED_LINHA_4         30
#define OLED_LINHA_5         40
#define OLED_LINHA_6         50

Adafruit_SSD1306 display(OLED_SCREEN_WIDTH, 
                         OLED_SCREEN_HEIGHT, 
                         &Wire, 
                         OLED_RESET, 
                         100000UL, 
                         100000UL);

#endif
