#include "menuconfig.h"
#include "oledfont.h" //字体文件
#include "menufontshow.h" //字符函数
#include "Graphicalfunctions.h" // 图形化函数

void test1_1(menu_area *target);
void test1_2(menu_area *target);
void test1_3(menu_area *target);


void test4(void);

uint8_t count=0;

/*
	功能：初始化菜单列表 <此函数名不可修改>
*/
void MakeMenu(void)
{
	menu_area *p;
	menu_area *testmenu;
	
	TargetMenu = AddToMenuList(34, 0, 32, 16, 1, NULL);
	TargetMenu->menuinterface = test1_1;
	AddToSpecialFunction(TargetMenu, ExitShowMenuList, 500);
	
	p = AddToMenuList(34, 16, 32, 16, 1, TargetMenu);
	p->menuinterface = test1_2;
	p->menulistend = 1;
	AddToSpecialFunction(p, EnterShowMenuList, 500);
	
	p = AddToMenuList(34, 0, 32, 16, 1, TargetMenu);
	p ->menuinterface = test1_3;
	
	testmenu = AddToMenuList(34, 0, 32, 16, 1, NULL);
	LinkToParentClass(p, testmenu);
	
	MenuOverall(TargetMenu) -> menuinterface = test4; //列表全局
	
	MenuHeartTimeStart = 1;//菜单心跳
}


/*
	功能：菜单显示的具体实现函数
	注意：函数必须是 void 类型，形参必须且仅为 menu_area *target
*/

void test1_1(menu_area *target)
{
	SetFont(F8X16, F8X16_SizeInf);//设置字体
	
	if(TriggerCheck(target, ExitShowMenuList)) {
//		count ++;
		return;
	}
	MenuShowNum(target, 0, 0, 3, 123);
}
void test1_2(menu_area *target)
{
	SetFont(F8X16, F8X16_SizeInf);//设置字体
	
	if(TriggerCheck(target, EnterShowMenuList)) {
		count++;
		return;
	}
	MenuShowNum(target, 0, 0, 3, 456);
}
void test1_3(menu_area *target)
{
	SetFont(F8X16, F8X16_SizeInf);//设置字体
	
	MenuHzAndAsc(target, 0, 0, "3");
}
void test4(void)
{
	SetFont(F6X8, F6X8_SizeInf);//设置字体
	MenuShowAscStr(NULL, 0, 32, "asd123~{|}");
//	SetFont(F8X16, F8X16_SizeInf);//设置字体
//	MenuShowAscStr(NULL, 0, 42, "asd123~{|}");
	MenuHzAndAsc(NULL, 0, 42, "3滋生1");
	
	MenuShowNum(NULL, 0, 0, 3, count);
}



