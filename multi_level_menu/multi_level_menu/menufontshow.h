#ifndef _menufontshow_h_
#define _menufontshow_h_

#include "menu.h"
#include "menuconfig.h"


extern const uint8_t *font_SizeInf; //字体大小

//设置英文显示字体
void SetFont(const uint8_t *xfont,const uint8_t *xfont_SizeInf);



//功能：在菜单中写Asc字符，target为NULL在屏幕写
void MenuShowAsc(menu_area *target, int16_t x, int16_t y, uint8_t asc);


//功能：在菜单中写Asc字符串，target为NULL在屏幕写
void MenuShowAscStr(menu_area *target, int16_t x, int16_t y, const uint8_t *str);
// 汉字 Asc 混合显示，target为NULL在屏幕写
void MenuHzAndAsc(menu_area *target, int16_t x, int16_t y, const uint8_t *s_dat);


// 文本显示 <仅支持英文> 宽高字节为单位
void CharacterText(menu_area *target, int16_t x, int16_t y, uint8_t ascw, uint8_t asch, const uint8_t *str, uint16_t num);
// 文本显示 <中英文混合显示> 宽高字节为单位 行间隔以最大的字体高度执行！！
void CharacterTextC(menu_area *target, int16_t x, int16_t y, uint8_t ascw, uint8_t asch,const uint8_t *str, uint16_t num);



//功能：在菜单中写Asc数字，target为NULL在屏幕写
void MenuShowNum(menu_area *target, int16_t x, int16_t y, uint8_t len, uint32_t num);


// -------------- 串 口 --------------


#define MENUSERIALWINDOWWIDTH   10 //窗口宽度，字节为单位
#define MENUSERIALWINDOWHIGH   3 //窗口高度，字节为单位

#define MenuSerialBufSzie  (MENUSERIALWINDOWWIDTH * MENUSERIALWINDOWHIGH + 1) //串口缓冲区字节大小


extern uint16_t MenuSerialRxNum; //接收数量，最大为MenuSerialBufSzie


//串口填充
void MenuPaddingSerialBuf(uint8_t dat);
// 串口文本显示<仅支持英文>
void SerialCharacterText(menu_area *target, int16_t x, int16_t y);
//清空串口显示
void ClearnSerialShowBuf(void);


#endif



