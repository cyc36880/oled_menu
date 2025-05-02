#include "Graphicalfunctions.h"
#include "math.h"

// ======================= 图 形 化 函 数 ========================

// 读点
unsigned char read_point(int16_t x, int16_t y)
{
	int px = x, py = (int)(y/8);
	unsigned char dat=0;
	
	if(x<0 || y<0) return 0;
	if(x>= SCREENWIDTH || y>=SCREENHIGH) return 0;
	
	dat = (DisplayBuff[px + py*SCREENWIDTH]>>(y%8)) & 0x01;
	
	return dat;
}

// 画点
void write_point(int16_t x, int16_t y, uint8_t w_d) 
{
	int px = x, py = (int)(y/8);
	
	if(x>=SCREENWIDTH-1 || y >= SCREENHIGH) return;
	if(x<0 || y<0) return;
	
	if(w_d)
		DisplayBuff[px + py*SCREENWIDTH] |= (0x01 << (y % 8));
	else
		DisplayBuff[px + py*SCREENWIDTH] &= ~(0x01 << (y % 8));
}

// 画线
void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{
		write_point(uRow,uCol, 1);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
// 空心圆
void DrawCircle(int16_t x0,int16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		write_point(x0+a,y0-b,1);             //5
 		write_point(x0+b,y0-a,1);             //0           
		write_point(x0+b,y0+a,1);             //4               
		write_point(x0+a,y0+b,1);             //6 
		write_point(x0-a,y0+b,1);             //1       
 		write_point(x0-b,y0+a,1);             
		write_point(x0-a,y0-b,1);             //2             
  		write_point(x0-b,y0-a,1);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 
//实心圆
void DrawCircle_Solid(int16_t x0,int16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		DrawLine(x0+a,y0-b,x0+a,y0+b);
		DrawLine(x0+b,y0-a,x0+b,y0+a);
		DrawLine(x0-a,y0-b,x0-a,y0+b);
		DrawLine(x0-b,y0-a,x0-b,y0+a); 	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
	}
}

// ----------------空 心 圆 角 矩 形------------
#define SWAP(x, y)       \
	    (y) = (x) + (y); \
	    (x) = (y) - (x); \
	    (y) = (y) - (x);
#define MAX(x,y)  		((x)>(y) ? (x):(y))
#define MIN(x,y)  		((x)<(y) ? (x):(y))

void DrawFastHLine(int x, int y, unsigned char w)
{
  int end = x+w;
	int a;
  for ( a = MAX(0,x); a < end; a++)
  {
    write_point(a,y,1);
  }
}

void DrawFastVLine(int x, int y, unsigned char h)
{
  	int end = y+h;
	int a;
  for (a = MAX(0,y); a < end; a++)
  {
    write_point(x,a,1);
  }
}
//实心圆角矩形
void DrawCircleHelper(int x0, int y0, unsigned char r, unsigned char cornername)
{
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;
  while (x<y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    if (cornername & 0x4)
    {
      write_point(x0 + x, y0 + y,1);
      write_point(x0 + y, y0 + x,1);
    }
    if (cornername & 0x2)
    {
      write_point(x0 + x, y0 - y,1);
      write_point(x0 + y, y0 - x,1);
    }
    if (cornername & 0x8)
    {
      write_point(x0 - y, y0 + x,1);
      write_point(x0 - x, y0 + y,1);
    }
    if (cornername & 0x1)
    {
      write_point(x0 - y, y0 - x,1);
      write_point(x0 - x, y0 - y,1);
    }
  }
}
void DrawRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r)
{
  // smarter version
  DrawFastHLine(x+r, y, w-2*r); // Top
  DrawFastHLine(x+r, y+h-1, w-2*r); // Bottom
  DrawFastVLine(x, y+r, h-2*r); // Left
  DrawFastVLine(x+w-1, y+r, h-2*r); // Right
  // draw four corners
  DrawCircleHelper(x+r, y+r, r, 1);
  DrawCircleHelper(x+w-r-1, y+r, r, 2);
  DrawCircleHelper(x+w-r-1, y+h-r-1, r, 4);
  DrawCircleHelper(x+r, y+h-r-1, r, 8);
}

void DrawFillCircleHelper(int x0, int y0, unsigned char r, unsigned char cornername, int delta)
{
  // used to do circles and roundrects!
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;
  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    if (cornername & 0x1)
    {
      DrawFastVLine(x0+x, y0-y, 2*y+1+delta);
      DrawFastVLine(x0+y, y0-x, 2*x+1+delta);
    }

    if (cornername & 0x2)
    {
      DrawFastVLine(x0-x, y0-y, 2*y+1+delta);
      DrawFastVLine(x0-y, y0-x, 2*x+1+delta);
    }
  }
}
void DrawFillRect2(unsigned int x,unsigned int y,unsigned int w,unsigned char h)
{
	unsigned int i,j;
	for(j=y;j<=y+h;j++){
		for(i=x;i<=x+w;i++){
			write_point(i,j,1);
		}
	}
}
void DrawfillRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r)
{
  DrawFillRect2(x+r, y, w-2*r, h);

  // draw four corners
  DrawFillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1);
  DrawFillCircleHelper(x+r, y+r, r, 2, h-2*r-1);
}

//多边形变换
void PolygonTransformation(int16_t x0,int16_t y0,uint16_t r, uint16_t n)
{
	unsigned char i =0,j;
	int x[30],y[30];
		
	for(i=0;i<n;i++)
	{
		x[i]=r*cos(2*3.1415926*i/n)+x0;
		y[i]=r*sin(2*3.1415926*i/n)+y0;
	}
	for(i=0;i<=n-2;i++)
	{
		for(j=i+1;j<=n-1;j++)
			DrawLine(x[i], y[i], x[j], y[j]);
	}
}

#define RADIAN(angle)  ((angle==0)?0:(3.1415926535*angle/180)) // ----------

/*
	centerX : 中心点x坐标
	centerY ：中心点y坐标
	x       ：目标点x坐标
	y       ：目标点y坐标
	angle   ：旋转角度
	direct  ：正反转 1顺时针 0逆时针 
	返回值  ：unsigned int 指针 
*/
void RotateXY(int *xy, int centerX, int centerY,int x, int y,int Angle,int direct) 
{
	double angle = RADIAN(Angle);
	int temp=(y-centerY)*(y-centerY)+(x-centerX)*(x-centerX);
	double r=sqrt(temp);
	double a0=atan2(y-centerY,x-centerX);
	double d=0;
	
	if(Angle==0)
	{
		xy[0] = x;
		xy[1] = y;
		return;
	}
	if(direct)
	{
		d = centerX+r*cos(a0+angle);
		if(((unsigned int)(d*10) )% 10 > 5) xy[0]= (unsigned int)d + 1;
		else                                xy[0]= (unsigned int)d;
		
		d = centerY+r*sin(a0+angle);
		if(((unsigned int)(d*10)) % 10 > 5) xy[1]= (unsigned int)d + 1;
		else                                xy[1]= (unsigned int)d;
	}
	else
	{
		d = centerX+r*cos(a0-angle);
		if(((unsigned int)(d*10) )% 10 > 5) xy[0]= (unsigned int)d + 1;
		else                                xy[0]= (unsigned int)d;
		 
		d = centerY+r*sin(a0-angle);
		if(((unsigned int)(d*10) )% 10 > 5) xy[1]= (unsigned int)d + 1;
		else                                xy[1]= (unsigned int)d;
	}
}

