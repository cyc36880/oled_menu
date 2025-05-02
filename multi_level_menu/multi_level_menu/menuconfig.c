#include "menuconfig.h"
#include "menu.h"
#include "oledfont.h" //字体文件
#include "menufontshow.h" //字符函数
#include "Graphicalfunctions.h" // 图形化函数


void test1_1(menu_area *target);

void alwayst(void);
void alwayst1(void);

menu_area menutest0;

menu_area menutest1;
menu_area menutest2;
menu_area menutest3;
menu_area menutest4;
menu_area menutest5;
menu_area menutest6;


menu_area menutest7;
menu_area menutest8;
menu_area menutest9;
menu_area menutest10;
menu_area menutest11;
menu_area menutest12;


/*
	功能：初始化菜单列表 <此函数名不可修改>
*/
static void MakeMenu(void)
{
	TargetMenu = SetMenu(&menutest0, MenuScreenCenterX(32), 0, 32, 16, 1, NULL); //使用以创建好的菜单配置
	menutest0.menuinterface = test1_1;
	
	SetMenu(&menutest1, MenuScreenCenterX(32), 0, 32, 16, 1, NULL)->menuinterface = test1_1;
	LinkToParentClass(&menutest0, &menutest1);
	
	FastSimilarMenu(&menutest1, &menutest2, 2)->menuinterface = test1_1;
	FastSimilarMenu(&menutest1, &menutest3, 2)->menuinterface = test1_1;
		SetMenu(&menutest7, MenuScreenCenterX(32), 0, 32, 16, 1, NULL)->menuinterface = test1_1;
		LinkToParentClass(&menutest3, &menutest7);
		FastSimilarMenu(&menutest7, &menutest8, 2)->menuinterface = test1_1;
		FastSimilarMenu(&menutest7, &menutest9, 2)->menuinterface = test1_1;
		FastSimilarMenu(&menutest7, &menutest10, 2)->menuinterface = test1_1;
		FastSimilarMenu(&menutest7, &menutest11, 2)->menuinterface = test1_1;
		FastSimilarMenu(&menutest7, &menutest12, 2)->menuinterface = test1_1;
		MenuOverall(&menutest7)->menuinterface = alwayst1;
	FastSimilarMenu(&menutest1, &menutest4, 2)->menuinterface = test1_1;
	FastSimilarMenu(&menutest1, &menutest5, 2)->menuinterface = test1_1;
	FastSimilarMenu(&menutest1, &menutest6, 2)->menuinterface = test1_1;
	
	MenuOverall(&menutest1)->menuinterface = alwayst;
	MakeMenuListRing(&menutest1);
	
	
	
	
	MenuHeartTimeStart = ENABLE;//菜单心跳
}

/*
	功能：菜单显示的具体实现函数
	注意：函数必须是 void 类型，形参必须且仅为 menu_area *target
*/
void test1_1(menu_area *target)
{
	MenuShowNum(target, 0, 0, 2, target->id);
}

void alwayst(void)
{
	ScrollingDisplay_Y(TargetMenu, 0, 48, 0, 48);
}
void alwayst1(void)
{
	ScrollingDisplay_Y(TargetMenu, 0, 64, 16, 32);
}






//功能：初始化菜单相关内容
void MenuInit(void)
{
	OLED_Init(); // OLED初始化
	MakeMenu(); //菜单列表初始化
}




