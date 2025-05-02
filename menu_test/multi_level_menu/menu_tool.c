#include "menu_tool.h"

// *******************************
// *********** 消 息 *************
// *******************************

MessageTypedef *Message_Key = NULL; //按键消息广播列表 

/**
	功能：  消息广播
	mes：   消息链表句柄 
	mesage：要广播的消息 
 **/
void MessageBroadcast(MessageTypedef *mes, uint16_t mesage)
{
	if(mes == NULL) return;
	for( ; ; ) 
	{
		if(  !(mes->state & Mes_Have) ) {
			mes->message = mesage;
			mes->state |=  Mes_Have;
		}
		mes = mes->next;
		if(mes == NULL) break;
	}
}

/*
	功能：消息初始化
	Target：要列入的消息列表头句柄（指针） 
	mes：要初始化的消息 
*/

MessageTypedef * MessageInit(MessageTypedef** Target, MessageTypedef* mes)
{
	MessageTypedef *p = *Target;
	
	if(*Target == NULL) {
		*Target = mes;
	}
	else {
		for( ; ; )
		{
			if(p->next == NULL) break;
			p = p->next;
		}
		p->next = mes;
	}
	
	mes->message = 0;
	mes->state = 0;
	mes->next = NULL;
	
	return mes;
}



// ======================== 横 向 滚 动 显 示 ========================

/*
	功能： 对 菜单列表 的x坐标+值
	target：当前所处的仍一菜单指针
	showSX：列表允许显示的起始y坐标
	showEX：列表允许显示的结束y坐标
	x_dat：增加值
	style: 显示设置 0 不在显示区域内的会被使能menulistend，提高刷新效率

*/
static void ChangeMenuX(menu_area *target, int16_t showSX, int16_t showEX, int16_t x_dat, uint8_t style)
{
	menu_area *heard = NULL;
	menu_area *tail = NULL;

	if(!target) return;

	heard = FindMeunListHeard(target);
	tail = FindMeunListTail(target);

	for( ; ;) {
		heard->x += x_dat;
		if(style == 0) {
			if( heard->x < showSX  || (heard->x + heard->width) > showEX) {
				heard->menulistend = ENABLE;
			}
			else{
				heard->menulistend = DISABLE;
			}
		}
		
		if(heard == tail) break;
		heard = heard->next;
	}
}

/*
	功能：滚动显示
	target：当前所处的任一菜单指针， 滚动参考为此
	showSY：列表允许显示的起始y坐标 头坐标
	showEY：列表允许显示的结束y坐标 底坐标
	TarSY： 指针允许的起始y坐标     头坐标 应 >= showSY
	TarEY： 指针允许的结束y坐标     底坐标 应 <= showEY
	style： 逐次逼近显示.   0 直接刷新至目标   1 逐像素刷新至目标

	ret: 无需滚动或滚动完成返回1，否则返回0

	注意：该函数会改变大量菜单的menulistend属性，在该菜单所在的菜单列表中，对于出入菜单特殊功能，
		建议使用EnterMenu，ExitMenu。EnterShowMenuList与ExitShowMenuList存在多次触发问题
*/
uint8_t ScrollingDisplay_X(menu_area *target, int16_t showSX, uint8_t showEX, int16_t TarSX, int16_t TarEX, uint8_t style) 
{
	const uint8_t speed1=17, speed2=9, speed3=3;
	uint8_t state = 1;

	menu_area *p;

	p = FindMeunListHeard(target);
	
	if( !(p->specialfeatures & MenuScrollingX) ) {
		p->specialfeatures |= MenuScrollingX;
		ChangeMenuX(target, showSX, showEX, 0, style);
	}
	
	if(showSX > TarSX) showSX = TarSX;
	if(TarEX > showEX) TarEX = showEX;
	
	if(target->x < TarSX) {
		if(style == 0)  //直达
			ChangeMenuX(target, showSX, showEX, TarSX-target->x, 0);
		else { //滚动
				if(TarSX-target->x > speed1) ChangeMenuX(target, showSX, showEX, speed1, 1);
				else if(TarSX-target->x>speed2) ChangeMenuX(target, showSX, showEX, speed2, 1);
				else if(TarSX-target->x>speed3) ChangeMenuX(target, showSX, showEX, speed3, 1);
				else ChangeMenuX(target, showSX, showEX, 1, 1);
				state = 0;
		}
	}
	else if(target->x+target->width-1 > TarEX) {
		if(style == 0) {
			ChangeMenuX(target, showSX, showEX, TarEX-target->x - target->width+1, 0);
		}
		else {
			if(target->x+target->width-TarEX>speed1) ChangeMenuX(target, showSX, showEX, -speed1, 1);
			else if(target->x+target->width-TarEX>speed2) ChangeMenuX(target, showSX, showEX, -speed2, 1);
			else if(target->x+target->width-TarEX>speed3) ChangeMenuX(target, showSX, showEX, -speed3, 1);
			else ChangeMenuX(target, showSX, showEX, -1, 1);
			state = 0;
		}
	}
	return state;
}

