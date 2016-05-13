//==============================================================================
//
// Title:		TreeMenu.c
// Purpose:		A short description of the implementation.
//
// Created on:	2015/11/25 at 14:10:54 by cwj.
// Copyright:	hy. All Rights Reserved.
//
//==============================================================================
#include <ansi_c.h>
#include "sqlite3.h"
#include <userint.h>
#include "ManageDll.h"

#include "TreeMenu.h"
#include <stdio.h>
#include "DBbase.h"

static int mngPanel = -1;
static int menuIDBold;
static int menuIDLine;
static int menuIDColor; 
static int menuSeparator = -1;
static int menuAdd;
static int menuDele;
static int menuModify;
static int treeSelect = -1;
static int irecord =-1;


static int nIndex;   //zhw
char tag[256]={0};  //唯一标准，即属性
static char sAttribute[256];  //属性
static char sParentsName[256]={0}; //父名

int SelectItem(){return treeSelect;}

int CVICALLBACK ShowSysMenuCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK TreeItemAdd(int panel, int ctrlId, int menuId, void* callback);
void CVICALLBACK TreeItemDele(int panel, int ctrlId, int menuId, void* callback);
void CVICALLBACK TreeItemModify(int panel, int ctrlId, int menuId, void* callback);


void CVICALLBACK DefferSysMenuItems(void* callback)
{
	ShowSysMenuCB(0,0,0, callback, 0,0);
}
int InitMenu(int panel)
{
	mngPanel = panel;
	if (mngPanel == -1)
		return 0;
	
	{
		PostDeferredCall(DefferSysMenuItems, 0);
	
		HideBuiltInCtrlMenuItem (panel, PANEL_TREE, VAL_SEARCH);
		HideBuiltInCtrlMenuItem (panel, PANEL_TREE, VAL_SORT);
		HideBuiltInCtrlMenuItem (panel, PANEL_TREE, VAL_EXPAND_SUBTREE);
		HideBuiltInCtrlMenuItem (panel, PANEL_TREE, VAL_COLLAPSE_SUBTREE);
		HideBuiltInCtrlMenuItem (panel, PANEL_TREE, VAL_EXPAND_ALL);
		HideBuiltInCtrlMenuItem (panel, PANEL_TREE, VAL_COLLAPSE_ALL);
		
		menuAdd = NewCtrlMenuItem(panel, PANEL_TREE, "增 加", -1, TreeItemAdd, 0);
		menuDele = NewCtrlMenuItem(panel, PANEL_TREE, "删 除", -1, TreeItemDele, 0);
		menuModify = NewCtrlMenuItem(panel, PANEL_TREE, "修 改", -1, TreeItemModify, 0);
	}
	AddTreeListPics();
	DBbase();  
	irecord  =	SelectDB();
	irecord ++; 
	SetCtrlAttribute(panel, PANEL_TREE, ATTR_DISABLE_TREE_TOOLTIPS, 1);
	return 1;
}
int CVICALLBACK ShowSysMenuCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int val;
	switch (event)
	{
		case EVENT_COMMIT:
			//GetCtrlVal (panel, PANEL_CHECKBOX, &val);  
			break;
	}
	return 0;
}
	

