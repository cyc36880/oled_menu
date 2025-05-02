#include "menu.h"
#include "Graphicalfunctions.h"


// ================================= 设 备 ========================================

enum MenuState StatusInformation = Menu_noaction; //输入设备状态
enum MenuState StatusInformationAlways = Menu_noaction; // 输入设备状态 <不会改变>

uint8_t InuptEnable = ENABLE;


/*
	* 功能：输入设备状态
*/

static enum MenuState Scan(void)
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

/*
	0: 输入立即触发
	1: 输入回弹后再触发，但不影响StatusInformationAlways
*/
#if 1
static bool KeyState(enum MenuState *k)
{
	static enum MenuState KeySt = Menu_noaction;
	
	if(KeySt != Scan())
	{
		HAL_Delay(1);
		if(KeySt != Scan())
		{
			*k = KeySt;
			KeySt = Scan();
			StatusInformationAlways = KeySt;
			return 1;
		}
	}
	return 0;
}
#else
static bool KeyState(enum MenuState *k)
{
	static enum MenuState KeySt = Menu_noaction;
	
	if(KeySt != Scan())
	{
		HAL_Delay(1);
		if(KeySt != Scan())
		{
			KeySt = Scan();
			*k = KeySt;
			StatusInformationAlways = KeySt;
			return 1;
		}
	}
	return 0;
}
#endif

/*
	功能：输入设备扫描，该函数仅由系统调用
*/
static void EquipmentState(void)
{
	if(InuptEnable == DISABLE) { //输入是否使能
		StatusInformationAlways = Menu_noaction;
		StatusInformation = Menu_noaction;
		return;
	}
	if(KeyState(&StatusInformation)){ //按键扫描
		if(StatusInformation != Menu_noaction){ //非空闲
			ScreenPara.refresh = 1; //屏幕刷新
		}
	}
}


// ================================= 屏 幕 ========================================

const unsigned int BUFFWEIGH = SCREENWIDTH;
const unsigned int BUFFHIGH  = DIVIDEUP(SCREENHIGH);
enum ScreenShowManner SCREENSHOWMANNER = ScreenNormal; //屏幕显示方式
enum GraphicsShowManner GRAPHICSSHOWMANNER = GraphicsNormal; //图形显示方式

// 定义屏幕信息
TypedefScreen ScreenPara = {BUFFWEIGH, BUFFHIGH, 1};

// 显示缓存
unsigned char DisplayBuff[BUFFWEIGH * BUFFHIGH] = {0};


//清空显示缓存
void ClearnBuff(void)
{
	unsigned int i, j = BUFFWEIGH * BUFFHIGH;
	for(i=0; i<j; i++)
		DisplayBuff[i] = 0;
}


// ========================== 菜 单 ==================================

uint32_t MenuSize = 0;//菜单申请的空间大小
/*
	功能：菜单申请空间
	size：申请空间的字节数
	@ret：申请空间的首地址，失败为空
*/
void *MenuMalloc(uint16_t size)
{
	void *addr;
	addr = malloc(size);
	if(addr) {
		MenuSize += size;
	}
	return addr;
}


/*
*	功能：对已有菜单注册或添加菜单 
*	target：已有的菜单
*	x, y, width, high: 为注册菜单单元的位置与大小 
*	checked: 能否被选中，1能 0否 
*	transfer：为 NULL 注册的是菜单头，填入菜单地址为其尾加入 
*/
menu_area *SetMenu(menu_area *target, int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer)
{
	menu_area *p = target;
	menu_area *k;
	
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

	p->x = x;
	p->y = y;
	p->width = width;
	p->high = high;
	p->checked =  checked; // 能否选中 
	p->menulistend = DISENBLE; //菜单列表结束
	p->next = NULL;      //下一个
	p->subclass = NULL;  //子类 
	p->father = NULL;    // 父类 
	p->menuinterface = NULL; // 菜单内容
	p->specialfeatures = NULL; //特殊功能
	p->specfeattrigflag = NULL;//特殊功能触发标志
	p->menu_time = NULL; //时间队列
	return p;
}

