#include "menu.h"
#include "Graphicalfunctions.h"
#include "oled.h"
#include "main.h" //HAL函数库

extern uint32_t adcbuf[];

// ================================= 设 备 ========================================

static bool KeyPutDownFlag = 0; // 按键按下标志 （使只有按键才触发检查）
enum MenuState StatusInformation; //输入设备状态

/*
	* 功能：输入设备状态
*/

enum MenuState Scan(void)
{
	static enum MenuState adcstate = Menu_noaction;
	
	if(HAL_GPIO_ReadPin(K0_GPIO_Port, K0_Pin) == GPIO_PIN_RESET) return Menu_down;
	else if(HAL_GPIO_ReadPin(K1_GPIO_Port, K1_Pin) == GPIO_PIN_RESET) return Menu_Sub;
	
	if(adcbuf[0] > (1630 + 1500)){
		adcstate = Menu_Sub;
		return adcstate;
	} 
	else if(adcbuf[0]<(1630 + 200) && adcbuf[0]>(1630 - 200)) adcstate = Menu_noaction;
	else if(adcbuf[0] < (1630 - 1500)) {
		adcstate = Menu_Father;
		return adcstate;
	}
	
	if(adcbuf[1] > (1630 + 1500)){
		adcstate = Menu_down;
		return adcstate;
	} 
	else if(adcbuf[1]<(1630 + 200) && adcbuf[1]>(1630 - 200)) adcstate = Menu_noaction;
	else if(adcbuf[1] < (1630 - 1500)){
		adcstate = Menu_up;
		return adcstate;
	} 
	
	
	return adcstate;
	
	
//	return Menu_noaction;
}

bool KeyState(enum MenuState *k)
{
	static enum MenuState KeySt = Menu_noaction;
	static uint8_t count=0;
	
	if(KeySt != Scan())
	{
		HAL_Delay(1);
		if(KeySt != Scan())
		{
			if(++count > 1){
				count = 0;
				*k = KeySt;
				return 1;	
			}
			KeySt = Scan();
		}
	}
	return 0;
}

/*
	功能：输入设备扫描，该函数仅由系统调用
*/
static void EquipmentState(void)
{
	if(KeyState(&StatusInformation)){ //按键扫描
		if(StatusInformation != Menu_noaction){ //非空闲
			ScreenPara.refresh = 1;
			KeyPutDownFlag = 1; //按键按下标志置1
		}
	}
}


// ================================= 屏 幕 ========================================

const unsigned int BUFFWEIGH = SCREENWIDTH;
const unsigned int BUFFHIGH  = DIVIDEUP(SCREENHIGH);

// 定义屏幕信息
TypedefScreen ScreenPara = {BUFFWEIGH, BUFFHIGH, 1};

// 显示缓存
unsigned char DisplayBuff[BUFFWEIGH* BUFFHIGH] = {0};


//清空显示缓存
void ClearnBuff(void)
{
	unsigned int i, j = BUFFWEIGH * BUFFHIGH;
	for(i=0; i<j; i++)
		DisplayBuff[i] = 0;
}


// ========================== 菜 单 ==================================
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
	if(!p) return NULL;
	if(transfer != NULL)  //尾部加入 
	{
		k = FindMeunListTail(transfer); //找到文件尾部
		k->next = p;
		p->previous = k;
		p->id = k->id + 1; 
	}
	else //创建菜单头
	{
		p->id = MENUHEARDID; // 菜单头ID 
		p->previous = NULL; //上一个
	}
	
	p->x = x>=BUFFWEIGH ? BUFFWEIGH-1 : x;
	p->y = y>=SCREENHIGH ? SCREENHIGH-1 : y;
	p->width = width+p->x>BUFFWEIGH ? BUFFWEIGH-p->x : width;
	p->high = high+p->y>SCREENHIGH ? SCREENHIGH-p->y : high;
	p->checked =  checked; // 能否选中 
	p->menulistend = NULL; //菜单列表结束
	p->next = NULL;      //下一个
	p->subclass = NULL;  //子类 
	p->father = NULL;    // 父类 
	p->menuinterface = NULL; // 菜单内容
	p->userinformation = NULL; //用户自定义信息
	p->menu_time = NULL; //时间队列
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
	if(target==NULL || source==NULL) return; //检查地址是否有效
	target->subclass = source;
	source->father = target;
}

