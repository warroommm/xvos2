//==============================================================================
//
// Title:		ManageTabTree.c
// Purpose:		A short description of the implementation.
//
// Created on:	2016/01/25 at 14:10:54 by cwj.
// Copyright:	hy. All Rights Reserved.
//
//==============================================================================

#include <ansi_c.h>
#include <userint.h>
#include "ManageDll.h"

#include "ManageTabTree.h"

static int tabPanel;
static int gPanel=0;

static int menuAdd;
static int menuDele;
static int menuModify;

static int gTreeItem = -1;
int SelectTreeItem()	{return gTreeItem;}

static void CVICALLBACK TreeItemAdd(int panel, int ctrlId, int menuId, void* callback);
static void CVICALLBACK TreeItemDele(int panel, int ctrlId, int menuId, void* callback);
static void CVICALLBACK TreeItemModify(int panel, int ctrlId, int menuId, void* callback);
static int CVICALLBACK ShowSysMenuCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event){
		case EVENT_COMMIT:
			break;
	}
	return 0;
}
static void CVICALLBACK DefferSysMenuItems(void* callback)   {ShowSysMenuCB(0,0,0, callback, 0,0);}

int InitTree(int panel)
{
	tabPanel = panel;
	
	if (gPanel == 0)
	{
		PostDeferredCall(DefferSysMenuItems, 0);
	
		GetPanelHandleFromTabPage(panel, PANEL_LIST_TAB, 0, &gPanel);
		
		HideBuiltInCtrlMenuItem (gPanel, TAB_DEVICE_TREE, VAL_SEARCH);
		HideBuiltInCtrlMenuItem (gPanel, TAB_DEVICE_TREE, VAL_SORT);
		HideBuiltInCtrlMenuItem (gPanel, TAB_DEVICE_TREE, VAL_EXPAND_SUBTREE);
		HideBuiltInCtrlMenuItem (gPanel, TAB_DEVICE_TREE, VAL_COLLAPSE_SUBTREE);
		HideBuiltInCtrlMenuItem (gPanel, TAB_DEVICE_TREE, VAL_EXPAND_ALL);
		HideBuiltInCtrlMenuItem (gPanel, TAB_DEVICE_TREE, VAL_COLLAPSE_ALL);
		
		menuAdd = NewCtrlMenuItem(gPanel, TAB_DEVICE_TREE, "增 1加", -1, TreeItemAdd, 0);
		menuDele = NewCtrlMenuItem(gPanel, TAB_DEVICE_TREE, "删 除", -1, TreeItemDele, 0);
		menuModify = NewCtrlMenuItem(gPanel, TAB_DEVICE_TREE, "修 改", -1, TreeItemModify, 0);
	
		SetCtrlAttribute(gPanel, TAB_DEVICE_TREE, ATTR_DISABLE_TREE_TOOLTIPS, 1);
	}
	return 1;
}

