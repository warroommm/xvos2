//-----------------------------------------------------------------------------
//This file contains the entry-point for the application.
//-----------------------------------------------------------------------------

#include "main.h"

#include "child.h"


#define SUBPANEL_WIDTH 		(653+18)
#define SUBPANEL_HEIGHT 	(437+38)
#define SUBPANEL_TOP 		28
#define SUBPANEL_LEFT 		235

#define	WM_TO_MAIN			9567   /* 主程序 响应处理 */
#define	WM_TO_SUB			9568   /* 发送给 子模块 处理 */
static intptr_t			g_hMain = 0;
static intptr_t			g_hSub = 0;
extern void ShowHideView (int nType);
int CVICALLBACK OnMain(int panel, int msg, unsigned int* wParam, unsigned int* lParam, void* callbackData);

static void MovePosition(int flag)
{
	static int gX = -1;
	static int gY = -1;
	static int gXsub = -1;
	static int gYsub = -1;
    int panel = GetMainPanel();
	int subPanel = GetChildPanel(SUB_PANEL);
	
	if (flag == 0)		//flag == 0 means initialize.
	{
	    SetSystemAttribute (ATTR_ALLOW_MISSING_CALLBACKS, 1);
	    SetSystemAttribute (ATTR_REPORT_LOAD_FAILURE, 0);

	    GetPanelAttribute (panel, ATTR_WIDTH, &gX);
	    GetPanelAttribute (panel, ATTR_HEIGHT, &gY);

	    GetPanelAttribute (subPanel, ATTR_WIDTH, &gXsub);
	    GetPanelAttribute (subPanel, ATTR_HEIGHT, &gYsub);
		
		SetCtrlAttribute (subPanel, SUB_PANEL_SPLITTER_2, ATTR_TOP, gYsub-4);
		SetCtrlAttribute (subPanel, SUB_PANEL_SPLITTER, ATTR_LEFT, gXsub-4);
		SetCtrlAttribute (subPanel, SUB_PANEL_CANVAS, ATTR_LEFT, 0);
		SetCtrlAttribute (subPanel, SUB_PANEL_CANVAS, ATTR_TOP, 30);
		SetCtrlAttribute (subPanel, SUB_PANEL_CANVAS, ATTR_WIDTH, gXsub-4);
		SetCtrlAttribute (subPanel, SUB_PANEL_CANVAS, ATTR_HEIGHT, gYsub-4);
	}
	else if (flag == 1) //means main resize
	{
	    int height, width;

        GetPanelAttribute (panel, ATTR_WIDTH, &width);
        GetPanelAttribute (panel, ATTR_HEIGHT, &height);

		OperateSplitter (panel, PANEL_BOTTOM_SPLITTER, height - gY);
		OperateSplitter (panel, PANEL_RIGHT_SPLITTER, width - gX);

        gX = width;
        gY = height;
	}
	else if (flag == 2)	//means subpanel resize
	{
	    int height, width;
        GetPanelAttribute (subPanel, ATTR_WIDTH, &width);
        GetPanelAttribute (subPanel, ATTR_HEIGHT, &height);
		
		OperateSplitter (subPanel, SUB_PANEL_SPLITTER_2, height - gYsub);
		OperateSplitter (subPanel, SUB_PANEL_SPLITTER, width - gXsub);

		gXsub = width;
		gYsub = height;
	}
}


