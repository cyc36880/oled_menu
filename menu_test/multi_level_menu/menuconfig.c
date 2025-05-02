#include "menuconfig.h"

#include "menu.h"
#include "oledfont.h" //字体文件
#include "menufontshow.h" //字符函数
#include "Graphicalfunctions.h" // 图形化函数
#include "menu_tool.h" //工具


ProgressBarTypedef Bar; //进度条
MessageTypedef Mes;  //消息-滚动列表

menu_area menumain;
menu_area menutest;
menu_area menuArray[10];
menu_area menuTail;

menu_area menu2;

void test1_1(menu_area *target);
void test1_2(menu_area *target);
void MenuTail(menu_area *target);

static int8_t count = 0;

/*
	功能：初始化菜单列表 <此函数名不可修改>
*/
static void MakeMenu(void)
{
	MesInit_Key(&Mes);//消息

	
	ProgressBarInit(&Bar, 0, 50, 14, 10)->animation=ENABLE; //使能进度条动画
	
	TargetMenu = &menumain;
	TargetMenuPointrt.style = ENABLE; //使能指示器动画
	
	AddToFunctionTicker(50, normalRun, MenuRefresh, 0); //所有界面刷新
	

	SetMenu(&menumain, MSCX(16*2), MSCY(16), 16*2, 16, ENABLE, NULL)->menuinterface = test1_1;
	
		SetMenu(&menutest, 0, 0, 16*2, 16, ENABLE, NULL)->menuinterface = test1_2;
		BatchFastSimilarMenuDown(&menutest, menuArray, 10, 2, test1_2);
		LinkToParentClass(&menumain, &menutest); //链接到父类
		FastSimilarMenuDown(&menutest, &menuTail, 2)->menuinterface = MenuTail; //添加尾特，并殊处理
		menuTail.checked = DISABLE;
		MakeMenuListRing(&menutest); //首尾相连
		AddToSpecialFunction(&menuTail, EnterMenu, 0); //特殊功能
	
			SetMenu(&menu2, 0, 0, 32, 16, ENABLE, NULL)->menuinterface = test1_1;
			LinkToParentClass(&menuArray[2], &menu2);//链接到父类
}

/*
	功能：菜单显示的具体实现函数
	注意：函数必须是 void 类型，形参必须且仅为 menu_area *target
*/


void test1_1(menu_area *target)
{
	FontInfoType *fontinfo;
	
	uint8_t limitsize[] = {30, 20, 0, 0};
	
//	fontinfo = MenuShowNum(target, 0, 0, 0, target->id);
	
	if(count < 20)
	{
		fontinfo = m_printf(target, 1, 0, 0, "1滋2\nabc\nccd123");
	}
	else
	{
		fontinfo = m_printf(target, 1, 0, 0, "12");
	}
	
	if(SetIndicatorSize(fontinfo, target, 1, limitsize)) // 尺寸自动调整
	{
		MesBro_Key(Menu_noaction);
	}

	if(count > 20)
		gotoMenu(&menutest, Menu_Sub);

	
	
}

void test1_2(menu_area *target)
{
	FontInfoType *fontinfo;
	
	fontinfo = MenuShowNum(target, 0, 0, 0, target->id);
	
	SetIndicatorSize(fontinfo, target, 0, NULL); // 尺寸自动调整
}

void MenuTail(menu_area *target)
{
	if(TriggerCheck(target, EnterMenu))
	{
		if( FindMeunListHeard(TargetMenuPointrt.LastTargetMenuP) == &menumain)
		{
			ScrollingDisplay_Y( FindMeunListHeard(target) , 0, 3000, 0, 63, 0); // 消除 切入菜单列表时 滚动效果， 注意 showSY与showEY的大小
		}
		
		MenuCoorAlignment(TargetMenu, 0); //菜单折叠
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



/* ************** 菜单跟随刷新 ******************** */


/**** 菜单的每次刷新都会执行该函数 执行优先级最高 《！！不要写显示相关函数，会被异常清空！！》 ****/
void MenuAlwaysRun_PH(void)
{
	
	MenuDynamicBlurry(0); //界面虚化
	
}



/**** 菜单的每次刷新都会执行该函数 执行优先级中****/
void MenuAlwaysRun_PM(void)
{
	
	ListSwitchIns(&Mes, 0, 0, 63); //滚动列表
	
}



/**** 菜单的每次刷新都会执行该函数 执行优先级低 ****/
void MenuAlwaysRun_PL(void)
{
	
	if(++count > 40) count=0;
	MenuShowNum(NULL, 70, 0, 0, count);
	
	Bar.maxVal = NextCancheMenuList(FindMeunListHeard(TargetMenu), -1)->id; //设置进度条最大值
	
	SetProBarAttibute(&Bar, hideframe);
	ProgressBar(&Bar, 35, 40, TargetMenuPointrt.TargetMenuP->id); //进度条
	DrawRoundRect(32, 37, Bar.width+6, Bar.high+6, Bar.R+2);

	MenuDynamicBlurry(1);//界面虚化
	
}




//************* 初始化菜单相关内容 *********************

void MenuInit(void)
{
	MenuSysBaseInit(); //系统初始化
	MakeMenu(); //菜单列表初始化

	/* ********** USER BEGIN********** */
	
	
	
	/* ********** USER END ********** */
	
	MenuHeartTimeStart = ENABLE;//菜单心跳
	
}