/*
*	功能：注册或添加菜单 
*	x, y, width, high: 为注册菜单单元的位置与大小 
*	checked: 能否被选中，1能 0否 
*	transfer：为 NULL 注册的是菜单头，填入菜单地址为其尾加入 
*/
menu_area * AddToMenuList(int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer)
{
	menu_area *p;
	
	p = (menu_area *) MenuMalloc(sizeof(menu_area)); //申请内存空间
	
	return SetMenu(p, x, y, width, high, checked, transfer);
}

/*
	功能：快速目标菜单下方仿制
	target：要仿制的目标菜单
	source：拥有的菜单
	kind：类型 0:有超出部分立即按照头创建 1:只有完全在屏幕下方才按头创建 2:仅在最后下方仿造
*/

menu_area * FastSimilarMenu(menu_area *target, menu_area *source, uint8_t kind)
{
	if(!source) return NULL;
	if(!target) {
		return SetMenu(source, 0, 0, 0, 0, ENABLE, NULL);
	}
	target = FindMeunListHeard(target);
	menu_area *p = FindMeunListTail(target); 
	
	if(kind == 0)  { //超出屏幕，立即
		if(p->y+p->high*2 > SCREENHIGH) {
			p->menulistend = ENABLE;
			return SetMenu(source, target->x, target->y, target->width,target->high, target->checked, p);
		}
		else {
			return SetMenu(source, p->x, p->y+p->high, p->width,p->high, p->checked, p);
		}
	}
	else if(kind == 1){
		if(p->y+p->high >= SCREENHIGH) {
			p->menulistend = ENABLE;
			return SetMenu(source, target->x, target->y, target->width,target->high, target->checked, p);
		}
		else {
			return SetMenu(source, p->x, p->y+p->high, p->width,p->high, p->checked, p);
		}
	}
	else {
		return SetMenu(source, p->x, p->y+p->high, p->width,p->high, p->checked, p);
	}
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
		if(p==NULL || p==target) return target;
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
		if(targetmenu->previous->menulistend == ENABLE) return targetmenu; //上一个为结尾
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
		if(targetmenu->menulistend == ENABLE) return targetmenu;//显示菜单尾
		targetmenu = targetmenu->next;
	}
}

/*
	功能： 目标菜单首尾相连，地址为空跳过 
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
	注意：该函数将占用 specialfeatures，执行指定列表时，会将 specialfeatures 置 1， 
		  用于判断是不是时间列表函数执行的该函数
*/
static void AddToMenuTimeList(menu_area *target, uint16_t ms)
{
	if(target == NULL) return; // 检查地址是否有效
	
	target->menu_time = (menu_timems *) MenuMalloc(sizeof(menu_timems));
	if(!(target->menu_time)) return;
	target->menu_time->counttime=0; //默认起始计数值
	target->menu_time->timems = ms; // 想要执行时间间隔
}







// ====================== 特 殊 功 能 ========================


/*
	功能：特殊功能注册
	target：要注册的菜单
	function：特殊功能表里的指针，可或
	ms；延时时间，含MenuTime时，ms为延时，不含时，ms不被使用，一般填NULL
*/
void AddToSpecialFunction(menu_area *target, uint16_t function, uint16_t ms)
{
	
	if(target == NULL) return;
	
	if(function & MenuTime) { 
		AddToMenuTimeList(target, ms);//时间队列
	}
	target->specialfeatures |= function; //特殊功能注册
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
	if( (target->specfeattrigflag)&function ){
		triggerflag = 1;
		target->specfeattrigflag &= ~function;
	}
	return triggerflag;
}
/*
	说明：隶属于某一菜单，且只有一个的菜单列表，向菜单头赋予指定属性
	功能：菜单列表始终执行函数
	target: 列表的任意指针
*/
static MenuListOverall *MenuOverallPointer = NULL;

