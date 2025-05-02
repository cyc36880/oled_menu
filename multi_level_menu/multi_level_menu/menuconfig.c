#include "menuconfig.h"
#include "oledfont.h" //字体文件
#include "menufontshow.h" //字符函数

void test1_1(menu_area *target);
void test1_2(menu_area *target);
void test1_3(menu_area *target);
void test1_4(menu_area *target);
void test1_5(menu_area *target);

void test2_1(menu_area *target);
void test2_2(menu_area *target);
void test2_3(menu_area *target);
void test2_4(menu_area *target);
void test2_5(menu_area *target);

void test3_1(menu_area *target);

/*
	功能：初始化菜单列表 <此函数名不可修改>
*/
void MakeMenuHeard(void)
{
	menu_area *menuheard;
	menu_area *menuheard2;
	
	TargetMenu = AddToMenuList(8*5 + 4, 0, 8*4, 16, 1, NULL); //创建菜单头
	AddToMenuList(8*6, 16, 8*3, 16, 1, TargetMenu);
	AddToMenuList(8*6, 32, 8*3, 16, 1, TargetMenu);
	AddToMenuList(8*6, 0, 8*3, 16, 1, TargetMenu);
	AddToMenuList(8*6, 16, 8*3, 16, 1, TargetMenu);
	
	TargetMenu->menuinterface = test1_1; //链接对应函数
	MenuListAddressing(TargetMenu, 0, 1)->menuinterface = test1_2;
	MenuListAddressing(TargetMenu, 0, 2)->menuinterface = test1_3;
	MenuListAddressing(TargetMenu, 0, 3)->menuinterface = test1_4;
	MenuListAddressing(TargetMenu, 0, 4)->menuinterface = test1_5;
	
	MenuListAddressing(TargetMenu, 0, 2)->menulistend = 1; //列表尾部
	
	MakeMenuListRing(TargetMenu);//首尾相连
	
//
	menuheard = AddToMenuList(8*7, 0, 8*1, 16, 1, NULL); //创建菜单头
	AddToMenuList(8*7, 16, 8*1, 16, 1, menuheard);
	AddToMenuList(8*7, 32, 8*1, 16, 1, menuheard);
	AddToMenuList(127-9, 63-16, 8*1, 16, 1, menuheard);
	AddToMenuList(0, 0, 8*1, 16, 0, menuheard);
	
	menuheard->menuinterface = test2_1;//链接对应函数
	MenuListAddressing(menuheard, 0, 1)->menuinterface = test2_2;
	MenuListAddressing(menuheard, 0, 2)->menuinterface = test2_3;
	MenuListAddressing(menuheard, 0, 3)->menuinterface = test2_4;
	MenuListAddressing(menuheard, 0, 4)->menuinterface = test2_5;
	
	AddToMenuTimeList(MenuListAddressing(menuheard, 0, 1), 500); //定时执行
	AddToMenuTimeList(MenuListAddressing(menuheard, 0, 4), 100); //定时执行
	
	MakeMenuListRing(menuheard);//首尾相连
	
	LinkToParentClass(TargetMenu, menuheard);//链接到父类
	
	MenuListAddressing(menuheard, 0, 3)->subclass = menuheard->father; //把菜单头的父类链接为此菜单子类，用于跳转
	
//
	menuheard2 = AddToMenuList(8*7, 0, 8*3, 16, 1, NULL); //创建菜单头
	
	menuheard2->menuinterface = test3_1;//链接对应函数
	
	LinkToParentClass(menuheard->next, menuheard2);//链接到父类
	
	MenuListAddressing(menuheard2, 0, 0)->subclass = menuheard2->father; //把菜单头的父类链接为此菜单子类，用于跳转
}


/*
	功能：菜单显示的具体实现函数
	注意：函数必须是 void 类型，形参必须是 menu_area *target
*/

void test1_1(menu_area *target)
{
	MenuHzAndAsc(target, 0, 0, "滋生");
}

void test1_2(menu_area *target)
{
	MenuShowAscStr(target, F8X16, F8X16_SizeInf, 0, 0, "1_2");
}

void test1_3(menu_area *target)
{
	MenuShowAscStr(target, F8X16, F8X16_SizeInf, 0, 0, "1_3");
}
void test1_4(menu_area *target)
{
	MenuShowAscStr(target, F8X16, F8X16_SizeInf, 0, 0, "1_4");
}
void test1_5(menu_area *target)
{
	MenuShowAscStr(target, F8X16, F8X16_SizeInf, 0, 0, "1_5");
}
// ------
void test2_1(menu_area *target)
{
	MenuShowAscStr(target, F8X16, F8X16_SizeInf, 0, 0, "+");
}
void test2_2(menu_area *target)
{
	static unsigned char i=0;
	
	if(TargetMenu==NextCancheMenuList(target, -1) && StatusInformation==Menu_confirm)
		if(i < 9) i++;
	if(TargetMenu==NextCancheMenuList(target, 1) && StatusInformation==Menu_confirm)
		if(i>0) i--;
	
	if(target->userinformation == 1) { //分辨是否为时间列表函数调用
		target->userinformation = 0;
		if(++i > 9) i=0;
	}
	
	target->width = 8 + (i>4)*8;
	MenuShowNum(target, F8X16, F8X16_SizeInf, 8*0, 0, 1, i);
}
void test2_3(menu_area *target)
{
	MenuShowAscStr(target, F8X16, F8X16_SizeInf, 0, 0, "-");
}
void test2_4(menu_area *target)
{
	MenuShowAscStr(target, F8X16, F8X16_SizeInf, 0, 0, "<");
}

void test2_5(menu_area *target)
{
	static uint8_t i =0;
	
	if(target->userinformation == 1){
		target->userinformation = 0;
		if(++i > 9) i=0;
	}
	
	MenuShowNum(target, F8X16, F8X16_SizeInf, 8*0, 0, 1, i);
}

//-----
void test3_1(menu_area *target)
{
	MenuShowAscStr(target, F8X16, F8X16_SizeInf, 0, 0, "3_1");
	MenuShowAscStr(NULL, F8X16, F8X16_SizeInf, target->x - 8*2, target->y+8*3, "Nothing!!");
}