/**
 * 功能：对菜单向上下偏移寻址
 * target：目标寻址菜单 
 * offset：偏移步数 
 * upordown: 0向下偏移 1向上偏移 
 * 返回值：相对与源地址偏移后的地址，offset=0返回源地址，无则返回NULL 
 **/
menu_area * MenuListAddressing(menu_area *target, bool upordown, uint16_t offset)
{
	menu_area *p = target;
	uint16_t i;
	
	if(target==NULL) return NULL; //检查地址是否有效
	
	for(i=0; i<offset; i++)
	{
		if(upordown == 0) 
		{
			if(p->next == NULL){  //检查下一个是否存在
				return NULL;
			}
			p = p->next;
		}
		else
		{
			if(p->previous == NULL){ //检查上一个是否存在
				return NULL;
			}
			p = p->previous;
		}
	}
	return p;
}

/**
 * 功能：对菜单头的前后偏移寻址 
 * target：目标寻址菜单 
 * offset：偏移步数 
 * upordown: 0向后偏移 1向前偏移 
 * 返回值：相对与源地址偏移后的地址，offset=0返回源地址，无则返回NULL 
 **/
menu_area * MenuHeadAddressing(menu_area *target,  bool upordown, uint16_t offset)
{
	menu_area *p = target;
	uint16_t i;
	
	if(target==NULL) return NULL; //检查地址是否有效
	
	for(i=0; i<offset; i++)
	{
		if(upordown == 0) 
		{
			if(p->subclass == NULL){ //检查子类是否存在
				return NULL;
			}
			p = p->subclass;
		}
		else
		{
			if(p->father == NULL){ //检查父类是否存在
				return NULL;
			}
			p = p->father;
		}
	}
	return p;
}

/*
	功能：寻找以目标单元为基准的第num个菜单可选中的菜单，无则返回原地址
		  输入空地址则返回空
	target: 基地址
	num：正值向下寻找，负值向上寻找
	返回值：寻址后的地址
*/
menu_area *NextCancheMenuList(menu_area *target, int16_t num)
{
	menu_area *p = target;
	uint16_t j = num<0?-num:num; // 绝对值
	uint16_t i;

	if(p==NULL) return NULL; //检查地址是否有效
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
	uint16_t j = num<0?-num:num; // 绝对值
	uint16_t i;

	if(p==NULL) return NULL;//检查地址是否有效
	for(i=0; i<j;)
	{
		p=MenuHeadAddressing(p, num<0, 1); 
		if(p==NULL) return target;
		if(p->checked == 1) i++;
	}
	return p;	
}


/*
	功能：找到菜单所在菜单列表的菜单头，空指针返回NULL 
	target：菜单指针
*/
menu_area *FindMeunListHeard(menu_area *target) 
{
	menu_area *heard=target;
	
	if(target == NULL) return NULL;//检查地址是否有效
	for( ; ; )
	{
		if(heard->id == MENUHEARDID) break; // 菜单列表头id 
		heard = heard->previous;
	}
	return heard;
}


/*
	功能：找到菜单所在菜单列表的菜单尾，空指针返回NULL 
	target：菜单指针
*/
menu_area *FindMeunListTail(menu_area *target) 
{
	menu_area *tail=target;
	
	if(target == NULL) return NULL; //检查地址是否有效
	
	for( ; ; )
	{
		if(tail->next == NULL) break; //检查下一个是否存在
		if(tail->next->id == MENUHEARDID) break; // 循环中，下一个是否为头
		tail = tail->next;
	}
	return tail;
}

/*
	功能：返回当前显示列表的头，即使它不能被选中
	target：列表的任一菜单指针
	返回值：target为空返回空，其余返回显示菜单头
*/
menu_area *MenuListShowHead(menu_area *target)
{
	menu_area *targetmenu = target;
	
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->id == MENUHEARDID) return targetmenu;//上一个为头
		if(targetmenu->previous->menulistend == 1) return targetmenu; //上一个为结尾
		targetmenu = targetmenu->previous;
	}
}