MenuListOverall *MenuOverall(menu_area *target)
{
	MenuListOverall *MenuOverallP;
	MenuListOverall *Tar = NULL;
	menu_area *p;
	
	p = FindMeunListHeard(target);
	if(!p) return NULL; //未找到菜单头，跳过
	if(p->specialfeatures & MenuHaveOverall) return NULL; //已有跳过
	
	Tar = (MenuListOverall *)MenuMalloc(sizeof(MenuListOverall)); //申请空间
	if( !(Tar) ) return NULL; //空间申请失败
	p->specialfeatures |= MenuHaveOverall; //菜单头赋予属性
	
	if(MenuOverallPointer==NULL){ //创建
		MenuOverallPointer = Tar;
	}
	else{
		MenuOverallP=MenuOverallPointer;
		while(MenuOverallP->next){
			MenuOverallP = MenuOverallP->next;
		}
		MenuOverallP->next = Tar;
	}
	Tar->Affiliation = target;
	Tar->menuinterface = NULL;
	Tar->next = NULL;
	
	return Tar;
}





// ======================== 滚 动 显 示 ========================

/*
	功能：对 菜单列表 的y坐标+值
	target：当前所处的仍一菜单指针
	showSY：列表允许显示的起始y坐标
	showEY：列表允许显示的结束y坐标
	y_dat：增加值
	@ret：NUL
*/
static void ChangeMenuY(menu_area *target, int16_t showSY, int16_t showEY, int16_t y_dat)
{
	menu_area *heard = NULL;
	menu_area *tail = NULL;
	
	if(!target) return;
	
	heard = FindMeunListHeard(target);
	tail = FindMeunListTail(target);
	
	for( ; ;) {
		heard->y += y_dat;
		if( heard->y < showSY  || (heard->y + heard->high) >= showEY || (heard->next ? heard->next->y + heard->next->high + y_dat > showEY : 0)) {
			heard->menulistend = ENABLE;
		}
		else{
			heard->menulistend = DISABLE;
		}
		if(heard == tail) break;
		heard = heard->next;
	}
}

/*
	功能：滚动显示
	target：当前所处的任一菜单指针
	showSY：列表允许显示的起始y坐标 头坐标
	showEY：列表允许显示的结束y坐标 底坐标
	TarSY： 指针允许的起始y坐标     头坐标 应 >= showSY
	TarEY： 指针允许的结束y坐标     底坐标 应 <= showEY
	@ret：NUL

	注意：该函数会改变大量菜单的menulistend属性，在该菜单所在的菜单列表中，对于出入菜单特殊功能，
		建议使用EnterMenu，ExitMenu。EnterShowMenuList与ExitShowMenuList存在多次触发问题
*/
void ScrollingDisplay_Y(menu_area *target, int16_t showSY, uint8_t showEY, int16_t TarSY, int16_t TarEY) 
{
	menu_area *p;
	
	p = FindMeunListHeard(target);
	
	if( !(p->specialfeatures & MenuScrolling) ) {
		p->specialfeatures |= MenuScrolling;
		ChangeMenuY(TargetMenu, showSY, showEY, 0);
	}
	
	if(showSY > TarSY) showSY = TarSY;
	if(TarEY > showEY) TarEY = showEY;
	
	if(TargetMenu->y < TarSY) {
		ChangeMenuY(TargetMenu, showSY, showEY, TarSY-TargetMenu->y);
	}
	else if(TargetMenu->y+TargetMenu->high >= TarEY) {
		ChangeMenuY(TargetMenu, showSY, showEY, TarEY-TargetMenu->y - TargetMenu->high);
	}
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
	
	if(x>=target->width || y>=target->high) return; // 判断是否超出菜单边界
	if(x<0 || y<0) return;
	
	write_point(px, py, w_b);
}

