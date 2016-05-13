#include <cvirte.h>		
#include <userint.h>
#include "ManageDll.h"

#include "manageHead.h"
#include <ansi_c.h>

#include <stdbool.h>   //让c支持bool型

#include "JsonImpl.h"
#include "treeMenu.h"

#include "sqlite3.h" 
#include "DBbase.h"
#include "ManageTab.h"
#include "ManageTabTree.h"


int mngPanel;
static int mngPanelTab = -1;
static int gnX = 0;
static int gnY = 0;
static int indexClosed;
static int indexOpen1;
static int indexOpen2;
static int indexOpen3;

intptr_t ghParent;
int gmsg;


#define 	_EXPORT_DLL_
static void MovePanel(bool bInit)
{
	if (bInit)
	{
		GetPanelAttribute (mngPanel, ATTR_WIDTH, &gnX);
		GetPanelAttribute (mngPanel, ATTR_HEIGHT, &gnY);
	}
	else
	{
		int nX, nY;
		GetPanelAttribute (mngPanel, ATTR_WIDTH, &nX);
		GetPanelAttribute (mngPanel, ATTR_HEIGHT, &nY);
		OperateSplitter(mngPanel, PANEL_SPLITTER_V, nX - gnX);
		gnX = nX;
		
		const int BTN_Y = 40;
		if (nY > BTN_Y)
		{
			int nTreeY, nListboxY;
			GetCtrlAttribute(mngPanel, PANEL_TREE, ATTR_HEIGHT, &nTreeY);
			GetCtrlAttribute(mngPanel, PANEL_LISTBOX, ATTR_HEIGHT, &nListboxY);

			float ratio = ((float)nTreeY) / (nTreeY + nListboxY);
			int top = (nY - BTN_Y) * ratio;
			
			SetCtrlAttribute(mngPanel, PANEL_TREE, ATTR_TOP, 0);
			SetCtrlAttribute(mngPanel, PANEL_TREE, ATTR_HEIGHT, top);
			
			SetCtrlAttribute(mngPanel, PANEL_SPLITTER_H2, ATTR_TOP, top+1);
			//SetCtrlAttribute(mngPanel, PANEL_CHECKBOX, ATTR_TOP, top+6);
			SetCtrlAttribute(mngPanel, PANEL_CMDBUTTON_JSON, ATTR_TOP, top+6);
			SetCtrlAttribute(mngPanel, PANEL_CMDBUTTON_SQLITE, ATTR_TOP, top+6);
			
			SetCtrlAttribute(mngPanel, PANEL_LISTBOX, ATTR_TOP, top+BTN_Y);
			SetCtrlAttribute(mngPanel, PANEL_LISTBOX, ATTR_HEIGHT, nY-(top+BTN_Y));
			
		
			OperateSplitter(mngPanel, PANEL_SPLITTER_H, nY - gnY);
			gnY = nY;
		}
	}
}

#ifdef _EXPORT_DLL_																
int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			if (InitCVIRTE (hinstDLL, 0, 0) == 0)
				return 0;	/* out of memory */
			break;
		case DLL_PROCESS_DETACH:
			if (!CVIRTEHasBeenDetached())		/* Do not call CVI functions if cvirte.dll has already been detached. */
			{
				CloseCVIRTE ();
			}
			break;
	}

	return 1;
}
#else

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((mngPanel = LoadPanel (0, "ManageDll.uir", PANEL)) < 0)
		return -1;
	MovePanel(true);

	InitMenu(mngPanel);
	DisplayPanel (mngPanel);
	
	RunUserInterface ();
	DiscardPanel (mngPanel);
	return 0;
}
#endif
static void PopulateTree();
int InitUI(int nParent, intptr_t hParent, int msg)
{
	if (mngPanel > 0)
		return mngPanel;

	if ((mngPanel = LoadPanelEx (nParent, "ManageDll.uir", PANEL, __CVIUserHInst)) < 0)
		return -1;
	
	ghParent = hParent;
	gmsg = msg;
	
	MovePanel(true);
	SetCtrlAttribute(mngPanel, PANEL_COMMANDBUTTON, ATTR_VISIBLE, 0); // ATTR_VISIBLE
	
	InitMenu(mngPanel);
	DisplayPanel (mngPanel);
	AddTreeListPics(); 
	PopulateTree();
	return mngPanel;
}
int InitUiTab(int nParent)
{
	if (mngPanelTab < 0)
	{
		if ((mngPanelTab = LoadPanelEx (nParent, "ManageDll.uir", PANEL_LIST, __CVIUserHInst)) < 0)
			return -1;
		
		MovePosition(mngPanelTab, 1);
		InitTree(mngPanelTab);
		DisplayPanel (mngPanelTab);
	}
	return mngPanelTab;
}

void DiscardUIObjectsForDLLM (void)
{
	if (mngPanel > 0)
	{
		DiscardPanel (mngPanel);
		mngPanel = 0;
	}
}