/*
	功能：返回当前显示列表的尾，即使它不能被选中
	target：列表的任一菜单指针
	返回值：target为空返回NULL，其余返回显示菜单尾
*/
menu_area *MenuListShowTail(menu_area *target)
{
	menu_area *targetmenu = target;
	
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->next == NULL) return targetmenu; //下一个为空
		if(targetmenu->next->id == MENUHEARDID) return targetmenu;//到达菜单列表尾
		if(targetmenu->menulistend == 1) return targetmenu;//显示菜单尾
		targetmenu = targetmenu->next;
	}
}

/*
	功能： 目标菜单首位相连，地址为空跳过 
	target：所在菜单列表的任一菜单 
*/
void MakeMenuListRing(menu_area *target)
{
	menu_area *heard, *tail;
	
	if(target == NULL) return; //检查地址是否有效
	
	heard = FindMeunListHeard(target); //找标准头
	tail = FindMeunListTail(target); //找标准尾
	
	tail->next = heard; //头尾链接
	heard->previous = tail;
}


// ==================== 时 间 队 列 =================

/*
	功能：将菜单添加入时间列表，在该界面下，每ms执行指定菜单
	target：指定菜单
	ms：间隔时间
	注意：该函数将占用 userinformation，执行指定列表时，会将 userinformation 置 1， 
		  用于判断是不是时间列表函数执行的该函数
*/
static void AddToMenuTimeList(menu_area *target, uint16_t ms)
{
	if(target == NULL) return; // 检查地址是否有效
	
	target->menu_time = (menu_timems *) malloc(sizeof(menu_timems));
	if(!(target->menu_time)) return;
	target->menu_time->counttime=0; //默认起始计数值
	target->menu_time->timems = ms; // 想要执行时间间隔
}


// ====================== 特 殊 功 能 ========================

static struct SPECIALNFORMATION *SpecialFunction = NULL;

/*
	功能：特殊功能注册
	target：要注册的菜单
	function：特殊功能表里的指针，可与
	ms；延时时间，含MenuTime时，ms为延时，不含时，ms不被使用，一般填NULL
*/
void AddToSpecialFunction(menu_area *target, uint16_t function, uint16_t ms)
{
	TypedefSpeFor *p;
	TypedefSpeFor *k = SpecialFunction;
	
	if(target == NULL) return;
	
	if(function & MenuTime) { //时间队列
		AddToMenuTimeList(target, ms);
		if( function & MenuTimeForce ){ //强制执行注册
			target->userinformation = MenuTimeForce;
		}
		if( (function &(~(MenuTime | MenuTimeForce))) == 0) return;
	}
	
	if(SpecialFunction == NULL){
		SpecialFunction = (TypedefSpeFor *) malloc(sizeof(TypedefSpeFor));
		if(!SpecialFunction) return;
		SpecialFunction->target = target;
		SpecialFunction->function = function;
		SpecialFunction->next = NULL;
		SpecialFunction->TriggerFlag = function;
	}
	else{
		for( ; ; ){ //找到列表尾
			if(k->next == NULL) break;
			k = k->next;
		}
		p = (TypedefSpeFor *) malloc(sizeof(TypedefSpeFor));
		if(!p) return;
		p->target = target; //目标菜单
		p->function = function; // 特殊功能
		p->next=NULL; // 时间列表下一个为空
		p->TriggerFlag = function;
		k->next = p; //与上一个链接
	}
}

/*
	功能：特殊功能检查，触发返回1，否则返回0
	target：想要检查的菜单
	function：想要检查的特殊功能
*/
bool TriggerCheck(menu_area *target, enum SpecialInformation function)
{
	bool triggerflag = 0;
	
	if(!target) return 0;
	if( (target->userinformation)&function ){
		triggerflag = 1;
		target->userinformation &= ~function;
	}
	return triggerflag;
}

// ========================== 图 形 化 =======================

/*
	功能：在目标菜单的 >相对位置< 画点
	target：目标菜单
	x, y；相对位置
	w_b：1有色 0无色
*/
void MenuSetPoint(menu_area *target, int16_t x, int16_t y, bool w_b)
{
	int16_t px=0, py=0; // 屏幕相对位置
	
	if(target == NULL) return;
	
	px = target->x + x;
	py = target->y + y;
	
	if(x>target->width || y>target->high) return; // 判断是否超出菜单边界
	if(x<0 || y<0) return;
	
	write_point(px, py, w_b);
}

