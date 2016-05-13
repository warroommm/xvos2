//==============================================================================
//
// Title:		TreeMenu.h
// Purpose:		A short description of the interface.
//
// Created on:	2015/11/25 at 14:10:54 by cwj.
// Copyright:	hy. All Rights Reserved.
//
//==============================================================================

#ifndef __TreeMenu_H__
#define __TreeMenu_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include "cvidef.h"

int InitMenu(int panel);
int SelectItem();

static int indexClosed;
static int indexOpen1;
static int indexOpen2;
static int indexOpen3;
static void InsertImagesToTreeItem(int index,int indexOpen);
static void AddTreeListPics();

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TreeMenu_H__ */

