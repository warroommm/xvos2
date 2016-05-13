// This file contains utility functions that are used to handle the various
// menu options (context menus and menus on the menu bar) provided in the application


#include <utility.h>
#include <ansi_c.h>
#include <userint.h>
#include "XVos.h"
#include "menu.h"
#include "pnl.h"
#include "utils.h"

// MRU list instrument
#include "inifile.h"
#include "menuutil.h"

static menuList gFileMenuList;
static IniText gIniText;

static int* CurrentTreeSelectedPanel(void);
static void ClearMarkedTreeItems(int itemMarkState);
static void UpdateMenus(int panelHandle, int menuHandle, int treeItemIndex);
static int AddUIRToMRUList (menuList menuListHandle,char *fullPath);
static int IsUIRLoaded(const char* fileName);

void SaveMRUList(void)
{
	/* Save FILE menulist filenames to INI object */
  	if (gFileMenuList)
    	MU_PutFileListInIniFile (gFileMenuList, gIniText, MRU_SECTIONNAME, MRU_TAGPREFIX, TRUE);

	/* Save options from UIR to system */
  	if(gIniText)
    	MU_WriteRegistryInfo (gIniText, MRU_REGISTRY_NAME);

  	Ini_Dispose(gIniText);
  	MU_DeleteMenuList(gFileMenuList);
}

int LoadUIR_safe(char* fileName)
{
	// don't load the same UIR twice
    if (IsUIRLoaded(fileName))
		return 0;

	return LoadUIR(fileName);
}
static int IsUIRLoaded(const char* fileName)
{
  	ListType* UIRList = GetUIRList();

  	return (int)ListFindItem (*UIRList, fileName, FRONT_OF_LIST, UIRPathCompare);
}

void CVICALLBACK MRUFileCB (menuList list, int menuIndex, int event, void *callbackData)
{
    char* fileName = (char*) callbackData;

    switch(event)
    {
        case EVENT_DISCARD:
            if(fileName)
                free(fileName);
        	break;
        case EVENT_COMMIT:
            if (fileName)
            {
				LoadUIR_safe(fileName);
            }
        break;
    }
}

void InitializeMRU(int panelHandle)
{
    if (0 == gIniText)       	//Initialize MRU list     
        gIniText = Ini_New(0);
 
    if (gIniText)
    {
       int menubarHandle = GetPanelMenuBar(panelHandle);
       /* Read previous MRU list data from system */
        MU_ReadRegistryInfo (gIniText, MRU_REGISTRY_NAME);

        /* Create FILE menulist if it does not already exist */
        if (!gFileMenuList)
            gFileMenuList = MU_CreateMenuList (menubarHandle, MAIN_FILE, MAIN_FILE_ABOVE_EXIT_SEPARATOR, MRU_MAX_ITEMS, MRUFileCB);

        if (gFileMenuList)
       	{
            /* Update attributes */
            MU_SetMenuListAttribute (gFileMenuList, 0, ATTR_MENULIST_APPEND_SHORTCUT, 1);
            MU_SetMenuListAttribute (gFileMenuList, 0, ATTR_MENULIST_ALLOW_DUPLICATE_ITEMS, 0);

            /* Update FILE menulist with files from INI object */
            MU_GetFileListFromIniFile (gFileMenuList, gIniText, "FileMenuList", "Filename", 1);
     	}
    }
}

// 动态增加一个到MRU*/
static int AddUIRToMRUList (menuList menuListHandle, char *fullPath)
{
    int retVal = 0;

    if ((menuListHandle) && (fullPath) &&  (fullPath[0] != '\0') )
    {
		char* pMake = MU_MakeShortFileName (NULL, fullPath, MRU_MAX_PATH_SIZE);
		retVal = MU_AddItemToMenuList (menuListHandle, FRONT_OF_LIST, pMake, StrDup (fullPath));
	}
    return retVal;
}

// Stores the handle to the panel currently selected in the tree control.
static int* CurrentTreeSelectedPanel()
{
    static panelHandle = 0;
    return &panelHandle;
}


// Launches the file dialog to select the UIR to be viewed.
void CVICALLBACK FileOpen (int menuBar, int menuItem, void *callbackData, int panel)
{
	return;
}


