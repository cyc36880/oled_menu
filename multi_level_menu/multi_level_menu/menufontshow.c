#include "menufontshow.h"
#include "Graphicalfunctions.h"
#include "oledfont.h"


static const uint8_t *font = F8X16+2;
const uint8_t *font_SizeInf = F8X16;

//设置英文显示字体
void SetFont(const uint8_t *xfont)
{
	font = xfont+2;
	font_SizeInf = xfont;
}

/*
	功能：在菜单中写Asc字符，target为NULL在屏幕写
	target：目标菜单
	font：字体数组
	font_SizeInf：字体宽高数组
	x,y：在目标菜单的相对偏移
	asc：一个ASCII字符
*/
void MenuShowAsc(menu_area *target,int16_t x, int16_t y, uint8_t asc)
{
	uint8_t c=0;
	uint8_t h,w;
	bool w_b=0;
	uint8_t wight = font_SizeInf[0];
	uint8_t high  = font_SizeInf[1];
	
	if(asc > '~' || asc < ' ') asc = ' '; //不在显示范围
	c=asc-' ';   // ' '=32,ASCII码表
	for(h=0; h<high; h++)//字高
	{
		for(w=0; w<wight; w++) //字宽
		{
			w_b = ( font[c*wight*DIVIDEUP(high) + w+wight*(h/8)] >> (h%8) ) & 1;
			if(w_b) {
				if(target == NULL) write_point(w+x, h+y, 1);
				else               MenuSetPoint(target, w+x, h+y, 1);
			}
			else { //覆盖显示
				if(GRAPHICSSHOWMANNER == GraphicsCover) {
					if(target == NULL) write_point(w+x, h+y, 0);
					else               MenuSetPoint(target, w+x, h+y, 0);
				}
			}
		}
	}
}

//m^n函数
static uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}

/*
	功能：在菜单中写Asc数字，target为NULL在屏幕写
	target：目标菜单
	font：字体数组
	font_SizeInf：字体宽高数组
	x,y：在目标菜单的相对偏移
	len：数字长度 0自动
	num：数字
*/
void MenuShowNum(menu_area *target, int16_t x, int16_t y, uint8_t len, uint32_t num)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;	
	uint32_t nump = num;
	
	if(len == 0) { //len=0 , 自动计算长度
		if(nump == 0) {
			len = 1;
		}
		while(nump) {
			len++;
			nump/=10;
		}
	}
	
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				MenuShowAsc(target, x+font_SizeInf[0]*t, y, ' ');
				continue;
			}else enshow=1; 
		}
		MenuShowAsc(target, x+font_SizeInf[0]*t, y, temp+'0');
	}
} 

/*
	功能：在菜单中写Asc字符串，target为NULL在屏幕写
	target：目标菜单
	font：字体数组
	font_SizeInf：字体宽高数组
	x,y：在目标菜单的相对偏移
	str：一个ASCII字符串
*/
void MenuShowAscStr(menu_area *target, int16_t x, int16_t y, const uint8_t *str)
{
	while(*str != '\0')
	{
		MenuShowAsc(target, x, y, *str);
		str++;
		x+=font_SizeInf[0];
	}
}


/*
	功能：英文显示文本

target：菜单指针
x：文本的左上角x坐标
y：文本的左上角y坐标
ascw：显示宽度 《字符个数！！》
str：显示的数据串
注意：数据\n为回车，其它未识别数据显示为空格，不支持中文显示！！
*/

void CharacterText(menu_area *target, int16_t x, int16_t y, uint16_t ascw, const uint8_t *str)
{
	uint8_t showbuf[2] = {0, 0};
	uint16_t strnum = 0;
	uint16_t showx=0, showy=0;
	const uint8_t *s = str;
	uint16_t num = 0;
	
	while(*s++) {
		num++;
	}

	if(ascw == 0) {
		ascw = -1;
	}
	
	while(num)
	{
		num--;
		showbuf[0] = str[strnum++];
		if(showbuf[0] == '\n') {
			showx = 0;
			showy++;
			continue;
		}
		MenuShowAscStr(target,  x + showx*font_SizeInf[0],  y + showy*font_SizeInf[1], showbuf);
		
		if(++showx >= ascw) {
			showx = 0;
			showy++;
		}
	}
}




// ---------------- 汉 字 显 示------------------------