/*
	功能：画目标菜单的矩形，空指针不画
	target: 目标菜单
*/
static void DrawMenuRectangle(menu_area *target)
{
	int16_t x1=0, y1=0, x2=0, y2=0;
		
	if(target->width<=0 || target->high<=0) return;
	if(target->x>=SCREENWIDTH || target->y>=SCREENHIGH) return;
	if(target->x+target->width<0 || target->y+target->high<0) return;
	
	x1 = target->x;
	y1 = target->y;
	x2 = x1 + target->width-1;
	y2 = y1 + target->high-1;
	
	DrawLine(x1, y1, x2, y1);
	DrawLine(x1, y1+1, x1, y2-1);
	DrawLine(x2, y2, x1, y2);
	DrawLine(x2, y2-1, x2, y1+1);
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
		if(targetmenu->previous->menulistend == ENABLE) return targetmenu; //上一个为结尾
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
		if(targetmenu->menulistend == ENABLE) return targetmenu;//显示菜单尾
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
bool MenuHeartTimeStart = DISABLE; //时间列表开始标志
static bool RefreshFlagForHeart = 0; //时间列表刷新标志

void MenuHeartTime(void)
{
	menu_area *p = TargetMenu; //菜单
	menu_area *MenuShowTail = NULL; //显示菜单尾
	
	bool refreshflag = 0; //屏幕刷新标志，0不刷新，1刷新
	
	if(MenuHeartTimeStart==DISABLE) return; //是否开始
	
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
				p->specfeattrigflag |= MenuTime; //赋值状态
				if( (p->specialfeatures)& MenuTimeForce ){ //是否强制执行
					if(p->menuinterface){ //指向地址存在
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
	static menu_area *lastmenuheard = NULL; 
	menu_area *lastmenutail = FindMeunListTail(lastmenuheard); 
	static menu_area *lastshowmenuheard = NULL;
	menu_area *lastshowmenutail = MenuListShowTail(lastshowmenuheard);	
	
	menu_area *nowmenuheard = FindMeunListHeard(target); 
	menu_area *nowmenutail = FindMeunListTail(target);
	menu_area *nowshowmenuheard = MenuListShowHead(target);
	menu_area *nowshowmenutail = MenuListShowTail(target);
	
	menu_area *p = NULL;
	
	uint8_t flag = 0;
	
	if(!lastmenuheard){
		lastmenuheard = nowmenuheard;
		lastmenutail = FindMeunListTail(lastmenuheard); 
		lastshowmenuheard = nowshowmenuheard;
		lastshowmenutail = MenuListShowTail(lastshowmenuheard);	
	}
	
	if( lastmenuheard != nowmenuheard){ //切换菜单级
		for( p=lastmenuheard; ; ){ //上一级退出执行
			if(p->specialfeatures & ExitMenu) {
				p->specfeattrigflag |= ExitMenu;
				p->menuinterface(p);
			}
			if(p==lastshowmenuheard){
				flag = 1;
			}
			if(flag) {
				if(p->specialfeatures & ExitShowMenuList) {
					p->specfeattrigflag |= ExitShowMenuList;
					p->menuinterface(p);
				}
			}
			if(p == lastshowmenutail) {
				flag = 0;
			}
			if(p == lastmenutail) break;
			p = p->next;
		}
		for(p=nowmenuheard; ; ){//本级进入执行
			if(p->specialfeatures & EnterMenu) {
				p->specfeattrigflag |= EnterMenu;
				p->menuinterface(p);
			}
			if(p==nowshowmenuheard){
				flag = 1;
			}
			if(flag) {
				if(p->specialfeatures & EnterShowMenuList) {
					p->specfeattrigflag |= EnterShowMenuList;
					p->menuinterface(p);
				}
			}
			if(p == nowshowmenutail) {
				flag = 0;
			}
			if(p == nowmenutail) break;
			p = p->next;
		}
		lastmenuheard = nowmenuheard;
		lastshowmenuheard = nowshowmenuheard;
		ClearnBuff(); // 清空缓存
		return;
	}
	
	if(lastshowmenuheard != nowshowmenuheard){ //切换显示菜单
		for(p=lastshowmenuheard; ; ) { //上一显示列表退出执行
			if(p->specialfeatures & ExitShowMenuList) {
				p->specfeattrigflag |= ExitShowMenuList;
				p->menuinterface(p);
			}
			if(p == lastshowmenutail) break;
			p = p->next;
		}
		for(p=nowshowmenuheard; ; ) { //本显示列表进入执行
			if(p->specialfeatures & EnterShowMenuList) {
				p->specfeattrigflag |= EnterShowMenuList;
				p->menuinterface(p);
			}
			if(p == nowshowmenutail) break;
			p = p->next;
		}
		lastshowmenuheard = nowshowmenuheard;
		ClearnBuff(); // 清空缓存
	}
}

//菜单全局显示
static void MenuListOverallRun(menu_area *target)
{
	menu_area *p;
	MenuListOverall *MenuListOverallP=MenuOverallPointer;
	
	if(!MenuListOverallP) return; //是否有全局
	p = FindMeunListHeard(target);
	if( !(p->specialfeatures & MenuHaveOverall) ) return; //是否创建
	
	while(MenuListOverallP->Affiliation != p){ //找到对应列表头的全局
		if( !(MenuListOverallP->next) ) return;
		MenuListOverallP = MenuListOverallP->next;
	}
	if(MenuListOverallP->menuinterface){
		MenuListOverallP->menuinterface();
	}
}

/*
	功能：设备输入状态改变实时目标菜单指针
*/
static void StateToPointer(void)
{
	menu_area *p = NULL;
	
	switch(StatusInformation)
	{
		case Menu_up: 		TargetMenu=NextCancheMenuList(TargetMenu, -1);break;
		case Menu_down: 	TargetMenu=NextCancheMenuList(TargetMenu,  1);break;
		case Menu_Sub:  
			if(TargetMenu->subclass != NULL) {
				StatusInformation = Menu_noaction; // 防止切换菜单列表时立即运行函数内部指令
				TargetMenu=NextCancheMenuHeard(TargetMenu, 1);
			}
			break;
		case Menu_Father:  
			// 0: 返回至菜单头的父类   1: 返回至当前菜单的父类
			#if 0
				if(TargetMenu->father != NULL) {
					StatusInformation = Menu_noaction; // 防止切换菜单列表时立即运行函数内部指令
					TargetMenu=NextCancheMenuHeard(TargetMenu, -1);
				}
			#else
				p = FindMeunListHeard(TargetMenu);
				if(p->father != NULL) {
					StatusInformation = Menu_noaction; // 防止切换菜单列表时立即运行函数内部指令
					TargetMenu=NextCancheMenuHeard(p, -1);
				}
			#endif
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
	功能：菜单运行函数。 刷新屏幕 之前，越靠后屏幕显示优先级越高
*/
void MenuRun(void)
{
	EquipmentState(); //输入设备
	
	if(RefreshFlagForHeart){ //时间列表刷新标志
		RefreshFlagForHeart = 0;
		ScreenPara.refresh = 1;
	}
	
	if(ScreenPara.refresh && TargetMenu)
	{
		StateToPointer(); // 设备输入状态改变实时目标菜单指针
		
		MenuListOverallRun(TargetMenu); //菜单全局
		SpecialFunctionRun(TargetMenu); //特殊功能运行
		MenuListInterface(); //依次显示当前菜单列表
		MenuCheckedStyle(TargetMenu);//菜单选中风格
		
		disp_flush();// 刷新屏幕
		
		ClearnBuff(); // 清空缓存
		StatusInformation = Menu_noaction; //输入设备状态复位
		ScreenPara.refresh=0;// 刷新标志复位
	}
}