void AddUIRtoTree(char* tempUIRPath)
{
    ListType *panelList = NULL;
    ListType *UIRList = GetUIRList();
    int panel = GetMainPanel();

    if(UIRList!=NULL)
    {
        size_t index = 0;
        UIRSTRUCT* UIRItem = NULL;
        PANELSTRUCT *panelStruct = NULL;
        size_t listSize = 0;
        int childIndex;

        // Add UIR Path to the Tree
        int itemIndex = InsertTreeItem (panel, PANEL_TREE, VAL_SIBLING, 0, VAL_NEXT, tempUIRPath, UIR_TAG, NULL, 0);

        index = ListFindItem (*UIRList, tempUIRPath, FRONT_OF_LIST, UIRPathCompare);
        UIRItem = ListGetPtrToItem (*UIRList, index);

        panelList = UIRItem->panelList;

        listSize = ListNumItems(*panelList);

        for(index =0;index<listSize;index++)
        {
            panelStruct = ListGetPtrToItem(*panelList,index);

            // Add the individual panels to the tree
            childIndex = InsertTreeItem (panel, PANEL_TREE, VAL_CHILD, itemIndex, VAL_LAST, panelStruct->panelName, PANEL_TAG, NULL, panelStruct->panelID);
        }
    }

    return;
}

//Remove tree items with the specified state from the tree control
static void ClearMarkedTreeItems(int itemMarkState)
{
    int item = 0;
    int count;
    char uirPath[260];
    int direction = VAL_NEXT_PLUS_SELF;

    int panelHandle = GetMainPanel();

    panelHandle = GetMainPanel();

    GetNumListItems (panelHandle, PANEL_TREE, &count);

    if(count > 0)
    {
        //check for tree items that are marked
        while(item != -1)
        {
            int beginIndex = item;

            GetNumListItems (panelHandle, PANEL_TREE, &count);

            if(count<=0)    	//if the tree is empty, no unloading needs to be done
				return;

            if(beginIndex <= count)
            {
             	direction = VAL_NEXT_PLUS_SELF;
             	beginIndex = VAL_FIRST ;
            }

            GetTreeItem (panelHandle, PANEL_TREE, VAL_SIBLING, 0, beginIndex, direction, itemMarkState, &item);
            if(item != -1)
            {
                GetTreeItemAttribute (panelHandle, PANEL_TREE, item, ATTR_LABEL_TEXT, uirPath);

                //Remove the UIR from the UIR linked list
                RemoveUIRFromList(uirPath);
                DeleteListItem (panelHandle, PANEL_TREE, item, 1);
            }
        }
    }
}

// 清空
void CVICALLBACK UIRCloseCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    return;
}

// Exits the application
void CVICALLBACK FileExit (int menuBar, int menuItem, void *callbackData, int panel)
{
     PanelCB(0, EVENT_CLOSE, 0, 0, 0);
}

void CVICALLBACK DeferredMenuCB (void *callbackData)
{
    char tag[MAX_TREE_ITEM_TAG_LEN];
    int menuBarHandle;
    int panelHandle;
    int selectedTreeItem;

    CallBackData *data = callbackData;
    panelHandle = data->panelHandle;
    selectedTreeItem = data->treeItemIndex;

    GetTreeItemTag (panelHandle, PANEL_TREE, selectedTreeItem, tag);

    if(strncmp(tag, PANEL_TAG, strlen(PANEL_TAG)) == 0)
    {
        char uirPath[MAX_PATHNAME_LEN];

        int parentIndex;
        size_t foundItemIndex;
        int* currentPanel = NULL;
        UIRSTRUCT *foundUIR;
        char treeItemLabel[MAX_PATHNAME_LEN];
        PANELSTRUCT *panelStruct;
        int menuItemID = 0;

        ListType *UIRList = GetUIRList();

        GetTreeItemParent(panelHandle, PANEL_TREE, selectedTreeItem, &parentIndex);
        GetTreeItemAttribute (panelHandle, PANEL_TREE, parentIndex, ATTR_LABEL_TEXT, uirPath);

        foundItemIndex = ListFindItem(*UIRList, uirPath, FRONT_OF_LIST, UIRPathCompare);
        foundUIR = ListGetPtrToItem(*UIRList, foundItemIndex);

        GetTreeItemAttribute(panelHandle, PANEL_TREE, selectedTreeItem, ATTR_LABEL_TEXT, treeItemLabel);
        foundItemIndex = ListFindItem(*foundUIR->panelList, treeItemLabel, FRONT_OF_LIST, PanelCompare);

        panelStruct = ListGetPtrToItem(*foundUIR->panelList, foundItemIndex);

        currentPanel = CurrentTreeSelectedPanel();
        *currentPanel= panelStruct->panelHandle;

        menuBarHandle = LoadMenuBar (0, UIRNAME, TREE);

        UpdateMenus(*currentPanel, menuBarHandle,selectedTreeItem);

        menuItemID = RunPopupMenu (menuBarHandle, TREE_PANEL, panelHandle, data->point.y, data->point.x, 0, 0, 0, 0);

        //if the panel close menu item is selected, update the tree control
        //checkbox to indicate this
        if(menuItemID == TREE_PANEL_ATTRIB_CLOSE)
        {
            SetTreeItemAttribute (panelHandle, PANEL_TREE, selectedTreeItem, ATTR_MARK_STATE, VAL_MARK_OFF);
        }

        DiscardMenuBar(menuBarHandle);
    }

    free(data);
}


