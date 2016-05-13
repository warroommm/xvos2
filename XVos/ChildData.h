//==============================================================================
//
// Title:		ChildData.h
// Purpose:		A short description of the interface.
//
// Created on:	2016/2/2 at 13:05:20 by cwj.
// Copyright:	hy. All Rights Reserved.
//
//==============================================================================

#ifndef __ChildData_H__
#define __ChildData_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include "cvidef.h"


typedef struct _TabData{
	char 	szName[256];
	char 	szID[256];
	char 	szPID[256];
	int 	level;
	int		tick;
}TabData;

int InitData(int dele);
int GetBtnName(const char* pId, char* pName);		//通过编码，找名称，
int GetBtnPID(const char* pId, char* pPID);			//通过编码，找父级编码
int CreateBtnData(int panel, const char* pParentId, int* gBtn);
int CountofBtnData(const char* pParentId);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ChildData_H__ */
