#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <graphics.h>
#include <windows.h>
#include <math.h>
#include <time.h>

#define int16_t short int   
#define uint16_t unsigned short int

const unsigned int BUFFWEIGH = 600;//屏幕尺寸
const unsigned int BUFFHIGH  = 300;  

// ============== 图 形 ================


void LCD_DrawPoint(int x, int y)
{
	putpixel(x,y,YELLOW);
}
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
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
		LCD_DrawPoint(uRow,uCol);//画点 
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

void DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_DrawLine(x1, y1, x2, y1);
	LCD_DrawLine(x1, y1, x1, y2);
	LCD_DrawLine(x2, y2, x2, y1);
	LCD_DrawLine(x2, y2, x1, y2);
}

// ============== 菜 单 ================= 
typedef struct MENU_AREA
{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t high;
	uint16_t id;
	bool checked; //能否被选中 
	struct MENU_AREA *next;     //下一个
	struct MENU_AREA *previous; //上一个
	struct MENU_AREA *subclass; //子类
	struct MENU_AREA *father;   //父类
}menu_area;


/*
*	功能：注册或添加菜单 
*	x, y, width, high: 为注册菜单单元的位置与大小 
*	checked: 能否被选中，1能 0否 
*	transfer：为 NULL 注册的是菜单头，填入菜单地址为其尾加入 
*/
menu_area * AddToMenuList(uint16_t x, uint16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer)
{
	menu_area *p;
	menu_area *k;
	
	p = (menu_area *) malloc(sizeof(menu_area));
	
	if(transfer != NULL)  //尾部加入 
	{
		k = transfer;
		
		for( ; k->next!=NULL; )
		{
			k = k->next;
		}
		k->next = p;
		p->previous = k;
		p->id = k->id + 1; 
	}
	else
	{
		p->id = 1; // 菜单头ID 
		p->previous = NULL; //上一个
	}
	
	p->x = x>=BUFFWEIGH ? BUFFWEIGH-1 : x;
	p->y = y>=BUFFHIGH ? BUFFHIGH-1 : y;
	p->width = width+p->x>BUFFWEIGH ? BUFFWEIGH-p->x : width;
	p->high = high+p->y>BUFFHIGH ? BUFFHIGH-p->y : high;
	p->checked =  checked; // 能否选中 
	p->next = NULL;      //下一个
	p->subclass = NULL;  //子类 
	p->father = NULL;    // 父类 
	return p;
}


/**
 *  功能：链接到父类
 *  target：目标地址
 *  source：源地址 
 * 
 **/
void LinkToParentClass(menu_area *target, menu_area *source)
{
	target->subclass = source;
	source->father = target;
}


/**
 * 功能：对菜单向下偏移寻址 
 * target：目标寻址菜单 
 * offset：偏移步数 
 * upordown: 0向下偏移 1向上偏移 
 * 返回值：相对与源地址偏移后的地址，offset=0返回源地址，无则返回NULL 
 **/
menu_area * MenuListAddressing(menu_area *target, bool upordown, uint16_t offset)
{
	menu_area *p = target;
	uint16_t i;
	for(i=0; i<offset; i++)
	{
		if(upordown == 0) 
		{
			if(p->next == NULL){
				return NULL;
			}
			p = p->next;
		}
		else
		{
			if(p->previous == NULL){
				return NULL;
			}
			p = p->previous;
		}
	}
	return p;
}


/**
 * 功能：对菜单头的向后偏移寻址 
 * target：目标寻址菜单 
 * offset：偏移步数 
 * upordown: 0向下偏移 1向上偏移 
 * 返回值：相对与源地址偏移后的地址，offset=0返回源地址，无则返回NULL 
 **/
menu_area * MenuHeadAddressing(menu_area *target,  bool upordown, uint16_t offset)
{
	menu_area *p = target;
	uint16_t i;
	for(i=0; i<offset; i++)
	{
		if(upordown == 0) 
		{
			if(p->subclass == NULL){
				return NULL;
			}
			p = p->subclass;
		}
		else
		{
			if(p->father == NULL){
				return NULL;
			}
			p = p->father;
		}
	}
	return p;
}