//Update menus associated with the panels
static void UpdateMenus(int panelHandle, int menuHandle, int treeItemIndex)
{
    int treeMarkState;
    int bCanMax;
    int bCanMin;
    int bCloseCtrl;
    int bMoveable;
    int bParentCheck;
    int bSizable;
    int bTitle;
    int bColors;
    int height, width;

    GetTreeItemAttribute (GetMainPanel(), PANEL_TREE, treeItemIndex, ATTR_MARK_STATE, &treeMarkState);

    if(treeMarkState == VAL_MARK_ON)
        SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_CLOSE, ATTR_DIMMED, 0);
    else
        SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_CLOSE, ATTR_DIMMED, 1);

    GetPanelAttribute (panelHandle, ATTR_PANEL_PARENT, &bParentCheck);

    //these attributes only have an effect if the panel is not a child panel
    if(bParentCheck == 0)
    {
        SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_MAXIMIZE, ATTR_DIMMED, 0);
        SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_MINIMIZE, ATTR_DIMMED, 0);
        SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_CLOSE_CTRL, ATTR_DIMMED, 0);

        /* can maximize */
        GetPanelAttribute (panelHandle, ATTR_CAN_MAXIMIZE, &bCanMax);
        SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_MAXIMIZE, ATTR_CHECKED, bCanMax);

        /* can minimize */
        GetPanelAttribute (panelHandle, ATTR_CAN_MINIMIZE, &bCanMin);
        SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_MINIMIZE, ATTR_CHECKED, bCanMin);

        /* close control */
        GetPanelAttribute (panelHandle, ATTR_CLOSE_ITEM_VISIBLE, &bCloseCtrl);
        SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_CLOSE_CTRL, ATTR_CHECKED, bCloseCtrl);

    }
    else //dim them otherwise
    {
        SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_MAXIMIZE, ATTR_DIMMED, 1);
        SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_MINIMIZE, ATTR_DIMMED, 1);
        SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_CLOSE_CTRL, ATTR_DIMMED, 1);
    }
    /* movable */
    GetPanelAttribute (panelHandle, ATTR_MOVABLE, &bMoveable);
    SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_MOVABLE, ATTR_CHECKED, bMoveable);

    /* sizable */
    GetPanelAttribute (panelHandle, ATTR_SIZABLE, &bSizable);
    SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_SIZABLE, ATTR_CHECKED, bSizable);

    /* Title visible */
    GetPanelAttribute (panelHandle, ATTR_TITLEBAR_VISIBLE, &bTitle);
    SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_TITLE_VISIBLE, ATTR_CHECKED, bTitle);

    /* System colors */
    GetPanelAttribute (panelHandle, ATTR_CONFORM_TO_SYSTEM, &bColors);
    SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_SYSTEM_COLORS, ATTR_CHECKED, bColors);

    /* Scalable */
    GetPanelAttribute (panelHandle, ATTR_SCALE_CONTENTS_ON_RESIZE, &bColors);
    SetMenuBarAttribute (menuHandle, TREE_PANEL_ATTRIB_SCALABLE, ATTR_CHECKED, bColors);

    /* If minimum height and width are zero, set minimum height and width to 1/2 */
    GetPanelAttribute (panelHandle, ATTR_MIN_HEIGHT_FOR_SCALING, &height);
    GetPanelAttribute (panelHandle, ATTR_MIN_WIDTH_FOR_SCALING, &width);
    if ((!height) && (!width))
    {
        GetPanelAttribute (panelHandle, ATTR_WIDTH, &height);
        GetPanelAttribute (panelHandle, ATTR_WIDTH, &width);

        SetPanelAttribute (panelHandle,ATTR_MIN_HEIGHT_FOR_SCALING, height/2);
        SetPanelAttribute (panelHandle,ATTR_MIN_WIDTH_FOR_SCALING, width/2);
    }
}



