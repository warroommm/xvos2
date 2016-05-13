#include <userint.h>
#include <ansi_c.h>

#include "Thd.h"		//resource file
//==============================================================================
//
// Title:		GraphEx.c
// Purpose:		A short description of the implementation.
//
// Created on:	2015/11/12 at 9:13:10 by cwj.
// Copyright:	hy. All Rights Reserved.
//
//==============================================================================
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

static int gFlag = 0;		//标志鼠标是否在有效区域按下.1按下状态，0未按下
//static int bStart = 1;
extern int FindAnnoId(int control);

void RestoreGraph(int panel, int control, double xMin, double yMin, double xMax, double yMax)
{
	double x1, x2, y1, y2;
	if (yMin == 0 && yMax == 0)
	{
		x1 = 0;
		GetCtrlVal(panel, PANEL_NUMERIC, &x2);
		GetCtrlVal(panel, PANEL_NUMERIC_Y1, &y1);
		GetCtrlVal(panel, PANEL_NUMERIC_Y2, &y2);
	}
	else
	{
		x1 = xMin;
		x2 = xMax;
		y1 = yMin;
		y2 = yMax;
	}
	
	if (y1 > y2)
		y1 = 0;
	SetAxisScalingMode(panel, control, VAL_LEFT_YAXIS, VAL_MANUAL, y1, y2);
	SetAxisScalingMode(panel, control, VAL_BOTTOM_XAXIS, VAL_MANUAL, x1, x2);
}

int UpdateXY(int panel, int control, int* inx, int* iny)
{
	int 	plotL, plotT, plotX, plotY;					//plot
	GetCtrlAttribute(panel, control, ATTR_PLOT_AREA_TOP, &plotT);
	GetCtrlAttribute(panel, control, ATTR_PLOT_AREA_LEFT, &plotL);
	GetCtrlAttribute(panel, control, ATTR_PLOT_AREA_WIDTH, &plotX);
	GetCtrlAttribute(panel, control, ATTR_PLOT_AREA_HEIGHT, &plotY);
	
	int 	grapL, grapT, grapX, grapY;					//graph
	GetCtrlAttribute(panel, control, ATTR_TOP, &grapT);
	GetCtrlAttribute(panel, control, ATTR_LEFT, &grapL);
	GetCtrlAttribute(panel, control, ATTR_WIDTH, &grapX);
	GetCtrlAttribute(panel, control, ATTR_HEIGHT, &grapY);
	
	int 	xdX, xdY;
	GetRelativeMouseState(panel, control, &xdX, &xdY, NULL, NULL, NULL);
	
	*inx = min(xdX, (plotL+plotX));
	*iny = min(xdY, (plotT+plotY));
	*inx = max(*inx, plotL);
	*iny = max(*iny, plotT);
	
	*inx += grapL;
	*iny += grapT;
	
	return 0;
}

static int posGPlot(int add)
{
	const int cnt = 16;
	static int loop[cnt]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	if (add > -1)
	{
		for (int n = 0; n < cnt; n ++)
		{
			if (loop[n] == -1)
			{
				loop[n] = add;
				return n;
			}
		}
		return -1;	//ignore
	}
	else
	{
		int dele = loop[0];
		loop[cnt-1] = -1;
		for (int n = 0; n < cnt-1; n ++)
		{
			loop[n] = loop[n+1];
		}
		return dele;
	}
}
static void DeleGPlot(int panel, int control)
{
	char szCaption[128];

	int plotId = posGPlot(-1);
	if (plotId >= 0)
	{
		int del = DeleteGraphPlot (panel, control, plotId, VAL_IMMEDIATE_DRAW);
		sprintf(szCaption, "Del id:%d, ret:%d", plotId, del);
	}
	else
		sprintf(szCaption, "Del:error!!!");

	//SetAnnotationAttribute(panel, control, 2, ATTR_ANNOTATION_CAPTION, szCaption);
}
static void AddGPlot(int panel, int control, double x1, double y1, double x2, double y2)
{
	char szCaption[128];
	
	int plotId = PlotRectangle(panel, control, x1, y1, x2, y2, VAL_DK_CYAN, VAL_TRANSPARENT);
	if (plotId >= 0)
	{
		int ret = posGPlot(plotId);
		sprintf(szCaption, "add id:%d, ret:%d", plotId, ret);
	}
	else
	{
		sprintf(szCaption, "plot error!!");
	}
}
int ctrl_cursor(int panel, int control, int posX, int posY, int type)
{//添加光标的功能
	double cursorX=0, cursorY=0;
	
	if (type == 9)
	{
		int activeCursor;
		GetActiveGraphCursor (panel, control, &activeCursor);
    	GetGraphCursor (panel, control, activeCursor, &cursorX, &cursorY);
	}
	else
	{
		if (0)
		{
			UpdateXY(panel, control, &posX, &posY);
	
			int r = GetGraphCoordsFromPoint(panel, control, MakePoint(posX, posY), &cursorX, &cursorY);
			if (r == 0)
				return 0;
		}
		else
		{
			int activeCursor;
			GetActiveGraphCursor (panel, control, &activeCursor);
	    	GetGraphCursor (panel, control, activeCursor, &cursorX, &cursorY);
			//SetGraphCursor(panel, control, activeCursor, cursorX, cursorY);
			//SetActiveGraphCursor(panel, control, activeCursor);
			//GetGraph
		}
	}
	
	int AnnoId = FindAnnoId(control);
	if (AnnoId > 0)
	{
		char szCaption[128];
		sprintf(szCaption, "x:%.2f    y:%.2f (%d)", cursorX, cursorY, type);
		SetAnnotationAttribute(panel, control, AnnoId, ATTR_ANNOTATION_CAPTION, szCaption);
	}
	return AnnoId;
}

