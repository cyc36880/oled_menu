#include "Graphicalfunctions.h"
#include "math.h"

unsigned char ReadPoint(int16_t x, int16_t y)
{
	int px = x, py = (int)(y/8);
	unsigned char dat=0;
	
	if(x<0 || y<0) return 0;
	if(x>= SCREENWIDTH-1 || y>=SCREENHIGH) return 0;
	
	dat = (DisplayBuff[px + py*SCREENWIDTH]>>(y%8)) & 0x01;
	
	return dat;
}
void WritePoint(int16_t x, int16_t y, uint8_t w_d) 
{
	int px = x, py = (int)(y/8);
	
	if(x>=SCREENWIDTH-1 || y >= SCREENHIGH) return;
	if(x<0 || y<0) return;
	
	if(w_d)
		DisplayBuff[px + py*SCREENWIDTH] |= (0x01 << (y % 8));
	else
		DisplayBuff[px + py*SCREENWIDTH] &= ~(0x01 << (y % 8));
}
unsigned char read_point(int16_t x, int16_t y)
{
	unsigned char point = 0;
	
	point = ReadPoint(x, y);
	return point;
}
void write_point(int16_t x, int16_t y, uint8_t w_d) 
{
	if(GRAPHICSSHOWMANNER == GraphicsRollColor) { 
		WritePoint(x,  y,  !read_point(x, y));
		return;
	}
	WritePoint(x,  y,  w_d);
}
static void FastLineT(int16_t x, int16_t y, uint16_t w)
{
	while(w) {
		write_point(x+w-1, y, 1);
		w--;
	}
}
static void FastLineL(int16_t x, int16_t y, uint16_t h)
{
	while(h){
		write_point(x, y+h-1, 1);
		h--;
	}
}
void DrawRect(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	if(h==0 || w==0) return;
	FastLineT(x, y, w);
	FastLineT(x, y+h-1, w);
	if(h<3) return;
	FastLineL(x, y+1, h-2);
	FastLineL(x+w-1, y+1, h-2);
}
void DrawFillRect(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	while(h) {
		FastLineT(x, y+h-1, w);
		h--;
	}
}
void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1;
	else if(delta_x==0)incx=0;
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )
	{
		write_point(uRow,uCol, 1);
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
void DrawCircle(int16_t x0,int16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             
	while(a<=b)
	{
		write_point(x0+a,y0-b,1);             
 		write_point(x0+b,y0-a,1);                      
		write_point(x0+b,y0+a,1);                          
		write_point(x0+a,y0+b,1);             
		write_point(x0-a,y0+b,1);                  
 		write_point(x0-b,y0+a,1);             
		write_point(x0-a,y0-b,1);                        
  		write_point(x0-b,y0-a,1);              	         
		a++;

		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 
void DrawCircle_Solid(int16_t x0,int16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);            
	while(a<=b)
	{
		DrawLine(x0+a,y0-b,x0+a,y0+b);
		DrawLine(x0+b,y0-a,x0+b,y0+a);
		DrawLine(x0-a,y0-b,x0-a,y0+b);
		DrawLine(x0-b,y0-a,x0-b,y0+a); 	         
		a++;
 
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
	}
}
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
      if(x<y) write_point(x0 + y, y0 + x,1);
    }
    if (cornername & 0x2)
    {
      write_point(x0 + x, y0 - y,1);
      if(x<y) write_point(x0 + y, y0 - x,1);
    }
    if (cornername & 0x8)
    {
      write_point(x0 - y, y0 + x,1);
      if(x<y) write_point(x0 - x, y0 + y,1);
    }
    if (cornername & 0x1)
    {
      write_point(x0 - y, y0 - x,1);
      if(x<y) write_point(x0 - x, y0 - y,1);
    }
  }
}
void DrawRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r)
{

  DrawFastHLine(x+r, y, w-2*r); 
  DrawFastHLine(x+r, y+h-1, w-2*r); 
  DrawFastVLine(x, y+r, h-2*r); 
  DrawFastVLine(x+w-1, y+r, h-2*r); 

  DrawCircleHelper(x+r, y+r, r, 1);
  DrawCircleHelper(x+w-r-1, y+r, r, 2);
  DrawCircleHelper(x+w-r-1, y+h-r-1, r, 4);
  DrawCircleHelper(x+r, y+h-r-1, r, 8);
}
void DrawFillCircleHelper(int x0, int y0, unsigned char r, unsigned char cornername, int delta)
{

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
      if(x<y) DrawFastVLine(x0+y, y0-x, 2*x+1+delta);
    }

    if (cornername & 0x2)
    {
      DrawFastVLine(x0-x, y0-y, 2*y+1+delta);
      if(x<y) DrawFastVLine(x0-y, y0-x, 2*x+1+delta);
    }
  }
}
void DrawFillRect2(int x,int y,unsigned int w,unsigned char h)
{
	int i,j;
	for(j=y;j<=y+h;j++){
		for(i=x;i<=x+w;i++){
			write_point(i,j,1);
		}
	}
}
void DrawfillRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r)
{
  DrawFillRect2(x+r, y, w-2*r-1, h);


  DrawFillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1);
  DrawFillCircleHelper(x+r, y+r, r, 2, h-2*r-1);
}
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