/*
	功能：寻找以目标单元为基准的第num个菜单可选中的菜单，无则返回原地址
	target: 基地址
	num：正值向下寻找，负值向上寻找
	返回值：寻址后的地址
*/
menu_area *NextCancheMenuList(menu_area *target, int16_t num)
{
	menu_area *p = target;
	uint16_t j = num<0?-num:num;
	uint16_t i;

	if(p==NULL) return NULL;
	for(i=0; i<j;)
	{
		p=MenuListAddressing(p, num<0, 1);
		if(p==NULL) return target;
		if(p->checked == 1) i++;
	}
	
	return p;
}


/*
	功能：寻找以目标单元为基准的第num个菜单可选中的菜单头，无则返回原地址
	target: 基地址
	num：正值向右寻找，负值向左寻找
	返回值：寻址后的地址
*/
menu_area *NextCancheMenuHeard(menu_area *target, int16_t num)
{
	menu_area *p = target;
	uint16_t j = num<0?-num:num;
	uint16_t i;

	if(p==NULL) return NULL;
	for(i=0; i<j;)
	{
		p=MenuHeadAddressing(p, num<0, 1);
		if(p==NULL) return target;
		if(p->checked == 1) i++;
	}
	
	return p;	
}

/*
	功能：找到菜单所在菜单列表的菜单头，空指针或超过最大列表数返回NULL 
	target：菜单指针
*/
menu_area *FindMeunListHeard(menu_area *target) 
{
	menu_area *heard=target;
	
	if(target == NULL) return NULL;
	for( ; ; )
	{
		if(heard->id == 1) break; // 菜单列表头id 
		heard = heard->previous;
	}
	return heard;
}

/*
	功能：找到菜单所在菜单列表的菜单尾，空指针或超过最大列表数返回NULL 
	target：菜单指针
*/
menu_area *FindMeunListTail(menu_area *target) 
{
	menu_area *tail=target;
	
	if(target == NULL) return NULL;
	
	for( ; ; )
	{
		if(tail->next == NULL) break;
		if(tail->next->id == 1) break;
		tail = tail->next;
	}
	return tail;
}


/*
	功能： 目标菜单首位相连，地址为空跳过 
	target：所在菜单列表的任一菜单 
*/
void MakeMenuListRing(menu_area *target)
{
	menu_area *heard, *tail;
	
	if(target == NULL) return;
	
	heard = FindMeunListHeard(target);
	tail = FindMeunListTail(target);
	
	tail->next = heard;
	heard->previous = tail;
}


int main()
{
	char Scan = NULL;
	
	menu_area *MainMenu = NULL;
	menu_area *MainMenu02 = NULL;
	
	menu_area *p = NULL;
	
	MainMenu = AddToMenuList(0, 50, 100, 50, 1, NULL);
	AddToMenuList(0, 100, 110, 50, 1, MainMenu);
	
	MainMenu02 = AddToMenuList(30, 50, 100, 50, 1, NULL);
	LinkToParentClass(MainMenu, MainMenu02);
	AddToMenuList(30, 100, 120, 50, 1, MainMenu02);
	AddToMenuList(30, 150, 130, 50, 1, MainMenu02);

	MakeMenuListRing(MainMenu);
	MakeMenuListRing(MainMenu); //测试是否会死循环 
	MakeMenuListRing(MainMenu02);
	
	p = MainMenu;
	
	initgraph(600,300);
	
	while(1)
	{
		DrawRectangle(p->x, p->y, p->x+p->width, p->y+p->high);
		scanf("%c", &Scan);
		cleardevice();
		
		
		if(Scan == 'a')
		{
			p = NextCancheMenuHeard(p, -1);
		}
		if(Scan == 'd')
		{
			p = NextCancheMenuHeard(p, 1);
		}
		if(Scan == 's')
		{
			p = NextCancheMenuList(p,1);
		}
		
		if(Scan == 'w')
		{
			p = NextCancheMenuList(p,-1);
		}
		
		Scan = NULL;
	}
}
