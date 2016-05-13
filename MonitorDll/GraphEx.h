//==============================================================================
//
// Title:		GraphEx.h
// Purpose:		A short description of the interface.
//
// Created on:	2015/11/12 at 9:13:10 by cwj.
// Copyright:	hy. All Rights Reserved.
//
//==============================================================================

#ifndef __GraphEx_H__
#define __GraphEx_H__

#ifdef __cplusplus
    extern "C" {
#endif


#include "cvidef.h"

//type 0:click, 1:move, 2:click_up.
extern int GraphCB_ex (int panel, int control, int posX, int posY, int type);

extern void GraphClickFlag(int panel, int control, int newValue);   //强制设置click事件，1:click， 0:点 击 失 效
extern int ctrl_cursor(int panel, int control, int posX, int posY, int type);
//extern void Test(int start);
#ifdef __cplusplus
    }
#endif

#endif  /* ndef __GraphEx_H__ */
