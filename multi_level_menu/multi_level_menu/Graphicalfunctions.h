#ifndef _Graphicalfunctions_h_
#define _Graphicalfunctions_h_

#include "menu.h"

// 画点
void write_point(int16_t x, int16_t y, uint8_t w_d);
// 读点
unsigned char read_point(int16_t x, int16_t y);

//在8x128的数组里画线
void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);

//空心圆
void DrawCircle(int16_t x0,int16_t y0,uint8_t r);
//实心圆
void DrawCircle_Solid(int16_t x0,int16_t y0,uint8_t r);

//空心圆角矩形
void DrawRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);
//实心圆角矩形
void DrawfillRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);

//多边形变换
void PolygonTransformation(int16_t x0,int16_t y0,uint16_t r, uint16_t n);

//旋转目标点
void RotateXY(int *xy, int centerX, int centerY,int x, int y,int Angle,int direct);


#endif
