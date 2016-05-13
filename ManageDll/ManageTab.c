#include <ansi_c.h>
#include <userint.h>
#include <stdbool.h>
#include "ManageTab.h"
#include "ManageDll.h"

static int MovePositionTabOld(int panel, int init)
{
	static int gXTab = -1;
	static int gYTab = -1;
	
	if (init == 1)
	{
		int index, tabPage;
		GetActiveTabPage(panel, PANEL_LIST_TAB, &index);
		GetPanelHandleFromTabPage(panel, PANEL_LIST_TAB, index, &tabPage);
	
	    GetPanelAttribute (tabPage, ATTR_WIDTH, &gXTab);
	    GetPanelAttribute (tabPage, ATTR_HEIGHT, &gYTab);

		//char buff[256];
		//memset(buff, 0x0, sizeof(buff));
		//GetTabPageAttribute(panel, PANEL_LIST_TAB, index, ATTR_LABEL_TEXT, buff);
	}
	else
	{
		int begin, end;
		static bool bMoveOnly = false;
		if (bMoveOnly)
		{
			bMoveOnly = false;
			
			begin = 0;
			if (GetNumTabPages(panel, PANEL_LIST_TAB, &end) < 0)
				return 0;
		}
		else
		{
			GetActiveTabPage(panel, PANEL_LIST_TAB, &begin);
			end = begin + 1;
		}
		
		int height=0, width=0;
		for (int index = begin; index < end; index ++)
	    {
			int tabPage;
			GetPanelHandleFromTabPage(panel, PANEL_LIST_TAB, index, &tabPage);
			
		    GetPanelAttribute (tabPage, ATTR_WIDTH, &width);
		    GetPanelAttribute (tabPage, ATTR_HEIGHT, &height);
		
			int nTabSplitH = -1, nTabSplitV = -1;
			if (index == 0)
			{
				nTabSplitH = TAB_DEVICE_SPLITTER_H_1;
				nTabSplitV = TAB_DEVICE_SPLITTER_V_1;
			}
			else if (index == 1)
			{
				nTabSplitH = TAB_LIB_SPLITTER_H_2;
				nTabSplitV = TAB_LIB_SPLITTER_V_2;
			}
			else if (index == 2)
			{
				nTabSplitH = TAB_UPDATE_SPLITTER_H_3;
				nTabSplitV = TAB_UPDATE_SPLITTER_V_3;
			}
			else
				return 0;
	
			OperateSplitter (tabPage, nTabSplitH, height - gYTab);
			OperateSplitter (tabPage, nTabSplitV, width - gXTab);
		}
        gXTab = width;
        gYTab = height;
	}
	return 1;
}
static int MovePositionTab(int panel, int flag)
{
	static int gXTab[16], gYTab[16];
	
	int index, tabPage;
	GetActiveTabPage(panel, PANEL_LIST_TAB, &index);
	GetPanelHandleFromTabPage(panel, PANEL_LIST_TAB, index, &tabPage);
	if (flag == -1)
	{
		int cnt = 0;
		if (GetNumTabPages(panel, PANEL_LIST_TAB, &cnt) < 0)
			return 0;
		
		int x, y;
	    GetPanelAttribute (tabPage, ATTR_WIDTH, &x);
	    GetPanelAttribute (tabPage, ATTR_HEIGHT, &y);
		for (int n = 0; n < cnt; n ++)
		{
			gXTab[n] = x;
			gYTab[n] = y;
		}
		
		//char buff[256];
		//memset(buff, 0x0, sizeof(buff));
		//GetTabPageAttribute(panel, PANEL_LIST_TAB, index, ATTR_LABEL_TEXT, buff);
	}
	else
	{
		int height=0, width=0;
	    {
		    GetPanelAttribute (tabPage, ATTR_WIDTH, &width);
		    GetPanelAttribute (tabPage, ATTR_HEIGHT, &height);
		
			int nTabSplitH = -1, nTabSplitV = -1;
			if (index == 0)
			{
				nTabSplitH = TAB_DEVICE_SPLITTER_H_1;
				nTabSplitV = TAB_DEVICE_SPLITTER_V_1;
			}
			else if (index == 1)
			{
				nTabSplitH = TAB_LIB_SPLITTER_H_2;
				nTabSplitV = TAB_LIB_SPLITTER_V_2;
			}
			else if (index == 2)
			{
				nTabSplitH = TAB_UPDATE_SPLITTER_H_3;
				nTabSplitV = TAB_UPDATE_SPLITTER_V_3;
			}
			else
				return 0;
	
			OperateSplitter (tabPage, nTabSplitH, height - gYTab[index]);
			OperateSplitter (tabPage, nTabSplitV, width - gXTab[index]);
		}
        gXTab[index] = width;
        gYTab[index] = height;
	}
	return 1;
}

void MovePosition(int panel, int init)	   //定义 全局
{
	static int gX = -1;
	static int gY = -1;
	
	if (init == 1)		//flag == 1 means initialize.
	{
	    GetPanelAttribute (panel, ATTR_WIDTH, &gX);
	    GetPanelAttribute (panel, ATTR_HEIGHT, &gY);

		MovePositionTab(panel, -1);		//init
	}
	else if (init == 0) //means main resize
	{
	    int height, width;

        GetPanelAttribute (panel, ATTR_WIDTH, &width);
        GetPanelAttribute (panel, ATTR_HEIGHT, &height);

		OperateSplitter (panel, PANEL_LIST_SPLITTER_H, height - gY);
		OperateSplitter (panel, PANEL_LIST_SPLITTER_V, width - gX);  
        gX = width;
        gY = height;
	
		MovePositionTab(panel, 0);		//move
	}
	
	//MovePositionTabOld(panel, init);		//tab_ctrl init.
}

int CVICALLBACK ListPanelCB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_RIGHT_CLICK:
			break;
		case EVENT_GOT_FOCUS:
			break;
		case EVENT_LOST_FOCUS:
			break;
		case EVENT_CLOSE:
			break;
		case EVENT_PANEL_SIZING:
		case EVENT_PANEL_SIZE:
			MovePosition(panel, 0);
			break;
		case EVENT_END_TASK:
			break;
	}
	return 0;
}

int CVICALLBACK TabCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	//int oldindex, index;
	switch (event)
	{
		case EVENT_RIGHT_CLICK:
			break;
		case EVENT_GOT_FOCUS:
			break;
		case EVENT_ACTIVE_TAB_CHANGE:
			//oldindex = eventData1;
			//index = eventData2;
			MovePositionTab(panel, 0);
			break;
	}
	return 0;
}
