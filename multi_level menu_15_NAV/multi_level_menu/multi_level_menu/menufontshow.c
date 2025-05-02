#include "menufontshow.h"
#include "Graphicalfunctions.h"
#include "oledfont.h"

static const uint8_t *font = F8X16+2;
const uint8_t *font_SizeInf = F8X16;
void SetFont(const uint8_t *xfont)
{
	font = xfont+2;
	font_SizeInf = xfont;
}
void MenuShowAsc(menu_area *target,int16_t x, int16_t y, uint8_t asc)
{
	uint8_t c=0;
	uint8_t h,w;
	bool w_b=0;
	uint8_t wight = font_SizeInf[0];
	uint8_t high  = font_SizeInf[1];
	
	if(asc > '~' || asc < ' ') asc = ' ';
	c=asc-' ';   
	for(h=0; h<high; h++)
	{
		for(w=0; w<wight; w++) 
		{
			w_b = ( font[c*wight*DIVIDEUP(high) + w+wight*(h/8)] >> (h%8) ) & 1;
			if(w_b) {
				if(target == NULL) write_point(w+x, h+y, 1);
				else               MenuSetPoint(target, w+x, h+y, 1);
			}
			else {
				if(GRAPHICSSHOWMANNER == GraphicsCover) {
					if(target == NULL) write_point(w+x, h+y, 0);
					else               MenuSetPoint(target, w+x, h+y, 0);
				}
			}
		}
	}
}
static uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}
void MenuShowNum(menu_area *target, int16_t x, int16_t y, uint8_t len, uint32_t num)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;	
	uint32_t nump = num;
	
	if(len == 0) { 
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
void MenuShowAscStr(menu_area *target, int16_t x, int16_t y, const uint8_t *str)
{
	while(*str != '\0')
	{
		MenuShowAsc(target, x, y, *str);
		str++;
		x+=font_SizeInf[0];
	}
}
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
static unsigned int mystrlen(const uint8_t *str)
{
	unsigned int i=0;
	for(i=0; str[i] != '\0'; i++);
	return i;
}
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
static void MenuHZ16x16(menu_area *target, int16_t x, int16_t y, uint8_t *s_dat)
{
	unsigned int j0=0;
	unsigned char i,j;
	
	j0=HzInStr(HZK16x16index, s_dat);
	if(j0 == 0) {
		return; 
	}
	for(j=0; j<16; j++) 
	{
		for(i=0; i<16; i++) 
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
void MenuHzAndAsc(menu_area *target, int16_t x, int16_t y, const uint8_t *s_dat)
{
	unsigned int s_datlen = mystrlen(s_dat);
	unsigned int i=0;
	unsigned char HZShowBuf[] = {0,0,0}; 
	uint8_t offset = font_SizeInf[1] < 16 ? 0 : font_SizeInf[1] - 16; 
	
	for(i=0; i<s_datlen; )
	{
		if(s_dat[i] > 127) 
		{
			HZShowBuf[0]=s_dat[i];
			HZShowBuf[1]=s_dat[i+1];
			MenuHZ16x16(target, x, y + offset, HZShowBuf);
			x+=16; 
			i+=2;  
		}
		else   
		{
			MenuShowAsc(target, x, y + offset + (16 - font_SizeInf[1]), s_dat[i]);
			x+=font_SizeInf[0]; 
			i++; 
		}
	}
}
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
		if(showbuf[0] > 0x7f) { 
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
		if(showbuf[0] > 0x7f) { 
			MenuHZ16x16(target,  x + showx,  y + showy*offset, showbuf);
			showbuf[1] = 0;
			showx += 16;
		}
		else { 
			MenuShowAscStr(target, x + showx, y + showy*offset + (16 - font_SizeInf[1]), showbuf);
			showx += font_SizeInf[0];
		}
		if(showx >= ascw) { 
			showx = 0;
			showy++;
		}
	}
}
static uint8_t MenuSerialBuf[MenuSerialBufSzie] = {0}; 
uint16_t MenuSerialRxNum = 0;
static uint16_t MenuSerialShowPPos[2] = {0, 0};
static uint16_t SerialstartShow = 0; 
static uint16_t MenuSerialShowP = 0;
void SerialCharacterText(menu_area *target, int16_t x, int16_t y)
{
	uint8_t showbuf[2] = {0, 0};
	uint16_t strnum = SerialstartShow;
	uint8_t showx=0, showy=0;
	uint8_t *str = MenuSerialBuf; 
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
void ClearnSerialShowBuf(void)
{
	MenuSerialRxNum = 0;
	MenuSerialShowP = 0;
	SerialstartShow = 0;
	MenuSerialShowPPos[0] = 0;
	MenuSerialShowPPos[1] = 0;
}
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
void MenuPaddingSerialBuf(uint8_t dat) 
{
	uint8_t flog = 0;
	
	MenuSerialBuf[MenuSerialShowP] = dat;
	if(++MenuSerialRxNum >= MenuSerialBufSzie) {
		MenuSerialRxNum = MenuSerialBufSzie;
	}
	if(++MenuSerialShowP >= MenuSerialBufSzie) { 
		MenuSerialShowP = 0;
	}
	if(dat == '\n') {
		if(MenuSerialShowPPos[0] == MENUSERIALWINDOWWIDTH) { 
			MenuSerialShowPPos[1]+=2;
		}
		else {
			MenuSerialShowPPos[1]+=1;
		}
		MenuSerialShowPPos[0] = 0;
		
		if(MenuSerialShowPPos[1] >= MENUSERIALWINDOWHIGH) { 
			SerialstartN(MenuSerialShowPPos[1] - MENUSERIALWINDOWHIGH + 1);
			MenuSerialShowPPos[1] = MENUSERIALWINDOWHIGH-1;
		}
		return;
	}
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
uint8_t SerStrMatching(const uint8_t *str)
{
	uint16_t strstart = SerialstartShow;
	uint16_t strend = MenuSerialShowP;
	uint16_t mystrtotal;
	
	uint8_t matnum = 0;
	
	mystrtotal = mystrlen(str);

	if(mystrtotal > MenuSerialRxNum) {
		
		return 0;
	}
	for(; strstart != strend; ) {
		if(MenuSerialBuf[strstart] == str[matnum]) {
			matnum++;
		}
		else {
			matnum = 0;
		}
		if(matnum == mystrtotal) {
			return 1;
		}
		
		if(++strstart == MenuSerialBufSzie) {
			strstart = 0;
		}
	}
	return 0;
}