void CVICALLBACK TreeItemAdd(int panel, int ctrlId, int menuItem, void* callback)
{  	//ctrlId means tree's id.

	int numItem = 0;
	int parentItem = gTreeItem;
	char labelTxt[256] = {0};
	if (parentItem == -1)
	{
		GetNumListItems(panel, ctrlId, &numItem);
		
	}
	else
	{
		//int zz = 0;
		//GetNumTreeItems(panel, ctrlId, VAL_CHILD, parentItem, VAL_FIRST, VAL_NEXT_PLUS_SELF, VAL_UNSELECTED, &zz);
		//int nz = 0;
		
		int cnt;
		if (GetNumListItems(panel, ctrlId, &cnt) < 0)
		{
			gTreeItem = -1;
			return;
		}

		GetTreeItemAttribute (panel, ctrlId, parentItem, ATTR_LABEL_TEXT, labelTxt);
	}
	
	int dlg = LoadPanelEx(tabPanel, "ManageDll.uir", PANEL_MENU, __CVIUserHInst);
	
	SetCtrlVal(dlg, PANEL_MENU_STRING_NAME, labelTxt);
	SetPanelAttribute(dlg, ATTR_TITLE, "增 加");
	SetCtrlAttribute(dlg, PANEL_MENU_CB_OK, ATTR_LABEL_TEXT, "增 加"); 
	
    InstallPopup(dlg);
	int exit = 0;
	while (exit == 0)
    {
		int dlgCtrl;
    	GetUserEvent (1, &dlg, &dlgCtrl);

        if (dlgCtrl == PANEL_MENU_CB_OK)
		{
			GetCtrlVal(dlg, PANEL_MENU_STRING_NAME, labelTxt);
			int nIndex;
			if (parentItem == -1)
			{
				if (numItem == 0)
					nIndex = InsertTreeItem(panel, ctrlId, VAL_SIBLING, parentItem, VAL_LAST, labelTxt, "", NULL, parentItem);
				else
					nIndex = InsertTreeItem(panel, ctrlId, VAL_SIBLING, 0, VAL_LAST, labelTxt, "", NULL, 0);
			}
			else
				nIndex = InsertTreeItem(panel, ctrlId, VAL_CHILD, parentItem, VAL_LAST, labelTxt, "", NULL, parentItem);
			
			exit = 1;
		}
        else if (dlgCtrl == PANEL_MENU_CB_CANCEL)
        {
			exit = 1;
		}
		else if (dlgCtrl == PANEL_MENU_STRING_NAME)
		{
		}
   	}
	DiscardPanel(dlg);
}
void CVICALLBACK TreeItemDele(int panel, int ctrlId, int menuItem, void* callback)
{
	if (gTreeItem > -1)
	{
		DeleteListItem(panel, ctrlId, gTreeItem, 1);
		
	}
}
void CVICALLBACK TreeItemModify(int panel, int ctrlId, int menuItem, void* callback)
{
	int parentItem = gTreeItem;
	if (parentItem == -1)
		return;

	char labelTxt[256] = {0};
	GetTreeItemAttribute (panel, ctrlId, parentItem, ATTR_LABEL_TEXT, labelTxt);
	
	int dlg = LoadPanelEx(tabPanel, "ManageDll.uir", PANEL_MENU, __CVIUserHInst);
	
	SetCtrlVal(dlg, PANEL_MENU_STRING_NAME, labelTxt);
	SetPanelAttribute(dlg, ATTR_TITLE, "修 改");
	SetCtrlAttribute(dlg, PANEL_MENU_CB_OK, ATTR_LABEL_TEXT, "修 改"); 
	
    InstallPopup(dlg);
	int exit = 0;
	while (exit == 0)
    {
		int dlgCtrl;
    	GetUserEvent (1, &dlg, &dlgCtrl);

        if (dlgCtrl == PANEL_MENU_CB_OK)
		{
			GetCtrlVal(dlg, PANEL_MENU_STRING_NAME, labelTxt);
			SetTreeItemAttribute(panel, ctrlId, parentItem, ATTR_LABEL_TEXT, labelTxt);
			
			exit = 1;
		}
        else if (dlgCtrl == PANEL_MENU_CB_CANCEL)
        {
			exit = 1;
		}
		else if (dlgCtrl == PANEL_MENU_STRING_NAME)
		{
		}
   	}
	DiscardPanel(dlg);
}

int CVICALLBACK DeviceTreeCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int item;
	switch (event)
	{
		case EVENT_COMMIT:
			if (GetActiveTreeItem (gPanel, TAB_DEVICE_TREE, &item) >= 0)
			{
			}
			break;
		case EVENT_LEFT_CLICK:
			break;
		case EVENT_RIGHT_CLICK:
			if (GetActiveTreeItem (gPanel, TAB_DEVICE_TREE, &item) >= 0)
			{
				//printf("success: %d(x:%d, y:%d\n", item, eventData2, eventData1);
			}
			else
			{
				//printf("failure: %d(x:%d, y:%d\n", -1, eventData2, eventData1);
			}
			break;
		case EVENT_SELECTION_CHANGE:
			gTreeItem = eventData1 ? eventData2 : -1;
			break;
	}
	return 0;
}