// ======================== 纵 向 滚 动 显 示 ========================

/*
	功能：对 菜单列表 的y坐标+值
	target：当前所处的仍一菜单指针
	showSY：列表允许显示的起始y坐标
	showEY：列表允许显示的结束y坐标
	y_dat：增加值
	style: 显示设置 0 不在显示区域内的会被使能menulistend，提高刷新效率

*/
static void ChangeMenuY(menu_area *target, int16_t showSY, int16_t showEY, int16_t y_dat, uint8_t style)
{
	menu_area *heard = NULL;
	menu_area *tail = NULL;
	
	if(!target) return;
	
	heard = FindMeunListHeard(target);
	tail = FindMeunListTail(target);
	
	for( ; ;) {
		heard->y += y_dat;
		if(style == 0) {
			if( heard->y < showSY  || (heard->y + heard->high-1) > showEY ) {
				heard->menulistend = ENABLE;
			}
			else{
				heard->menulistend = DISABLE;
			}
		}
		
		if(heard == tail) break;
		heard = heard->next;
	}
}

/*
	功能：滚动显示
	target：当前所处的任一菜单指针, 滚动参考为此
	showSY：列表允许显示的起始y坐标 头坐标
	showEY：列表允许显示的结束y坐标 底坐标
	TarSY： 指针允许的起始y坐标     头坐标 应 >= showSY
	TarEY： 指针允许的结束y坐标     底坐标 应 <= showEY
	style： 逐次逼近显示   0 直接刷新至目标   1 逐像素刷新至目标

	ret: 无需滚动或滚动完成返回1，否则返回0

	注意：该函数会改变大量菜单的menulistend属性，在该菜单所在的菜单列表中，对于出入菜单特殊功能，
		建议使用EnterMenu，ExitMenu。EnterShowMenuList与ExitShowMenuList存在多次触发问题
*/
uint8_t ScrollingDisplay_Y(menu_area *target, int16_t showSY, uint8_t showEY, int16_t TarSY, int16_t TarEY, uint8_t style) 
{
	const uint8_t speed1=17, speed2=9, speed3=3;
	uint8_t state = 1;
	menu_area *p;
	
	if(target == NULL) return 1;
	
	p = FindMeunListHeard(target);
	
	if( !(p->specialfeatures & MenuScrollingY) ) {
		p->specialfeatures |= MenuScrollingY;
		ChangeMenuY(target, showSY, showEY, 0, style);
	}
	
	if(showSY > TarSY) showSY = TarSY;
	if(TarEY > showEY) TarEY = showEY;
	
	if(target->y < TarSY) {
		if(style == 0)  //直达
			ChangeMenuY(target, showSY, showEY, TarSY-target->y, 0);
		else { //滚动
			if(TarSY-target->y>speed1) ChangeMenuY(target, showSY, showEY, speed1, 1);
			else if(TarSY-target->y>speed2) ChangeMenuY(target, showSY, showEY, speed2, 1);
			else if(TarSY-target->y>speed3) ChangeMenuY(target, showSY, showEY, speed3, 1);
			else ChangeMenuY(target, showSY, showEY, 1, 1);
		} 
		state = 0;          
	}
	else if(target->y+target->high-1 > TarEY) {
		if(style == 0) 
			ChangeMenuY(target, showSY, showEY, TarEY-target->y - target->high+1, 0);
		else {
			if(target->y+target->high-TarEY>speed1) ChangeMenuY(target, showSY, showEY, -speed1, 1);
			else if(target->y+target->high-TarEY>speed2) ChangeMenuY(target, showSY, showEY, -speed2, 1);
			else if(target->y+target->high-TarEY>speed3) ChangeMenuY(target, showSY, showEY, -speed3, 1);
			else ChangeMenuY(target, showSY, showEY, -1, 1);
		}
		state = 0;
	}
	return state;
}


