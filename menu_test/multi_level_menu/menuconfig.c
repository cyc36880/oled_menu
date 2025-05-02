#include "menu.h"
#include "menuconfig.h"
#include "oledfont.h" //字体文件
#include "menufontshow.h" //字符函数
#include "Graphicalfunctions.h" // 图形化函数
#include "menu_tool.h" //工具


ProgressBarTypedef Bar; //进度条
MessageTypedef Mes; //消息

//所有界面刷新
FunctionTicker OverTimeTicker;
void OverTime(void);


menu_area menumain;
menu_area menutest;
menu_area menuArray[10];
menu_area menuTail;

menu_area menu2;

void test1_1(menu_area *target);
void test1_2(menu_area *target);
void MenuTail(menu_area *target);

/*
	功能：初始化菜单列表 <此函数名不可修改>
*/
static void MakeMenu(void)
{
	MessageInit(&Message_Key, &Mes);//消息
	ProgressBarInit(&Bar, 50, 14, 10, 0)->animation=ENABLE; //使能进度条动画

	TargetMenu = &menumain;
	TargetMenuPointrt.style = ENABLE; //使能指示器动画
	
	SetFunctionTicker(&OverTimeTicker, 100, normalRun, OverTime); //所有界面刷新
	
	SetMenu(&menumain, MSCX(32), MSCY(16), 32, 16, ENABLE, NULL)->menuinterface = test1_1;

	

	SetMenu(&menutest, 0, 0, 32, 16, ENABLE, NULL)->menuinterface = test1_1;
	BatchFastSimilarMenuDown(&menutest, menuArray, 10, 2, test1_2);
	FastSimilarMenuDown(&menutest, &menuTail, 2)->menuinterface = MenuTail;
	menuTail.checked = DISABLE;

	AddToSpecialFunction(&menuTail, EnterMenu, 0);

	LinkToParentClass(&menumain, &menutest);

	SetMenu(&menu2, 0, 0, 32, 16, ENABLE, NULL)->menuinterface = test1_1;
	LinkToParentClass(&menuArray[2], &menu2);

	MakeMenuListRing(&menutest);
} 



/*
	功能：菜单显示的具体实现函数
	注意：函数必须是 void 类型，形参必须且仅为 menu_area *target
*/


//所有界面刷新
void OverTime(void) 
{
	MenuRefresh(0);
}

void test1_1(menu_area *target)
{
	MenuShowNum(target, 0, 0, 0, target->id);
}
void test1_2(menu_area *target)
{
	MenuShowNum(target, 0, 0, 0, target->id);
}

void MenuTail(menu_area *target)
{
	if(TriggerCheck(target, EnterMenu))
	{
		MenuCoorAlignment(target);
		return;
	}
}






// ==================================================================
// ======================== 菜 单 固 定 函 数 =======================
// ==================================================================



//**************** while中全速运行  ************************


void AlwaysRun(void)
{
	
}



//*************** 菜单跟随刷新 *********************


// 菜单的每次刷新都会执行该函数 执行优先级最高
void MenuAlwaysRun_PH(void)
{
	
}
// 菜单的每次刷新都会执行该函数 执行优先级中
void MenuAlwaysRun_PM(void)
{
	ListSwitchIns(&Mes, 0, 63, 0); //滚动列表

}
// 菜单的每次刷新都会执行该函数 执行优先级最低
void MenuAlwaysRun_PL(void)
{
	static int8_t count = 0;
	if(++count > 20) count=0;
	MenuShowNum(NULL, 70, 0, 0, count);
	
	Bar.maxVal = NextCancheMenuList(FindMeunListHeard(TargetMenu), -1)->id; //进度条
	ProgressBar(&Bar, 30, 40, TargetMenuPointrt.TargetMenuP->id);
	
//	GRAPHICSSHOWMANNER = GraphicsRollColor; //图片快速绘制
//	FastDrawPic2(NULL, picture0, 10 - count, 13+count);
//	GRAPHICSSHOWMANNER = GraphicsNormal;
}


//************* 初始化菜单相关内容 *********************

void MenuInit(void)
{
	TargetMenuPointrt.style = DISABLE; //不使能指针动效
	TargetMenuPointrt.show = ENABLE;  //指针显示

	OLED_Init(); // OLED初始化
	MakeMenu(); //菜单列表初始化

	MenuHeartTimeStart = ENABLE;//菜单心跳
}








