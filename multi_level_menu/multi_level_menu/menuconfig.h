#ifndef _menuconfig_h_
#define _menuconfig_h_



#include "main.h" //HAL函数库
#include "spi.h" //HAL函数库

extern uint32_t adcbuf[];

//功能：初始化菜单列表
void MakeMenu(void);

#endif