char labelTxt[256] = {0}; 
void CVICALLBACK TreeItemAdd(int panel, int ctrlId, int menuItem, void* callback)
{  	//ctrlId means tree's id.
	int numItem = 0;
	int parentItem = treeSelect;

	int dlg = LoadPanelEx(mngPanel, "ManageDll.uir", PANEL_MENU, __CVIUserHInst);
	
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
			char srecord[256]={0};
			char snewTxt[256]={0};
			sprintf(srecord,"%d",irecord);
			sprintf(snewTxt,"%s",labelTxt); 
			if (parentItem == -1)
			{
				strcat(snewTxt,srecord);  
				nIndex = InsertTreeItem(panel, ctrlId, VAL_SIBLING, 0, VAL_LAST, labelTxt, snewTxt, NULL, parentItem);
				InsertImagesToTreeItem (nIndex,indexOpen1);  //zhw
				InsertDB(labelTxt,snewTxt,"1",1,irecord);
				irecord ++;
			   
			}
			else
			{
	 			strcat(sAttribute,labelTxt);  //属性// ATTR_CTRL_VAL 
				strcat(sAttribute,srecord); 
	 			nIndex = InsertTreeItem(panel, ctrlId, VAL_CHILD, parentItem, VAL_LAST, labelTxt, sAttribute, NULL, parentItem);
				int colnum =0; 
				GetTreeItemLevel (panel, PANEL_TREE, nIndex, &colnum);
				switch(colnum)
				{
					case 1:
						InsertImagesToTreeItem (nIndex,indexOpen2);  //zhw 
						InsertDB(labelTxt,sAttribute,sParentsName,2,irecord);
						break;
					case 2:
						InsertImagesToTreeItem (nIndex,indexOpen3);  //zhw 
						InsertDB(labelTxt,sAttribute,sParentsName,3,irecord);
						break;
					case 3:
						InsertImagesToTreeItem (nIndex,indexOpen3);  //zhw
						InsertDB(labelTxt,sAttribute,sParentsName,3,irecord);
						break;
					default:
						InsertImagesToTreeItem (nIndex,indexOpen3);//zhw
						InsertDB(labelTxt,sAttribute,sParentsName,3,irecord);
						break;
				}
				irecord ++; 
						
			}
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
	if (treeSelect > -1)
	{
		DeleteListItem(panel, ctrlId, treeSelect, 1);
		DeleteDB(sAttribute);
	}
}
void CVICALLBACK TreeItemModify(int panel, int ctrlId, int menuItem, void* callback)
{
	int parentItem = treeSelect;
	char labelTxt[256] = {0};  
	if (parentItem == -1)
		return;
	int dlg = LoadPanelEx(mngPanel, "ManageDll.uir", PANEL_MENU, __CVIUserHInst);
	
//	char oldname[256] = {0}; 
//	GetLabelFromIndex(panel,ctrlId,parentItem,oldname); //获取旧的名称
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
			UpdateDbTable(labelTxt,sAttribute);
			exit = 1;
		}
        else if (dlgCtrl == PANEL_MENU_CB_CANCEL)
        {
			exit = 1;
		}
		else if (dlgCtrl == PANEL_MENU_STRING_NAME)
		{
			//GetCtrlVal(dlg, PANEL_MENU_STRING_NAME, labelTxt);
			//SetTreeItemAttribute(panel, ctrlId, parentItem, ATTR_LABEL_TEXT, labelTxt);
		}
   	}
	DiscardPanel(dlg);
}

int CVICALLBACK TreeCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int item;  int x,y;
	int val=0;
	char *selectname;
	switch (event)
	{
		case EVENT_COMMIT:
			if (GetActiveTreeItem (panel, PANEL_TREE, &item) >= 0)
			{
			}
			break;
		case EVENT_SELECTION_CHANGE:
			if (eventData1)
			{
				treeSelect = eventData2;
				GetTreeItemTag(panel, PANEL_TREE, treeSelect, tag) ;   //获取当前选择的tag即属性
				sprintf(sParentsName,"%s",tag);  //父名
				sprintf(sAttribute,"%s",tag);  //属性
			}
			else
			{
				treeSelect = -1;
			}
			break;
	}
	return 0;
}


	
void InsertImagesToTreeItem(int index,int indexOpen) // intsert image   zhw
{
	SetTreeItemAttribute (mngPanel, PANEL_TREE, index, ATTR_IMAGE_INDEX, indexOpen);
	SetTreeItemAttribute (mngPanel, PANEL_TREE, index, ATTR_COLLAPSED_IMAGE_INDEX, indexOpen);
}

void AddTreeListPics()
{
	int bmOne,bmTwo,bmThree;
	GetBitmapFromFile("tree1.ico",&bmOne);
	indexOpen1	=	AddTreeImage (mngPanel, PANEL_TREE, bmOne);
	DiscardBitmap (bmOne);
	
	GetBitmapFromFile("tree2.ico",&bmTwo);
	indexOpen2	=	AddTreeImage (mngPanel, PANEL_TREE, bmTwo);
	DiscardBitmap (bmTwo);
	
	GetBitmapFromFile("tree3.ico",&bmThree);
	indexOpen3	=	AddTreeImage (mngPanel, PANEL_TREE, bmThree);
	DiscardBitmap (bmThree);
}