// ************* 菜 单 滑 动 ****************

/*
	功能：菜单堆叠（与显示的头菜单重叠）
	target：句柄
*/
void MenuCoorAlignment(menu_area *target)
{
	if(target == NULL) return ;

	menu_area *MenuShowHead = MenuListShowHead(target);
	menu_area *MenuShowTail = MenuListShowTail(target);
	menu_area *p;

	if(MenuShowHead == MenuShowTail) return ;

	p = MenuShowHead->next;

	for( ; ; )
	{
		p->x = MenuShowHead->x;
		p->y = MenuShowHead->y;

		if(p==MenuShowTail) break;
		p = p->next;
	}
}

/*
	功能：使堆叠的菜单 以头菜单 为目标恢复展开状态
	target：句柄
	mod： 0 纵向展开  1：横向展开
	style: 0 直接展开   1 动画展开
	ret : 完全展开或无需展开返回1，否则返回0
	

	注意：重新展开后，可能与原有的坐标不一致,会使其更紧凑
				！！会使菜单依次往后排序，无滚动菜单列表不要使用此函数
 */
uint8_t MenuCoorRecovery(menu_area *target, uint8_t mod, uint8_t style)
{
	const uint8_t speed1=17, speed2=5, speed3=2;
	uint8_t state=1;

	
	if(target == NULL) return 1;

	menu_area *MenuShowHead = MenuListShowHead(target);
	menu_area *MenuShowTail = MenuListShowTail(target);
	menu_area *p;

	int16_t dif;

	if(MenuShowHead == MenuShowTail) return 1;

	p = MenuShowHead->next;

	for( ; ; )
	{
		if(mod == 0) // 纵向展开
		{
			if(style==0) {
				p->y = p->previous->y + p->previous->high;
			}
			else 
			{
				dif = p->y - (p->previous->y + p->previous->high);
				if(dif != 0)
				{
					if( myabs(dif) > speed1) 
						p->y += dif<0?speed1:-speed1;
					else if( myabs(dif) > speed2) 
						p->y += dif<0?speed2:-speed2;
					else if( myabs(dif) > speed3) 
						p->y += dif<0?speed3:-speed3;
					else
						p->y += dif<0?1:-1;
					state = 0;
				}
			}
		}
		else         // 横向展开
		{
			if(style==0) {
				p->x = p->previous->x + p->previous->width;
			}
			else {
				dif = p->x - (p->previous->x + p->previous->width);
				if(dif != 0)
				{
					if( myabs(dif) > speed1) 
						p->x += dif<0?speed1:-speed1;
					else if( myabs(dif) > speed2) 
						p->x += dif<0?speed2:-speed2;
					else if( myabs(dif) > speed3) 
						p->x += dif<0?speed3:-speed3;
					else
						p->x += dif<0?1:-1;
					state = 0;
				}
			}
		}
		if(p==MenuShowTail) break;
		p = p->next;
	}
	
	return state;
}