/*
	功能：测量以\0为结尾的字符串长度
*/
static unsigned int mystrlen(const uint8_t *str)
{
	unsigned int i=0;
	for(i=0; str[i] != '\0'; i++);
	return i;
}

///*
//	功能：测量以\0为结尾的 汉字与字符 的字符串长度
//*/
//static unsigned int HZStrLen(uint8_t *str) 
//{
//	unsigned int StrLen = mystrlen(str);
//	unsigned int i=0;
//	unsigned int strnum=0;
//	for(i=0; i<StrLen;)
//	{
//		if((unsigned char)str[i]>127) 
//			i+=2;
//		else
//			i++;
//		strnum++;
//	}
//	return strnum;
//}

///*
//	功能：从已知字符串中找想要的字符的位置
//	formfind：目标字符串
//	find：想要寻找的字符所在的字符串
//	num：find中第num的字符
//	返回值：寻找字符在formfind中的位置
//*/
//static unsigned char StrIfHaveStr(const uint8_t *formfind, uint8_t *find, unsigned int num)
//{
//	unsigned int findLen = mystrlen(find);
//	unsigned int formfindlen = mystrlen(formfind);
//	unsigned int i=0, j=0;
//	unsigned int strnum=0, HZflag=0;
//	
//	if(num >= findLen) return 0;
//	if(formfindlen==0 || findLen==0) return 0;
//	for(strnum=0, i=0; strnum<num+1; )
//	{
//		if((unsigned char)find[i]>127){
//			i+=2;
//			if(strnum==num) HZflag=1;
//		}
//		else{
//			i++;	
//		}
//		strnum++;
//	}
//	for(strnum=0, j=0; strnum<formfindlen; )
//	{
//		strnum++;
//		if(HZflag)
//		{
//			if(formfind[j]==find[i-2] && formfind[j+1]==find[i-1]) 
//				return strnum;
//		}
//		else
//		{
//			if(formfind[j]==find[i-1])
//				return strnum;	
//		}
//		if((unsigned char)formfind[j]>127){
//			j+=2;
//		}
//		else{
//			j++;	
//		}	
//	}
//	return 0;
//}
//// 汉字显示，target为NULL在屏幕写
//static void MenuHZ16x16Str(menu_area *target, int16_t x, int16_t y, uint8_t *s_dat)
//{
//	unsigned char i0=0, s_datlen = HZStrLen(s_dat);
//	unsigned char j0=0;
//	
//	uint8_t i,j;
//	
//	for(i0=0; i0<s_datlen; i0++)
//	{
//		j0=StrIfHaveStr(HZK16x16index, s_dat, i0);
//		if(j0 == 0) {
//			continue; // 没有找到
//		}

//		for(j=0; j<16; j++) //高度
//		{
//			for(i=0; i<16; i++) //宽度
//			{
//				if((HZK16x16[(j0-1)*2 + j/8][i]) & (0x01<<(j%8))){
//					if(target == NULL) write_point(x+i, y+j, 1);
//					else               MenuSetPoint(target,x+i, y+j, 1);
//				}
//				else {
//					if(GRAPHICSSHOWMANNER == GraphicsCover) {
//						if(target == NULL) write_point(x+i, y+j, 0);
//						else               MenuSetPoint(target,x+i, y+j, 0);
//					}
//				}
//			}
//		}
//	}
//}

/*
	功能：单个汉字在字符串中滤去ascii的位置
*/
static unsigned int HzInStr(const uint8_t *formfind, const uint8_t *find) 
{
	unsigned int fontnum =  mystrlen(formfind);
	unsigned int i=0;
	unsigned int fontn = 0;
	
	if(fontnum<2 || mystrlen(find)!=2 || find[0]<=127) {
		return 0;
	}
	
	for(i=0; i<fontnum-1; i++)
	{
		if(formfind[i] > 127) {
			fontn++;
			if(formfind[++i]==find[1] && formfind[i-1]==find[0]) {
				return fontn;
			}
		}
	}
	return 0;
}
// 单个汉字显示，target为NULL在屏幕写
static void MenuHZ16x16(menu_area *target, int16_t x, int16_t y, uint8_t *s_dat)
{
	unsigned int j0=0;
	unsigned char i,j;
	
	j0=HzInStr(HZK16x16index, s_dat);
	if(j0 == 0) {
		return; // 没有找到
	}
	
	for(j=0; j<16; j++) //高度
	{
		for(i=0; i<16; i++) //宽度
		{
			if((HZK16x16[(j0-1)*2 + j/8][i]) & (0x01<<(j%8))){
				if(target == NULL) write_point(x+i, y+j, 1);
				else               MenuSetPoint(target,x+i, y+j, 1);
			}
			else {
				if(GRAPHICSSHOWMANNER == GraphicsCover) {
					if(target == NULL) write_point(x+i, y+j, 0);
					else               MenuSetPoint(target,x+i, y+j, 0);
				}
			}
		}
	}
}