void RotateXY(int *xy, int centerX, int centerY,int x, int y,int Angle, char direct) 
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
void ClearnMemory(void *m, uint16_t size)
{
	uint8_t *p = (uint8_t *) m;
	while(size--) {
		*p = 0;
		p++;
	}
}
void ClearnLineChartMapDat(TypLineChartMap *t)
{
	t->Startp = 0;
	t->Endp = 0;
	t->RxNum = 0;
	ClearnMemory(t->dat, sizeof(t->dat));
}
void AddDatToLineChartMap(TypLineChartMap *t, uint16_t d)
{
	uint8_t width = t->width<128?t->width:128; 
	uint8_t high = t->high<128?t->high:128; 
	
	if(++t->RxNum > 129) { 
		t->RxNum = 129;
	}
	if(d > t->max ) d = t->max; 
	d = high * 1.0 / t->max * d;
	
	t->dat[t->Endp] = d; 
	
	if(t->RxNum > width) { 
		if(++t->Startp >= 128) { 
			t->Startp = 0;
		}
	}
	if(++t->Endp >= 128) {
		t->Endp = 0;
	}
}
int16_t *LineChart(TypLineChartMap *t, int16_t x, int16_t y) 
{
	static int16_t xy[2] = {0, 0};
	int16_t posy = 0;   
	int16_t posy_2 = 0; 
	uint8_t Startp = 0; 
	uint8_t Startp_1 = 0;
	
	uint8_t offset=0; 
	
	xy[0] = 0; 
	xy[1] = 0;
	
	if(t->RxNum == 0) {
		
	}
	else if(t->RxNum == 1) { 
		
		posy = y + t->high - t->dat[0];
		write_point(x, posy, 1);
		
		xy[0] = x;
		xy[1] = posy;
	}
	else if(t->RxNum == 2) { 
		posy = y + t->high - t->dat[0];   
		posy_2 = y + t->high - t->dat[1];
		DrawLine(x, posy, x+1, posy_2);
		
		xy[0] = x + 1;
		xy[1] = posy_2;
	}
	else { 
		Startp = t->Startp;
		Startp_1 = Startp + 1 >= 128?0:Startp + 1;
		
		while(Startp_1 != t->Endp) {
			posy = y + t->high - t->dat[Startp]; 
			posy_2 = y + t->high - t->dat[Startp_1];
			
			DrawLine(x+offset, posy, x+offset+1, posy_2);
			
			Startp = Startp + 1 >= 128?0:Startp + 1;
			Startp_1 = Startp + 1 >= 128?0:Startp + 1;
			
			offset++;
		}
		xy[0] = x+offset-1;
		xy[1] = posy_2;
	}
	return xy;
}
void PictureShow(menu_area *target, const uint8_t *psize, const uint8_t *p, int16_t x, int16_t y)
{
	uint8_t h,w;
	bool w_b=0;
	uint8_t wight;
	uint8_t high;
	
	if(psize == NULL) {
		wight = p[0];
		high  = p[1];
		p += 2;
	}
	else {
		wight = psize[0];
		high  = psize[1];
	}
	for(h=0; h<high; h++)
	{
		for(w=0; w<wight; w++) 
		{
			w_b = ( p[ h/8*wight + w] >> (h%8) ) & 1;
			if(w_b) {
				if(target == NULL) write_point(w+x, h+y, 1);
				else               MenuSetPoint(target, w+x, h+y, 1);
			}
			else {
				if(GRAPHICSSHOWMANNER == GraphicsCover) {
					if(target == NULL) write_point(w+x, h+y, 0);
					else               MenuSetPoint(target, w+x, h+y, 0);
				}
			}
		}
	}
}
#define PI 3.1415926
#define SX 4
#define SY 6
#define DX PI / SX
#define DY PI * 2 / SY
#define X(a, b) (cx + v[a][b].x * r), (cy + v[a][b].y * r)
typedef struct { 
	double x, y;
} Vec;
static Vec v[SX + 1][SY + 1];