/*
	功能：画目标菜单的矩形，空指针不画
	target: 目标菜单
*/
static void DrawMenuRectangle(menu_area *target)
{
	uint16_t x1=0, y1=0, x2=0, y2=0;
	
	if(target == NULL) return;
	
	if(target->width<=0 || target->high<=0) return;
	
	x1 = target->x;
	y1 = target->y;
	x2 = x1 + target->width-1;
	y2 = y1 + target->high-1;
	
	DrawLine(x1, y1, x2, y1);
	DrawLine(x1, y1, x1, y2);
	DrawLine(x2, y2, x1, y2);
	DrawLine(x2, y2, x2, y1);
}



// ======================== 系 统 调 用 ====================

menu_area * TargetMenu = NULL; // 实时目标菜单

//以下两函数与上完全相同，为防止同时使用该函数，故设置专用函数
static menu_area *MenuListShowHeadForHeart(menu_area *target)
{
	menu_area *targetmenu = target;
	
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->previous == NULL) return targetmenu; //上一个不存在
		if(targetmenu->id == MENUHEARDID) return targetmenu;//上一个为头
		if(targetmenu->previous->menulistend == 1) return targetmenu; //上一个为结尾
		targetmenu = targetmenu->previous;
	}
}

static menu_area *MenuListShowTailForHeart(menu_area *target)
{
	menu_area *targetmenu = target;
	
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->next == NULL) return targetmenu; //下一个为空
		if(targetmenu->next->id == MENUHEARDID) return targetmenu;//到达菜单列表尾
		if(targetmenu->menulistend == 1) return targetmenu;//显示菜单尾
		targetmenu = targetmenu->next;
	}
}

/*
	功能：菜单心跳执行，每1ms执行该函数
*/
/*
	菜单心跳开始标志
	如果没有使用特殊功能中的时间列表功能，此标志不必管
	注意：请在菜单初始化的结尾置一该标志，防止中断与main同时调用相关函数
*/
bool MenuHeartTimeStart = 0; //时间列表开始标志
static bool RefreshFlagForHeart = 0; //时间列表刷新标志

void MenuHeartTime(void)
{
	menu_area *p = TargetMenu; //菜单
	menu_area *MenuShowTail = NULL; //显示菜单尾
	
	bool refreshflag = 0; //屏幕刷新标志，0不刷新，1刷新
	
	if(!MenuHeartTimeStart) return; //是否开始
	
	if(p==NULL) return; //检查地址是否有效
	
	RefreshFlagForHeart=1; //屏幕刷新
	
	p = MenuListShowHeadForHeart(p);// 找到开始显示的头
	MenuShowTail = MenuListShowTailForHeart(p);//显示菜单尾
	
	for( ; ; )
	{
		if(p->menu_time != NULL){ //是否创建
			p->menu_time->counttime++; //计时
			if( (p->menu_time->counttime) == p->menu_time->timems){ //到达计时点
				p->menu_time->counttime=0; //计时复位
				p->userinformation |= MenuTime; //赋值状态
				if( (p->userinformation)& MenuTimeForce ){ //是否强制执行
					if(!(p->menuinterface)){
						p->menuinterface(p); //执行指向函数
					}
				}
				refreshflag = 1;
			}
		}
		if(p==MenuShowTail) break;
		p=p->next;
	}
	
	if(!refreshflag) RefreshFlagForHeart=0;//屏幕刷新
}

/*
	功能：菜单列表各个菜单内容循环显示
*/
static void MenuListInterface(void)
{
	menu_area *p = TargetMenu;
	menu_area *pTail = NULL;
	
	if(p==NULL) return; //检查地址是否有效
	
	p = MenuListShowHead(p); //显示头
	pTail = MenuListShowTail(p);//显示尾
	
	for( ; ; )
	{
		if(p->menuinterface){
			p->menuinterface(p);
		}
		
		if(p == pTail) break; 
		p = p->next;
	}
}

/*
	功能：特殊功能运行
	target： 实时目标菜单指针
*/

