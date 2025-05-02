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





//--------  折 线 图 ------------

//内存清零
void ClearnMemory(void *m, uint16_t size);


typedef struct LineChartMap
{
	const uint8_t width; //宽度 0-128
	const uint8_t high;  //高度 0-128
	const uint16_t max;  //取值范围
	uint8_t Startp; //起始显示位置 
	uint8_t Endp;   //结束显示位置
	uint8_t RxNum;
	uint8_t dat[128];
}TypLineChartMap;
//定义实例
//TypLineChartMap p = {width, high, max};

//折线图内存清零 <初始化内存>
void ClearnLineChartMapDat(TypLineChartMap *t);
//向折线图中填充数据
void AddDatToLineChartMap(TypLineChartMap *t, uint16_t d);
//功能：折线图绘制 @ret：最后一个数据在折线图中的显示坐标
int16_t *LineChart(TypLineChartMap *t, int16_t x, int16_t y);





// ---------------- 图 片 -------------------

//图片显示
//取模格式：阴码 列行式 逆向
void PictureShow(menu_area *target, const uint8_t *Size, const uint8_t *p, int16_t x, int16_t y);


#endif