/*
	功能：列表切换实例（有动画效果）
				列表的 展开 与 滚动
	mes：消息句柄
	start：指示器允许起始位置
	end：  指示器允许结束位置
	dir：  方向。0 纵向，1横向
	
	注意：当使用“特殊功能”的EnterMenu来堆叠菜单 （MenuCoorAlignment） 时，此函数应比
			MenuCoorAlignment执行的优先级低，（如可放入在 MenuAlwaysRun_PM）
*/
void ListSwitchIns(MessageTypedef *mes, int16_t start, int16_t end, bool dir)
{
	uint8_t sta=0;
	uint8_t (*ScrollingDisplay)(menu_area *target, int16_t showSY, uint8_t showEY, int16_t TarSY, int16_t TarEY, uint8_t style);

	if(dir == 0)
		ScrollingDisplay = ScrollingDisplay_Y;
	else
		ScrollingDisplay = ScrollingDisplay_X;

	if( MesHave( (*mes)) ) //有消息
	{
		if(MesRead( (*mes) ) == Menu_Father) {
			if(MenuCoorRecovery(TargetMenu, dir, 1)) {
				if(ScrollingDisplay(TargetMenu, start, end, start, end, 1))
					MesClear( (*mes) );
			}
		}
		else {
			sta += MenuCoorRecovery(TargetMenu, dir, 1);
			sta += ScrollingDisplay(TargetMenu, start, end, start, end, 1);
			if(sta == 2) MesClear( (*mes) );
		}
	}
}


// *******************************************
// ************ 函 数 定 时 执 行 ************
// *******************************************

static FunctionTicker *FunctionTickerPointer = NULL;
static uint8_t FTrigFlag=DISABLE;

FunctionTicker *SetFunctionTicker(FunctionTicker *FTtarget, uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(void))
{
	if(FTtarget == NULL) return NULL;
	
	if(FunctionTickerPointer == NULL) { //头指针是否为空
		FunctionTickerPointer = FTtarget;
	}
	else {
		FunctionTicker *Tail = FunctionTickerPointer;
		
		while(Tail->next) {
			Tail=Tail->next;
		}
		Tail->next = FTtarget;
	}
	FTtarget->next = NULL;
	FTtarget->run = ENABLE;
	FTtarget->Flag = DISABLE;
	
	FTtarget->ms = ms;
	FTtarget->count = 0;
	FTtarget->RunMod = RunMod;
	FTtarget->Function = Function;
	
	return FTtarget;
}
/*
	功能：函数定时执行
	ms：定时时间
	RunMod：模式  normalRun或interruptRun
*/
FunctionTicker *AddToFunctionTicker(uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(void))
{
	FunctionTicker *FTtarget;
	
	FTtarget = MenuMalloc(sizeof(FunctionTicker));
	
	if(FTtarget == NULL) return NULL;
	
	return SetFunctionTicker(FTtarget, ms, RunMod, Function);
}

/*
	功能：在中断中查询函数是否该执行
*/
void FunctionTickerRunIRQ(uint16_t ms)
{
	FunctionTicker *p = FunctionTickerPointer;
	
	FTrigFlag = ENABLE;
	while(p)
	{
		if(p->run == ENABLE) {
			p->count += ms;
			if(p->count >= p->ms) {
				p->count=0;
				if(p->RunMod == interruptRun) {
					if(p->Function) p->Function();
				}
				else if(p->RunMod == normalRun) {
					p->Flag = ENABLE;
				}
			}
		}
		p=p->next;
	}
}

/*
	功能：在while中查询函数是否执行
*/
void FunctionTickerRun(void)
{
	FunctionTicker *p = FunctionTickerPointer;
	
	if(FTrigFlag == ENABLE)
	{
		FTrigFlag = DISABLE;
		while(p) {
			if(p->Flag == ENABLE) {
				p->Flag = DISABLE;
				if(p->Function) p->Function();
			}
			p=p->next;
		}
	}
}






// **********************************************
// ****************** 单次运行 ******************
// **********************************************

// t=0: dat相等运行一次  t=1：dat2改变运行一次!!!只能被调用一次!!!
bool tRunOne(TypeRunOne *RunOne, bool t, unsigned int dat1,unsigned int dat2)
{
	if(!t) //0: 值为dat运行一次
	{
		if(dat1 != dat2)
		{
			RunOne->floag = 1;
		}		
		if(RunOne->floag==1 && (dat1 == dat2))
		{
			RunOne->floag = 0;
			return 1;
		}
	}
	else //1：dat改变运行一次
	{
		if(RunOne->dat != dat2)
		{
			RunOne->dat = dat2;
			return 1;
		}
	}
	return 0;
}



// ****** 计算工具 **********


uint16_t myabs(int16_t dat)
{
	return dat<0?-dat:dat;
}