static void SpecialFunctionRun(menu_area *target)
{
	menu_area *NowMenuHeart=NULL;
	menu_area *TargetMenuHeart=NULL;

	TypedefSpeFor *p = SpecialFunction;
	
	if(target == NULL) return; //检查地址是否有效
	if(SpecialFunction == NULL) return; //检查是否创建特殊功能
	if(!KeyPutDownFlag) return;//使只有按键才触发检查
	
	NowMenuHeart = MenuListShowHead(target);
	
	for( ; ; )
	{
		TargetMenuHeart = MenuListShowHead(p->target);
		
		if( (p->function) & EnterMenu){
				if(target == p->target){
					if( !(p->TriggerFlag & EnterMenu) ){
						p->target->userinformation |= EnterMenu;
						p->TriggerFlag |= EnterMenu; //触发标志置1
					}
				}
				else{
					p->TriggerFlag &= ~EnterMenu; // 触发标志复位
				}
			}
		if( (p->function) & ExitMenu){
				if(target != p->target){
					if(!(p->TriggerFlag & ExitMenu)){
						p->target->userinformation |= ExitMenu;
						p->TriggerFlag |= ExitMenu; //触发标志置1
					}
				}
				else{
					p->TriggerFlag &= ~ExitMenu; // 触发标志复位
				}
			}
		if( (p->function) & EnterShowMenuList){
				if(NowMenuHeart == TargetMenuHeart){
					if(!(p->TriggerFlag & EnterShowMenuList)){
						p->target->userinformation |= EnterShowMenuList;
						p->TriggerFlag |= EnterShowMenuList; //触发标志置1
					}
				}
				else{
					p->TriggerFlag &= ~EnterShowMenuList; // 触发标志复位
				}
			}
		if( (p->function) & ExitShowMenuList){
				if(NowMenuHeart != TargetMenuHeart){
					if(!(p->TriggerFlag & ExitShowMenuList)){
						p->target->userinformation |= ExitShowMenuList;
						p->TriggerFlag |= ExitShowMenuList; //触发标志置1
					}
				}
				else{
					p->TriggerFlag &= ~ExitShowMenuList; // 触发标志复位
				}
			}
		if(p->next == NULL) break; //特殊功能列表
		p = p->next;
	}
}

/*
	功能：设备输入状态改变实时目标菜单指针
*/
static void StateToPointer(void)
{
	switch(StatusInformation)
	{
		case Menu_up: 		TargetMenu=NextCancheMenuList(TargetMenu, -1);break;
		case Menu_down: 	TargetMenu=NextCancheMenuList(TargetMenu,  1);break;
		case Menu_Sub:  
			if(TargetMenu->subclass != NULL) StatusInformation = Menu_noaction; // 防止切换菜单列表时立即运行函数内部指令
			TargetMenu=NextCancheMenuHeard(TargetMenu, 1);
			break;
		case Menu_Father:  
			if(TargetMenu->father != NULL) StatusInformation = Menu_noaction; // 防止切换菜单列表时立即运行函数内部指令
			TargetMenu=NextCancheMenuHeard(TargetMenu, -1);
			break;
		default:break;
	}
}


/*
	功能：菜单选中风格
*/
static void MenuCheckedStyle(menu_area *target)
{
	DrawMenuRectangle(target); // 用户菜单选中
}

/*
	功能：菜单运行函数。 the end 之前，越靠后屏幕显示优先级越高
*/
void MenuRun(void)
{
	EquipmentState(); //输入设备
	
	if(RefreshFlagForHeart){ //时间列表刷新标志
		RefreshFlagForHeart = 0;
		ScreenPara.refresh = 1;
	}
	
	if(ScreenPara.refresh)
	{
		StateToPointer(); // 设备输入状态改变实时目标菜单指针
		
		SpecialFunctionRun(TargetMenu); //特殊功能运行
		MenuListInterface(); //依次显示当前菜单列表
		MenuCheckedStyle(TargetMenu);//菜单选中风格
		disp_flush();// 刷新屏幕//the end
		ClearnBuff(); // 清空缓存
		
		KeyPutDownFlag = 0; //按键按下标志复位
		StatusInformation = Menu_noaction; //输入设备状态复位
		ScreenPara.refresh=0;// 刷新标志复位
	}
}





