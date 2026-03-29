#ifndef __OLED1315_H
#define __OLED1315_H

#include "main.h"

/***************根据自己需求更改****************/
#define OLED_SCL_PORT  			GPIOB
#define OLED_SCL_PIN			GPIO_PIN_6
#define OLED_SDA_PORT  			GPIOB
#define OLED_SDA_PIN			GPIO_PIN_7
/*********************END**********************/

void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Init(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode);
void OLED_DrawCircle(uint8_t x,uint8_t y,uint8_t r);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode,uint8_t refresh);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode,uint8_t refresh);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1,uint8_t mode,uint8_t refresh);
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1,uint8_t mode);
void OLED_ScrollDisplay(uint8_t num,uint8_t space,uint8_t mode);
void OLED_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode);

#endif