//Copies the panel image to clipboard
void CVICALLBACK CopyCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int hBMP;
    int *panelHandle = CurrentTreeSelectedPanel();

    GetPanelDisplayBitmap (*panelHandle, VAL_VISIBLE_AREA, VAL_ENTIRE_OBJECT, &hBMP);
    ClipboardPutBitmap (hBMP);
}

// Save panel image to file
void CVICALLBACK SaveCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int hBMP;
    char fileName[MAX_PATHNAME_LEN];
    int iFileStatus;
    int *panelHandle = NULL;
    panelHandle = CurrentTreeSelectedPanel();

    iFileStatus = FileSelectPopupEx ("", "*.bmp", "*.bmp", "Save Bitmap", VAL_SAVE_BUTTON, 0, 1, fileName);

    switch (iFileStatus) {
        case VAL_EXISTING_FILE_SELECTED:
        case VAL_NEW_FILE_SELECTED:
            GetPanelDisplayBitmap (*panelHandle, VAL_VISIBLE_AREA, VAL_ENTIRE_OBJECT, &hBMP);
            SaveBitmapToFile(fileName, hBMP);
            break;
        case VAL_NO_FILE_SELECTED:
            /* cancel selected */
            break;
        default:
             MessagePopup ("Error", "Error in FileSelectPopupEx().");
            break;
    }

    return;
}

// Prints the panel
void CVICALLBACK PrintCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int *panelHandle = NULL;
    panelHandle = CurrentTreeSelectedPanel();

    SetWaitCursor (1);

    PrintPanel (*panelHandle, "", 0, VAL_VISIBLE_AREA, 1);

    SetWaitCursor (0);
}

// Can Maximize attribute
void CVICALLBACK PanelAttribCanMaximizeCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int bCanMax;
    int *panelHandle = NULL;

    panelHandle = CurrentTreeSelectedPanel();

    GetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_MAXIMIZE, ATTR_CHECKED, &bCanMax);

    SetPanelAttribute (*panelHandle,ATTR_CAN_MAXIMIZE, !bCanMax);

    SetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_MAXIMIZE, ATTR_CHECKED, !bCanMax);
}

// Can minimize attribute
void CVICALLBACK PanelAttribCanMinimizeCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int bCanMin;
    int *panelHandle = CurrentTreeSelectedPanel();

    GetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_MINIMIZE, ATTR_CHECKED, &bCanMin);

    SetPanelAttribute (*panelHandle, ATTR_CAN_MINIMIZE, !bCanMin);

    SetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_MINIMIZE, ATTR_CHECKED, !bCanMin);
}

// Close Control Attribute
void CVICALLBACK CloseControlCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int bCloseCtrl;
    int *panelHandle = CurrentTreeSelectedPanel();

    GetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_CLOSE_CTRL, ATTR_CHECKED, &bCloseCtrl);

    SetPanelAttribute (*panelHandle,ATTR_CLOSE_ITEM_VISIBLE, !bCloseCtrl);

    SetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_CLOSE_CTRL, ATTR_CHECKED, !bCloseCtrl);
}

// Movable attribute
void CVICALLBACK MovableCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int bMovePanel;
    int *panelHandle = CurrentTreeSelectedPanel();

    GetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_MOVABLE, ATTR_CHECKED, &bMovePanel);

    SetPanelAttribute (*panelHandle, ATTR_MOVABLE, !bMovePanel);

    SetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_MOVABLE, ATTR_CHECKED, !bMovePanel);
}

// Sizable attribute
void CVICALLBACK SizableCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int bSizePanel;
    int *panelHandle = NULL;
    panelHandle = CurrentTreeSelectedPanel();

    GetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_SIZABLE, ATTR_CHECKED, &bSizePanel);

    SetPanelAttribute (*panelHandle, ATTR_SIZABLE, !bSizePanel);

    SetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_SIZABLE, ATTR_CHECKED, !bSizePanel);
}

