//==============================================================================
//
// Title:		ChildData.c
// Purpose:		A short description of the implementation.
//
// Created on:	2016/2/2 at 13:05:20 by cwj.
// Copyright:	hy. All Rights Reserved.
//
//==============================================================================
#include "main.h"

#include "ChildData.h"

static TabData* pData;
static int gCNT;
int InitData(int dele)
{
	if (dele == 0)
	{
		gCNT = GetDBData((void **)&pData);
		for (int n = 0; n < gCNT; n ++)
		{
			//printf("%d,,,,  %s,\t%s,\t%s,\t%d,\t%d\n", n+1, pData[n].szName, pData[n].szID, pData[n].szPID, pData[n].level, pData[n].tick);
		}
	}
	else
	{
		free(pData);
		gCNT = 0;
	}
	return gCNT;
}
int GetBtnName(const char* pId, char* pName)
{
	if (strcmp(pId, "1") == 0)
	{
		strcpy(pName, "无");
		return 1;
	}

	for (int n = 0; n < gCNT; n ++)
	{
		if (strcmp(pData[n].szID, pId) == 0)
		{
			strcpy(pName, pData[n].szName);
			return 1;
		}
	}
	return 0;
}
int GetBtnPID(const char* pId, char* pPID)			//通过编码，找父级编码
{
	if (strcmp(pId, "1") == 0)
	{
		strcpy(pPID, "");
		return 0;
	}

	for (int n = 0; n < gCNT; n ++)
	{
		if (strcmp(pData[n].szID, pId) == 0)
		{
			strcpy(pPID, pData[n].szPID);
			return 1;
		}
	}
	return 0;
}

int CountofBtnData(const char* pParentId)
{
	int cnt = 0;
	for (int n = 0; n < gCNT; n ++)
	{
		if (strcmp(pData[n].szPID, pParentId) == 0)
		{
			cnt ++;
		}
	}
	return cnt;
}
int CreateBtnData(int panel, const char* pParentId, int* gBtn)
{
	int cnt = CountofBtnData(pParentId);
	if (cnt == 0)
		return 0;
	
	int level = 0;
	int index = 0;
	for (int n = 0; n < gCNT; n ++)
	{
		if (strcmp(pData[n].szPID, pParentId) == 0)
		{
			gBtn[index] = NewCtrl(panel, CTRL_PICTURE_COMMAND_BUTTON, pData[n].szName, index*100+60, index*80+20);
			SetCtrlAttribute(panel, gBtn[index], ATTR_LABEL_COLOR, 0x0000ff);
			SetCtrlAttribute(panel, gBtn[index], ATTR_LABEL_BGCOLOR, 0x00ff00);
			SetCtrlAttribute(panel, gBtn[index], ATTR_DISABLE_CTRL_TOOLTIP, 1);
			SetCtrlAttribute(panel, gBtn[index], ATTR_TOOLTIP_TEXT, pData[n].szID);
			//InstallCtrlCallback(panel, gBtn[index], PicBtnCB, &bid);
			//SetCtrlAttribute(panel, gBtn[bid], ATTR_VISIBLE, 0);
			level = pData[n].level;
			index ++;
		}
	}
	return level;
}


