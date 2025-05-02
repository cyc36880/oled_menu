#include "menuconfig.h"
#include "oledfont.h" //字体文件
#include "menufontshow.h" //字符函数
#include "Graphicalfunctions.h" // 图形化函数

void test1_1(menu_area *target);

void test2_1(menu_area *target);
void test2_2(menu_area *target);
void test2_3(menu_area *target);
void test2_4(menu_area *target);
void test2_5(menu_area *target);

void test3_1(menu_area *target);

extern uint32_t adcbuf[];

/*
	功能：初始化菜单列表 <此函数名不可修改>
*/
void MakeMenuHeard(void)
{
	menu_area *menuheard;
	menu_area *menuheard2;
	
	TargetMenu = AddToMenuList(8*5 + 10, 0, 8*4, 16, 1, NULL); //创建菜单头	
	TargetMenu->menuinterface = test1_1; //链接对应函数	
//	MakeMenuListRing(TargetMenu);//首尾相连
	
	AddToSpecialFunction(TargetMenu, MenuTime, 10);
	
	menuheard = AddToMenuList(8*7, 0, 8*1, 16, 1, NULL); //创建菜单头
									   menuheard		->menuinterface = test2_1;
	AddToMenuList(8*7, 16, 8*1, 16, 1, menuheard)		->menuinterface = test2_2;
	AddToMenuList(8*7, 32, 8*1, 16, 1, menuheard)		->menuinterface = test2_3;
	AddToMenuList(127-9, 63-16, 8*1, 16, 1, menuheard)	->menuinterface = test2_4;;
	AddToMenuList(0, 0, 8*1, 16, 0, menuheard)			->menuinterface = test2_5;
	
	AddToSpecialFunction(MenuListAddressing(menuheard, 0, 1),  MenuTime | ExitShowMenuList, 500); //定时执行+退出执行
	AddToSpecialFunction(MenuListAddressing(menuheard, 0, 4),  MenuTime | EnterShowMenuList | MenuTimeForce, 100); //定时执行
	
	MakeMenuListRing(menuheard);//首尾相连
	
	LinkToParentClass(TargetMenu, menuheard);//链接到父类
	
	MenuListAddressing(menuheard, 0, 3)->subclass = menuheard->father; //把菜单头的父类链接为此菜单子类，用于跳转
	
//
	menuheard2 = AddToMenuList(8*7, 0, 8*3, 16, 1, NULL); //创建菜单头
	menuheard2->menuinterface = test3_1;//链接对应函数
	
	LinkToParentClass(menuheard->next, menuheard2);//链接到父类
	
	MenuListAddressing(menuheard2, 0, 0)->subclass = menuheard2->father; //把菜单头的父类链接为此菜单子类，用于跳转
	
	MenuHeartTimeStart = 1; //菜单心跳开始
}


/*
	功能：菜单显示的具体实现函数
	注意：函数必须是 void 类型，形参必须是 menu_area *target
*/

void test1_1(menu_area *target)
{
	static unsigned int i=0;
	static unsigned char flag=0;
	int xy[2] = {0, 0};	
	
	if(TriggerCheck(target, MenuTime)){
		
		if(++i >= 720) i=0;
		if(i%10 == 0) flag = !flag;
		
	}
	
	RotateXY(xy, 64, 32, 80, 32, i, 1);
	
	MenuHzAndAsc(target, 0, 0, "滋生");
	
	DrawCircle_Solid(xy[0], xy[1], 4);
	DrawCircle_Solid(64, 32, flag?i%10:10-i%10);
	DrawCircle(64, 32, 16);	

}


// ------
void test2_1(menu_area *target)
{
	MenuShowAscStr(target, F8X16, F8X16_SizeInf, 0, 0, "+");
}
void test2_2(menu_area *target)
{
	static unsigned char i=0;
	
	if(TriggerCheck(target, ExitShowMenuList))
	{
		i=0;
	}
	if(TriggerCheck(target, MenuTime) ) { //分辨是否为时间列表函数调用
		if(++i > 9) i=0;
	}
	
	if(TargetMenu==NextCancheMenuList(target, -1) && StatusInformation==Menu_Sub)
		if(i < 9) i++;
	if(TargetMenu==NextCancheMenuList(target, 1) && StatusInformation==Menu_Sub)
		if(i>0) i--;
	
	target->width = 8 + (i>4)*8; //为保证图像正常显示，修改属性应在所有图像操作函数的上方
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
	
	if(TriggerCheck(target, MenuTime)){
		if(++i >= 100) i=0;
		return;
	}
	if(TriggerCheck(target, EnterShowMenuList)){
		i=0;
	}
	
	MenuShowNum(NULL, F8X16, F8X16_SizeInf, 8*0, 0,  3, i);
}

//-----
void test3_1(menu_area *target)
{
	MenuShowAscStr(target, F8X16, F8X16_SizeInf, 0, 0, "3_1");
	MenuShowAscStr(NULL, F8X16, F8X16_SizeInf, target->x - 8*2, target->y+8*3, "Nothing!!");
}
