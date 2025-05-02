#include "menuconfig.h"
#include "oledfont.h" //字体文件
#include "menufontshow.h" //字符函数
#include "Graphicalfunctions.h" // 图形化函数

void test1_1(menu_area *target);
void test1_2(menu_area *target);
void test1_3(menu_area *target);


void test4(void);

/*
	功能：初始化菜单列表 <此函数名不可修改>
*/
void MakeMenuHeard(void)
{
	menu_area *p;
	
	TargetMenu = AddToMenuList(34, 0, 32, 16, 1, NULL);
	TargetMenu->menuinterface = test1_1;
	
	p = AddToMenuList(34, 16, 32, 16, 1, TargetMenu);
	p->menuinterface = test1_2;
	p->menulistend = 1;
	
	AddToMenuList(34, 0, 32, 16, 1, TargetMenu)->menuinterface = test1_3;
	
	MenuOverall(TargetMenu) -> menuinterface = test4; //列表全局
	
	MenuHeartTimeStart = 0;//菜单心跳
}


/*
	功能：菜单显示的具体实现函数
	注意：函数必须是 void 类型，形参必须且仅为 menu_area *target
*/

void test1_1(menu_area *target)
{
	MenuHzAndAsc(target, 0, 0, "1");
}
void test1_2(menu_area *target)
{
	MenuHzAndAsc(target, 0, 0, "2");
}
void test1_3(menu_area *target)
{
	MenuHzAndAsc(target, 0, 0, "3");
}
void test4(void)
{
//	MenuHzAndAsc(NULL, 0, 0, "5");
	MenuShowNum(NULL, F8X16,F8X16_SizeInf, 0, 0, 4, MenuSize);
}