// 汉字 Asc 混合显示，target为NULL在屏幕写
void MenuHzAndAsc(menu_area *target, int16_t x, int16_t y, const uint8_t *s_dat)
{
	unsigned int s_datlen = mystrlen(s_dat);
	unsigned int i=0;
	unsigned char HZShowBuf[] = {0,0,0}; // 必须三位，以0结尾
	uint8_t offset = font_SizeInf[1] < 16 ? 0 : font_SizeInf[1] - 16; //上下偏移量
	
	for(i=0; i<s_datlen; )
	{
		if(s_dat[i] > 127) //汉字
		{
			HZShowBuf[0]=s_dat[i];
			HZShowBuf[1]=s_dat[i+1];
			MenuHZ16x16(target, x, y + offset, HZShowBuf);
			x+=16; // 坐标右移 由汉字宽度决定
			i+=2;  // 字符串位置标志右移，汉字固定为2
		}
		else   //Asc 
		{
			MenuShowAsc(target, x, y + offset + (16 - font_SizeInf[1]), s_dat[i]);
			x+=font_SizeInf[0]; //坐标右移 由Asc宽度决定
			i++;  //字符串位置标志右移，Asc固定为1
		}
	}
}

/*
	功能：中文显示文本

target：菜单指针
x：文本的左上角x坐标
y：文本的左上角y坐标
ascw：显示宽度 
str：显示的数据串
注意：数据\n为回车，其它未识别数据显示为空格，行间隔以最大的字体高度执行！！
*/
void CharacterTextC(menu_area *target, int16_t x, int16_t y, uint16_t ascw, const uint8_t *str)
{
	uint8_t showbuf[3] = {0, 0, 0};
	uint16_t strnum = 0;
	uint16_t showx=0, showy=0;
	const uint8_t *s = str;
	uint8_t offset = font_SizeInf[1] < 16 ? 16 : font_SizeInf[1]; //文本上下偏移量
	uint16_t num = 0;
	
	while(*s++) {
		num++;
	}

	if(ascw == 0) {
		ascw = -1;
	}
	
	while(num)
	{
		num--;
		showbuf[0] = str[strnum];
		if(showbuf[0] > 0x7f) { //判断是否为中文
			showbuf[1] = str[strnum + 1];
			strnum += 2;
			num--;
		}
		else {
			strnum += 1;
		}
		if(showbuf[0] == '\n') {
			showx = 0;
			showy++;
			continue;
		}
		
		if(showbuf[0] > 0x7f) { //中文显示
			MenuHZ16x16(target,  x + showx,  y + showy*offset, showbuf);
			showbuf[1] = 0;
			showx += 16;
		}
		else { //英文显示
			MenuShowAscStr(target, x + showx, y + showy*offset + (16 - font_SizeInf[1]), showbuf);
			showx += font_SizeInf[0];
		}
		if(showx >= ascw) { //宽度超出
			showx = 0;
			showy++;
		}
	}
}



// ----------------- 串 口 ------------------

static uint8_t MenuSerialBuf[MenuSerialBufSzie] = {0}; //串口缓冲区
uint16_t MenuSerialRxNum = 0; //接收数量，最大为MenuSerialBufSzie
static uint16_t MenuSerialShowPPos[2] = {0, 0}; //最后一个显示字符的下一个坐标
const uint16_t windowsszie = MENUSERIALWINDOWWIDTH*MENUSERIALWINDOWHIGH; //窗口大小

static uint16_t SerialstartShow = 0; //显示起始位置
static uint16_t MenuSerialShowP = 0; //显示截止位置

