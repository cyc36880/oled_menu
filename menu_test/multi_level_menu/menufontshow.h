#ifndef _menufontshow_h_
#define _menufontshow_h_

#include "menu.h"
#include "menuconfig.h"

extern const uint8_t *font_SizeInf; //字体大小

// 设置英文显示字体
void SetFont(const uint8_t *xfont);


// 功能：在菜单中写Asc字符串，target为NULL在屏幕写
void MenuShowAscStr(menu_area *target, int16_t x, int16_t y, const uint8_t *str);
// 汉字 Asc 混合显示，target为NULL在屏幕写
void MenuHzAndAsc(menu_area *target, int16_t x, int16_t y, const uint8_t *s_dat);


// 文本显示 <仅支持英文>   <ascw 字符个数！！>
void CharacterText(menu_area *target, int16_t x, int16_t y, uint16_t ascw, const uint8_t *str);
// 文本显示 <中英文混合显示> 行间隔以最大的字体高度执行！！
void CharacterTextC(menu_area *target, int16_t x, int16_t y, uint16_t ascw, const uint8_t *str);


// 功能：在菜单中写Asc数字，target为NULL在屏幕写
void MenuShowNum(menu_area *target, int16_t x, int16_t y, uint8_t len, uint32_t num);


/*
	功能：如使用printf向屏幕打印字符
	target：目标菜单
	mod：0 内部调用CharacterText，不支持中文，但输出紧凑
			 1 内部调用CharacterTextC，支持中文，但行间可能有空隙
	x,y: 坐标
	formate与..: 与printf相同
	ret：输出的字符个数
	注意：内部申请固定内存100字节，不要输出太长字符串
*/
uint16_t m_printf(menu_area *target, uint8_t mod, int16_t x, int16_t y, const char *format, ...);


// -------------- 串 口 --------------


#define MENUSERIALWINDOWWIDTH   10 //窗口宽度，字节为单位 max:255
#define MENUSERIALWINDOWHIGH   4 //窗口高度，字节为单位   max:255

#define MenuSerialBufSzie  (MENUSERIALWINDOWWIDTH * MENUSERIALWINDOWHIGH + 1) //串口缓冲区字节大小


extern uint16_t MenuSerialRxNum; //接收数量，最大为MenuSerialBufSzie


//串口填充
void MenuPaddingSerialBuf(uint8_t dat);
// 串口文本显示<仅支持英文>
void SerialCharacterText(menu_area *target, int16_t x, int16_t y);
//清空串口显示
void ClearnSerialShowBuf(void);

/*
	串口字符串匹配
	@ret 1:含由匹配字符 0：匹配失败
*/
uint8_t SerStrMatching(const uint8_t *str);
	


#endif



