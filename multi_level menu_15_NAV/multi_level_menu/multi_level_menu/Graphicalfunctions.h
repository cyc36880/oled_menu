#ifndef _Graphicalfunctions_h_
#define _Graphicalfunctions_h_

#include "menu.h"


void write_point(int16_t x, int16_t y, uint8_t w_d);
unsigned char read_point(int16_t x, int16_t y);
void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void DrawCircle(int16_t x0,int16_t y0,uint8_t r);
void DrawCircle_Solid(int16_t x0,int16_t y0,uint8_t r);
void DrawRect(int16_t x, int16_t y, uint16_t w, uint16_t h);
void DrawFillRect(int16_t x, int16_t y, uint16_t w, uint16_t h);
void DrawRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);
void DrawfillRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);
void PolygonTransformation(int16_t x0,int16_t y0,uint16_t r, uint16_t n);
void RotateXY(int *xy, int centerX, int centerY, int x, int y, int Angle, char direct);
void Linecube(int cx, int cy, int w, int h, double rot);
void DrawCube(int centerX, int centerY, int size, float rotX, float rotY, float rotZ);
void ClearnMemory(void *m, uint16_t size);
typedef struct LineChartMap
{
	const uint8_t width; 
	const uint8_t high;  
	const uint16_t max;  
	uint8_t Startp; 
	uint8_t Endp;   
	uint8_t RxNum;
	uint8_t dat[128];
}TypLineChartMap;
void ClearnLineChartMapDat(TypLineChartMap *t);
void AddDatToLineChartMap(TypLineChartMap *t, uint16_t d);
int16_t *LineChart(TypLineChartMap *t, int16_t x, int16_t y);
void PictureShow(menu_area *target, const uint8_t *psize, const uint8_t *p, int16_t x, int16_t y);


#endif