static void calc(double i, double j, double rot, Vec* v) {
    double x = sin(i) * cos(j), y = sin(i) * sin(j), z = cos(i),
        s = sin(rot), c = cos(rot), c1 = 1 - c, u = 1 / sqrt(3), u2 = u * u;
    v->x = x * (c + u2 * c1) + y * (u2 * c1 - u * s) + z * (u2 * c1 + u * s);
    v->y = x * (u2 * c1 + u * s) + y * (c + u2 * c1) + z * (u2 * c1 - u * s);
}
void Linecube(int cx, int cy, int w, int h, double rot)
{
	int  r = h * 0.375;
	for(int i = 0; i <= SX; ++i) for(int j = 0; j <= SY; ++j) 
			calc(i * DX, j * DY, rot, &v[i][j]);
			
	for(int i = 0; i < SX; ++i) for(int j = 0; j < SY; ++j) {
		DrawLine(X(i, j), X(i + 1, j));
		DrawLine(X(i, j), X(i, j + 1));
	}
}
void DrawCube(int centerX, int centerY, int size, float rotX, float rotY, float rotZ)
{
    float halfSize = size / 2.0f;
    float vertices[8][3] = {
        {-halfSize, -halfSize, -halfSize},
        {halfSize, -halfSize, -halfSize},
        {halfSize, halfSize, -halfSize},
        {-halfSize, halfSize, -halfSize},
        {-halfSize, -halfSize, halfSize},
        {halfSize, -halfSize, halfSize},
        {halfSize, halfSize, halfSize},
        {-halfSize, halfSize, halfSize}
    };

  
    for (int i = 0; i < 8; i++) {
        float y = vertices[i][1];
        float z = vertices[i][2];
        vertices[i][1] = y * cos(rotX) + z * sin(rotX);
        vertices[i][2] = -y * sin(rotX) + z * cos(rotX);
    }

   
    for (int i = 0; i < 8; i++) {
        float x = vertices[i][0];
        float z = vertices[i][2];
        vertices[i][0] = x * cos(rotY) + z * sin(rotY);
        vertices[i][2] = -x * sin(rotY) + z * cos(rotY);
    }

    
    for (int i = 0; i < 8; i++) {
        float x = vertices[i][0];
        float y = vertices[i][1];
        vertices[i][0] = x * cos(rotZ) - y * sin(rotZ);
        vertices[i][1] = x * sin(rotZ) + y * cos(rotZ);
    }
    DrawLine( centerX + vertices[0][0], centerY + vertices[0][1], centerX + vertices[1][0], centerY + vertices[1][1]);
    DrawLine(centerX + vertices[1][0], centerY + vertices[1][1], centerX + vertices[2][0], centerY + vertices[2][1]);
    DrawLine( centerX + vertices[2][0], centerY + vertices[2][1], centerX + vertices[3][0], centerY + vertices[3][1]);
    DrawLine(centerX + vertices[3][0], centerY + vertices[3][1], centerX + vertices[0][0], centerY + vertices[0][1]);
    DrawLine( centerX + vertices[4][0], centerY + vertices[4][1], centerX + vertices[5][0], centerY + vertices[5][1]);
    DrawLine( centerX + vertices[5][0], centerY + vertices[5][1], centerX + vertices[6][0], centerY + vertices[6][1]);
    DrawLine( centerX + vertices[6][0], centerY + vertices[6][1], centerX + vertices[7][0], centerY + vertices[7][1]);
		DrawLine(centerX + vertices[7][0], centerY + vertices[7][1], centerX + vertices[4][0], centerY + vertices[4][1]);
		DrawLine( centerX + vertices[0][0], centerY + vertices[0][1], centerX + vertices[4][0], centerY + vertices[4][1]);
		DrawLine(centerX + vertices[1][0], centerY + vertices[1][1], centerX + vertices[5][0], centerY + vertices[5][1]);
		DrawLine( centerX + vertices[2][0], centerY + vertices[2][1], centerX + vertices[6][0], centerY + vertices[6][1]);
		DrawLine(centerX + vertices[3][0], centerY + vertices[3][1], centerX + vertices[7][0], centerY + vertices[7][1]);
}
