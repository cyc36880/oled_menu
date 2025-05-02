#ifndef _oled_h
#define _oled_h

#include "main.h"//HAL库

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
// 画点
void write_point(int16_t x, int16_t y, uint8_t w_d);
// 读点
unsigned char read_point(int16_t x, int16_t y);

//在8x128的数组里画线
void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);

//空心圆
void DrawCircle(int16_t x0,int16_t y0,u8 r);
//实心圆
void DrawCircle_Solid(int16_t x0,int16_t y0,u8 r);

//空心圆角矩形
void DrawRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);
//实心圆角矩形
void DrawfillRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);

//多边形变换
void PolygonTransformation(int16_t x0,int16_t y0,u16 r, u16 n);

//旋转目标点
void RotateXY(int *xy, int centerX, int centerY,int x, int y,int Angle,int direct);

//刷新整个屏幕
void disp_flush(void);



unsigned char read_point(int16_t x, int16_t y);

#endif