// Title bar Visible attribute
void CVICALLBACK TitleBarVisibleCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int bTitleVisible;
    int *panelHandle = CurrentTreeSelectedPanel();

    GetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_TITLE_VISIBLE, ATTR_CHECKED, &bTitleVisible);

    SetPanelAttribute (*panelHandle, ATTR_TITLEBAR_VISIBLE, !bTitleVisible);

    SetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_SIZABLE, ATTR_CHECKED, !bTitleVisible);
}

// Conform to system colors attribute
void CVICALLBACK SystemColorsCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int bSystemColors;
    int *panelHandle = CurrentTreeSelectedPanel();

    GetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_SYSTEM_COLORS, ATTR_CHECKED, &bSystemColors);

    SetPanelAttribute (*panelHandle, ATTR_CONFORM_TO_SYSTEM, !bSystemColors);

    SetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_SYSTEM_COLORS, ATTR_CHECKED, !bSystemColors);
}

// Scalable attribute
void CVICALLBACK ScalableCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int bScalablePanel;
    int *panelHandle = CurrentTreeSelectedPanel();

    GetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_SCALABLE, ATTR_CHECKED, &bScalablePanel);

    SetPanelAttribute (*panelHandle, ATTR_SCALE_CONTENTS_ON_RESIZE, !bScalablePanel);

    SetMenuBarAttribute (menuBar, TREE_PANEL_ATTRIB_SCALABLE, ATTR_CHECKED, !bScalablePanel);
}

// Hide the panel
void CVICALLBACK PanelCloseCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int *panelHandle = CurrentTreeSelectedPanel();

    HidePanel(*panelHandle);
}

// Show Help dialog associated with application
void CVICALLBACK ViewerHelpCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int ctrlID;
    int panelHandle = LoadPanel(0, UIRNAME, HELPPANEL);
	
    InstallPopup(panelHandle);
    GetUserEvent (1, &panelHandle, &ctrlID);
    DiscardPanel(panelHandle);
}

//Shows the "About" dialog
void CVICALLBACK AboutViewerCB (int menuBar, int menuItem, void *callbackData, int panel)
{
    int ctrlID;

    int panelHandle = LoadPanel(0, UIRNAME, ABOUTPANEL);

    InstallPopup(panelHandle);
    GetUserEvent (1, &panelHandle, &ctrlID);
    DiscardPanel(panelHandle);
}

int CVICALLBACK AboutCanvasCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    int userevent = 1001;

    //send a user event to wake up GetUserEvent in AboutViewerCB()
    switch (event)
    {
        case EVENT_LEFT_CLICK:   
        	QueueUserEvent(userevent,panel,control);
            break;
    }
    return 0;
}

// Prints the panel
void CVICALLBACK TreeAdd (int menuBar, int menuItem, void *callbackData, int panel)
{
    int *panelHandle = NULL;
    panelHandle = CurrentTreeSelectedPanel();
	//ListInsertItem();
	//InsertTreeItem();
	
}

// Prints the panel
void CVICALLBACK TreeModify (int menuBar, int menuItem, void *callbackData, int panel)
{
    int *panelHandle = NULL;
    panelHandle = CurrentTreeSelectedPanel();
}

// Prints the panel
void CVICALLBACK TreeDele (int menuBar, int menuItem, void *callbackData, int panel)
{
    int *panelHandle = NULL;
    panelHandle = CurrentTreeSelectedPanel();
}


void ShowHideView (int nType)
{
	static int nswitch = -1;
	
	if (nswitch == nType)
		return;
	
	nswitch = nType;
	int subPanel = GetChildPanel(SUB_PANEL);
	int graphPanel = GetDllPanel();
	int managerPanel = GetManagerPanel(1);

	if (nType == 0)
	{
		HidePanel(graphPanel);
		HidePanel(managerPanel);
		DisplayPanel(subPanel);
	}
	else if (nType == 1)
	{
		HidePanel(subPanel);
		HidePanel(managerPanel);
		DisplayPanel(graphPanel);
	}
	else if (nType == 2)
	{
		HidePanel(subPanel);
		HidePanel(graphPanel);
		DisplayPanel(managerPanel);
	}
}

void CVICALLBACK ShowViewOn (int menuBar, int menuItem, void *callbackData, int panel)
{
	ShowHideView(0);
}

void CVICALLBACK ShowViewGraph (int menuBar, int menuItem, void *callbackData, int panel)
{
	ShowHideView(1);
}

void CVICALLBACK ShowViewPara (int menuBar, int menuItem, void *callbackData, int panel)
{
	ShowHideView(2);
}