/*
	功能：显示串口文本

target：菜单指针
x：文本的左上角x坐标
y：文本的左上角y坐标
ascw：显示宽度，<以字符位单位>
asch：显示高度，<以字符位单位>

注意：未识别数据显示为空格，不支持中文显示！！
*/

void SerialCharacterText(menu_area *target, int16_t x, int16_t y)
{
	uint8_t showbuf[2] = {0, 0};
	uint16_t strnum = SerialstartShow;
	uint8_t showx=0, showy=0;
	uint8_t *str = MenuSerialBuf; //显示缓冲区
	uint16_t num = SerialstartShow; 
	
	while(num != MenuSerialShowP)
	{
		if(++num >= MenuSerialBufSzie) {
			num = 0;
		}
		showbuf[0] = str[strnum];
		if(++strnum >= MenuSerialBufSzie) {
			strnum = 0;
		}
		if(showbuf[0] == '\0') showbuf[0] = ' ';
		if(showbuf[0] == '\n') {
			showx = 0;
			if(++showy >= MENUSERIALWINDOWHIGH) {
				break;
			}
			continue;
		}
		MenuShowAscStr(target,  x + showx*font_SizeInf[0],  y + showy*font_SizeInf[1], showbuf);
		
		if(++showx >= MENUSERIALWINDOWWIDTH) {
			showx = 0;
			if(++showy >= MENUSERIALWINDOWHIGH) {
				break;
			}
		}
	}
}
//清空串口显示
void ClearnSerialShowBuf(void)
{
	MenuSerialRxNum = 0;
	MenuSerialShowP = 0;
	SerialstartShow = 0;
	MenuSerialShowPPos[0] = 0;
	MenuSerialShowPPos[1] = 0;
}
/*
	功能：起始刷新后移行
	n：后移的行数
*/
static void SerialstartN(uint8_t n)
{
	uint8_t i = 0;

	while(n--) {
		i = MENUSERIALWINDOWWIDTH;
		while(i--) {
			if(MenuSerialBuf[SerialstartShow] == '\n') {
				SerialstartShow++;
				if(SerialstartShow >= MenuSerialBufSzie) {
					SerialstartShow = SerialstartShow - MenuSerialBufSzie;
				}
				break;
			}
			SerialstartShow++;
			if(SerialstartShow >= MenuSerialBufSzie) {
				SerialstartShow = 0;
			}
		}
	}
}
//串口填充
void MenuPaddingSerialBuf(uint8_t dat) 
{
	uint8_t flog = 0;
	
	MenuSerialBuf[MenuSerialShowP] = dat;
	if(++MenuSerialRxNum >= MenuSerialBufSzie) {
		MenuSerialRxNum = MenuSerialBufSzie;
	}
	if(++MenuSerialShowP >= MenuSerialBufSzie) { //显示截止位置
		MenuSerialShowP = 0;
	}
	
	//接收\n字符
	if(dat == '\n') {
		if(MenuSerialShowPPos[0] == MENUSERIALWINDOWWIDTH) { //同时一行填充满
			MenuSerialShowPPos[1]+=2;
		}
		else { //正常行移
			MenuSerialShowPPos[1]+=1;
		}
		MenuSerialShowPPos[0] = 0;
		
		if(MenuSerialShowPPos[1] >= MENUSERIALWINDOWHIGH) { //超出行显示边界
			SerialstartN(MenuSerialShowPPos[1] - MENUSERIALWINDOWHIGH + 1); //移动起始显示行
			MenuSerialShowPPos[1] = MENUSERIALWINDOWHIGH-1;
		}
		return;
	}
	
	//除\n外的其它字符的接收
	if(MenuSerialShowPPos[0] >= MENUSERIALWINDOWWIDTH) {
		if(MenuSerialShowPPos[1] == MENUSERIALWINDOWHIGH-1) {
			if(MenuSerialShowPPos[0] == MENUSERIALWINDOWWIDTH) {
					flog = 1;
			}
		}
		else {
			if(++MenuSerialShowPPos[1] >= MENUSERIALWINDOWHIGH) {
				MenuSerialShowPPos[1] -= 1;
				flog = 1;
			}
		}
		if(flog) {
			flog = 0;
			SerialstartN(1);
		}
		MenuSerialShowPPos[0] = 0;
	}
	MenuSerialShowPPos[0]++;
}