static int AnchorRight(int panel, int subPanel)
{
    SetPanelPos	 (subPanel,	SUBPANEL_TOP, 		SUBPANEL_LEFT);
   	SetPanelSize (subPanel,	SUBPANEL_HEIGHT, 	SUBPANEL_WIDTH);

	AddPanelToSplitter (panel, PANEL_MID_SPLITTER, VAL_RIGHT_ANCHOR, subPanel, 1, 0);
    AddPanelToSplitter (panel, PANEL_RIGHT_SPLITTER, VAL_LEFT_ANCHOR, subPanel, 1, 0);
    AddPanelToSplitter (panel, PANEL_BOTTOM_SPLITTER, VAL_TOP_ANCHOR, subPanel, 1, 0);
	return 0;
}
static int AnchorLeft(int panel, int subPanel)
{
    SetPanelPos	 (subPanel,	SUBPANEL_TOP, 		2);
   	SetPanelSize (subPanel,	SUBPANEL_HEIGHT, 	SUBPANEL_LEFT-20);

	AddPanelToSplitter (panel, PANEL_MID_SPLITTER, VAL_LEFT_ANCHOR, subPanel, 1, 0);
    AddPanelToSplitter (panel, PANEL_BOTTOM_SPLITTER, VAL_TOP_ANCHOR, subPanel, 1, 0);
	return 0;
}
static int InitGraph()
{
	int panel = GetMainPanel();
	if (panel < 0)
		return -1;

	int panelGraph = InitUIall(panel);
	if (panelGraph < 0)
		return -1;
	
	SetDllPanel(panelGraph);
	
	AnchorRight(panel, panelGraph);
	return 0;
}
static int InitManager()
{
	int panel = GetMainPanel();
	if (panel < 0)
		return -1;

	{
	    //if (InstallWinMsgCallback (panel, WM_TO_MAIN, OnMain, VAL_MODE_IN_QUEUE, NULL, &g_hMain))   //安装
	    if (InstallWinMsgCallback (panel, WM_TO_MAIN, OnMain, VAL_MODE_INTERCEPT, NULL, &g_hMain))   //安装
		{
	        g_hMain = 0;
		}
	}

	int panelMng = InitUI(panel, g_hMain, WM_TO_MAIN);
	if (panelMng < 0)
		return -1;
	
	int panelMnglist = InitUiTab(panel);
	
	SetManagerPanel(panelMng, panelMnglist);
	
	AnchorRight(panel, panelMnglist);
	AnchorLeft(panel, panelMng);
	return 0;
}

static int InitializeDialog()
{
    int panelHandle = GetMainPanel();
    int subPanelHandle = GetChildPanel(SUB_PANEL);

    if (panelHandle < 0 || subPanelHandle < 0)
		return -1;

 	//SetPanelAttribute (subPanelHandle, ATTR_BACKCOLOR, 	VAL_YELLOW);

	AnchorRight(panelHandle, subPanelHandle);

    InitializeMRU(panelHandle);
    return 0;
}

//	application entry point
int main (int argc, char *argv[])
{
    if (InitCVIRTE (0, argv, 0) == 0)
        return -1;

    if (InitializeDialog() < 0)
		return -1;
	if (InitGraph() < 0)
		return -1;
	if (InitManager() < 0)
		return -1;
        
	MovePosition(0);
	
    int panel = GetMainPanel();
    int subPanel = GetChildPanel(SUB_PANEL);
    DisplayPanel (subPanel);
	DisplayPanel (panel);
	InitSubpanel();
		
	RunUserInterface ();

	UninitSubpanel();
    if (g_hMain)
    {
		int nz = RemoveWinMsgCallback (panel, WM_TO_MAIN);   //卸载
	}
	DiscardPanel (panel);
    SetSystemAttribute (ATTR_ALLOW_MISSING_CALLBACKS, 0);
    SetSystemAttribute (ATTR_REPORT_LOAD_FAILURE, 1);

    return 0;
}

//callback for the main application panel
int CVICALLBACK PanelCB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_CLOSE:
        	CleanUpLists();
        	QuitUserInterface (0);
        break;
        case EVENT_PANEL_SIZE:
			MovePosition(1);
        break;
    }
    return 0;
}

int CVICALLBACK SubPanelCB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:
			break;
		case EVENT_LOST_FOCUS:
 			break;
		case EVENT_CLOSE:
			break;
       case EVENT_PANEL_SIZE:
		   MovePosition(2);
		   break;
	}
	return 0;
}
/*
int Relay(int type, void* pData)
{
	if (type == 0)
	{
		char* pmsg = (char*) pData;
		printf("msg:%s\n", pmsg);
	}
	return 1;
}
*/
int CVICALLBACK OnMain(int panel, int msg, unsigned int* wParam, unsigned int* lParam, void* callbackData)
{
	if (msg == EVENT_NEWHANDLE)
	{
        g_hMain = *wParam;    
        printf ("New handle!!!\n");
	}
    else
	{
		int index = *wParam;
		int * pp = lParam;
		int pAddress = *pp;
		char* psz = (char*)pAddress;
		//printf("main recv:%d, %s\n", index, psz);
		
		if (strlen(psz) > 1)
		{
			ShowHideView(1);
			PutWave(psz);
		}
	}
    return 0;
}
