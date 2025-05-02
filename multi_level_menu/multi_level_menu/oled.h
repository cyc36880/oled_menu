#ifndef _oled_h
#define _oled_h

#include "main.h"

#define  u8 unsigned char 
#define	 u16 unsigned short int
#define  u32 unsigned int 
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

//sbit OLED_CS=P1^4; //片选
//sbit OLED_RST =P1^2;//复位
//sbit OLED_DC =P1^3;//数据/命令控制
//sbit OLED_SCL=P1^0;//时钟 D0（SCLK?
//sbit OLED_SDIN=P1^1;//D1（MOSI） 数据


#define OLED_CS_Clr()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET)
#define OLED_CS_Set()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET)

#define OLED_RST_Clr() HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET)
#define OLED_RST_Set() HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET)
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET)

   		  					   
void OLED_Init(void);          //初始化

void write_point(uint16_t x, uint16_t y, uint16_t w_d); //缓冲区画点

void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);//在8x128的数组里画线

void disp_flush(void);//刷新整个屏幕

#endif