int CVICALLBACK PanelCB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface (0);
		case EVENT_PANEL_SIZE:
			MovePanel(false);
			break;
	}
	return 0;
}

int CVICALLBACK BtnJsonCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int val, n;
	int ret;
	char szTip[64] = {"0000"};
	switch (event)
	{
		case EVENT_COMMIT:
			ret = SetTreeItemAttribute(panel, PANEL_TREE, 0, ATTR_TOOLTIP_TEXT, "DDD");
			break;
	}
	return 0;
}

int CVICALLBACK BtnSqliteCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_COMMIT:
			//这里放置数据库测试
			break;
	}
	return 0;
}

int CVICALLBACK BtnOkCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface(0);
			CloseDB();
			break;
	}
	return 0;
}


static char sAttribute[256]={0};
int LoadMyInfo( void * para, int n_column, char ** column_value, char ** column_name )
{   
	int nIndex = -1,nItem =-1;

	if (1)
	{
		static int rec = 0;
		if (rec == 0)
		{
	 	 	printf( "字段   ");
			for (int i = 0; i < n_column; i ++)
			{
				if (i == 2)
					printf("\t");
				printf("[%s]  ", column_name[i]);
			}
			printf("\n");
		}
 	 	printf( "%3d    ", rec);
		for (int i = 0; i < n_column; i ++)
		{
			if (i == 2)
				printf("\t");
			printf("[%s]  ", column_value[i]);
		}
		printf("\n");
		rec ++;
		
		char szzzz[64], z[64];
		strcpy(szzzz, column_value[0]);
		strcpy(z, column_name[0]);
		int nz = 0;
	}	
	
	if(strcmp(column_value[3],"1") ==0)
	{
		nIndex = InsertTreeItem(mngPanel, PANEL_TREE, VAL_SIBLING, 0, VAL_LAST, column_value[1], column_value[2], NULL, 0);
		InsertImagesToTreeItem(nIndex,indexOpen1);

	}else
	{
		GetTreeItemFromTag (mngPanel, PANEL_TREE, column_value[3], &nItem);
		nIndex = InsertTreeItem(mngPanel, PANEL_TREE, VAL_CHILD, nItem, VAL_LAST, column_value[1],column_value[2], NULL, 0);
		if(strcmp(column_value[4],"2") ==0)
		InsertImagesToTreeItem(nIndex,indexOpen2);
		else if(strcmp(column_value[4],"3") ==0)
		InsertImagesToTreeItem(nIndex,indexOpen3);
		else
		InsertImagesToTreeItem(nIndex,indexOpen3);	
		
	} 
	return 0;
}

static void PopulateTree()
{
	char *errmsg="";
	sqlite3 * db = DBbase();   //打开数据库;
	CreateDB(); //打开表

	int result = sqlite3_exec( db, "select * from MyTable_1", LoadMyInfo,NULL, &errmsg );
 	if( SQLITE_OK != result )
	{
		 return ;
	}
}

int GetDBData(void** pOutData)
{
	
	char *errmsg="";
	int cnt = 0;
	char **dbret;
	sqlite3 * db = DBbase();   //打开数据库;

	int nRow, nColumn;
	int result = sqlite3_get_table( db, "select count(*) from MyTable_1", &dbret, &nRow, &nColumn, &errmsg );
	if (SQLITE_OK != result)
	{
		return 0;
	}
	cnt = atoi(dbret[1]);
	sqlite3_free_table(dbret);
	
	*pOutData = malloc(sizeof(TabData)* cnt);
	TabData* pData = (TabData*)*pOutData;
	
	result = sqlite3_get_table( db, "select * from MyTable_1", &dbret, &nRow, &nColumn, &errmsg );
	if (SQLITE_OK != result)
	{
		return 0;
	}
	
	for (int i = 1; i <= nRow; i ++)
	{
		for (int j = 0; j < nColumn; j ++)
		{
			if (j == 1)
				strcpy(pData[i-1].szName, dbret[i*nColumn+1]);
			else if (j == 2)
				strcpy(pData[i-1].szID, dbret[i*nColumn+2]);
			else if (j == 3)
				strcpy(pData[i-1].szPID, dbret[i*nColumn+3]);
			else if (j == 4)
				pData[i-1].level = atoi(dbret[i*nColumn+4]);
			else if (j == 5)
				pData[i-1].tick = atoi(dbret[i*nColumn+5]);
		}
		//printf("%s,\t%s,\t%s,\t%d,\t%d\n", pData[i-1].szName, pData[i-1].szID, pData[i-1].szPID, pData[i-1].level, pData[i-1].tick);
	}
	return nRow;
}



static void InsertImagesToTreeItem(int index,int indexOpen) // intsert image   zhw
{
	SetTreeItemAttribute (mngPanel, PANEL_TREE, index, ATTR_IMAGE_INDEX, indexOpen);
	SetTreeItemAttribute (mngPanel, PANEL_TREE, index, ATTR_COLLAPSED_IMAGE_INDEX, indexOpen);
}

static void AddTreeListPics()
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