int GraphCB_ex(int panel, int control, int posX, int posY, int type)
{
	static int 		beginPixelX = 0, beginPixelY = 0;
	static double	beginX = -0.1, beginY = -0.1;
	static double 	endX = -0.1, endY = -0.1;
	if (type == 0)	//click
	{
		beginPixelX = posX;
		beginPixelY = posY;
		
		int ret = GetGraphCoordsFromPoint(panel, control, MakePoint(posX, posY), &beginX, &beginY);	//这函数 奇怪的是必须传入面板坐标才转出正确的 graph坐标值
		if (ret == 0)
		{
			beginX = beginY = -0.1;
		}
		
		gFlag = ret ? 1 : 0;					//标 识 点 击
		
		//////////////////////////////////////////////////////begin
		/*{//添加光标的功能
			int posCursorX = posX;
			int posCursorY = posY;
			UpdateXY(panel, control, &posCursorX, &posCursorY);
			
			double cursorX=endX, cursorY = endY;
			int r = GetGraphCoordsFromPoint(panel, control, MakePoint(posCursorX, posCursorY), &cursorX, &cursorY);
			int AnnoId = FindAnnoId(control);
			if (AnnoId > 0)
			{
				char szCaption[128];
				sprintf(szCaption, "x:%.2f_%.2f    y:%.2f_%.2f", endX, cursorX, endY, cursorY);
				SetAnnotationAttribute(panel, control, AnnoId, ATTR_ANNOTATION_CAPTION, szCaption);
			}
		}*/
		//////////////////////////////////////////////////////end
	}
	else if (type == 2)  //click up
	{
		if (gFlag)
		{
			gFlag = 0;		//弹起点击
			
			int xSpan = abs(posX - beginPixelX);
			int ySpan = abs(posY - beginPixelY);
			if (xSpan < 4 || ySpan < 4 || (xSpan < 8 && ySpan < 8))
			{
				DeleGPlot(panel, control);
				return 0;
			}
			
			if (endX < beginX)
			{
				double T = beginX;
				beginX = endX;
				endX = T;
			}
			if (endY < beginY)
			{
				double T = beginY;
				beginY = endY;
				endY = T;
			}

			DeleGPlot(panel, control);
			 RestoreGraph(panel, control, beginX, beginY, endX, endY);
		}
	}
	else if (type == 1)  //move
	{
		//if (gFlag)  //判断值移至下面。begin
		{
			static int moveX=0, moveY=0;
			int ret = GetGraphCoordsFromPoint(panel, control, MakePoint(posX, posY), &endX, &endY);
			if (ret == 0)
			{
				UpdateXY(panel, control, &posX, &posY);
				ret = GetGraphCoordsFromPoint(panel, control, MakePoint(posX, posY), &endX, &endY);
			}
			
			if (gFlag)  //判断值移至这里。end
			{
				if (ret)
				{
					moveX = posX;
					moveY = posY;
					DeleGPlot(panel, control);
					AddGPlot(panel, control, beginX, beginY, endX, endY);
				}
				else
				{
					char szCaption[128];
					sprintf(szCaption, "error!!!!!!!!!!!!!!!!!"); 
					SetAnnotationAttribute(panel, control, 2, ATTR_ANNOTATION_CAPTION, szCaption);
				}
			}
			//else
			//{
			//	ctrl_cursor(panel, control, posX, posY, type);
			//}
		}
	}
	return 0;
}

void GraphClickFlag(int panel, int control, int newValue)
{
	gFlag = newValue;
	if (newValue == 0)
		DeleGPlot(panel, control);
}
//void Test(int start)
//{
//	bStart = start;
//}
