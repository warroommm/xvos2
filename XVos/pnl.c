// This file has utility functions for handling interactions for the panels that
// are loaded at runtime.

#include <toolbox.h>
#include <userint.h>
#include <utility.h>
#include <ansi_c.h>
#include "pnl.h"
#include "ctrl.h"
#include "menu.h"
#include "XVos.h"
#include "utils.h"

int gbCtrlCB = 0;	  //flag is Ctrl Callback


int CVICALLBACK RuntimePanelCB (int panelHandle, int event, void *callbackData, int eventData1, int eventData2)
{
  char childWindowMenu[MAX_STRING_SIZE];
  char panelString[MAX_STRING_SIZE];
  int menuBarHandle;
  int menuID;
  int releaseID;
  size_t foundItem = 0;
  int isChildPanel = -1;
  ListType *panelList = callbackData;
  PANELSTRUCT* panelStruct = NULL;

  switch(event)
  {
   case EVENT_RIGHT_CLICK:

       //if the callback data does not contain the panel list, return
       if(panelList == NULL) return 0;

       //If this is a callback for a control, swallow the panel event
       if(gbCtrlCB)
       {
            gbCtrlCB = 0;
            return 1;
       }

       menuBarHandle = NewMenuBar (0);
       menuID = NewMenu(menuBarHandle,"",-1);
       GetPanelAttribute(panelHandle,ATTR_CONSTANT_NAME,panelString);
       sprintf(childWindowMenu,"Panel Constant: %s",panelString);

       NewMenuItem(menuBarHandle,menuID,childWindowMenu,-1,0,NULL,NULL);

       GetPanelAttribute (panelHandle, ATTR_TITLE, panelString);

       foundItem = ListFindItem(*panelList,panelString,FRONT_OF_LIST,PanelCompare);
       panelStruct = ListGetPtrToItem(*panelList,foundItem);

       strcpy(childWindowMenu,panelStruct->panelCBName);

       sprintf(panelString,"Panel Callback: %s",strlen(childWindowMenu)==0?"<null>":childWindowMenu);
       NewMenuItem(menuBarHandle,menuID,panelString,-1,0,NULL,NULL);

       InsertSeparator(menuBarHandle,menuID,-1);

       GetPanelAttribute (panelHandle, ATTR_PANEL_PARENT, &isChildPanel);

       if(!isChildPanel)
        sprintf(childWindowMenu,"Confine Window");
       else
        sprintf(childWindowMenu,"Release Window");

       releaseID = NewMenuItem (menuBarHandle, menuID, childWindowMenu, -1, 0, 0, 0);

       if( (releaseID == RunPopupMenu (menuBarHandle, menuID, panelHandle, eventData1, eventData2, 0, 0, 0, 0)) )
       {

        SetPanelParenthood(panelHandle,isChildPanel);
       }

       DiscardMenu(menuBarHandle,menuID);
       DiscardMenuBar(menuBarHandle);

   break;
  }
  return 0;
}

// Load the UIR and all its panels into memory and add
// to the tree control
int LoadUIR(char* panelPath)
{
    int breakprevState;
    int panelID=1;
    int currentPanelHandle = 0;
    ListType *list = NULL;
    char UIRName[MAX_FILENAME_LEN];
    UIRSTRUCT UIRItem;
    ListType* panelListType = NULL;
    int validUIR = 0;

    list = GetUIRList();
    SplitPath (panelPath, NULL, NULL, UIRName);

    strcpy(UIRItem.UIRPath, panelPath);
    strcpy(UIRItem.UIRName, UIRName);

    panelListType = (ListType*)malloc(sizeof(ListType));
    *panelListType = ListCreate(sizeof(PANELSTRUCT));
    UIRItem.panelList = panelListType;

    breakprevState = SetBreakOnLibraryErrors (0);

    while( (currentPanelHandle = LoadPanel(GetChildPanel(SUB_PANEL), panelPath, panelID))>0)
    {
        char panelTitle[MAX_STRING_SIZE],panelCBName[MAX_STRING_SIZE];
        size_t duplicateItem = 0;
        PANELSTRUCT panelStruct;

        //atleast one panel was loaded, therefor the UIR is valid
        validUIR = 1;

        GetPanelAttribute(currentPanelHandle,ATTR_TITLE,panelTitle);
        GetPanelAttribute(currentPanelHandle,ATTR_CALLBACK_NAME,panelCBName);

        if(!strlen(panelTitle))
        	sprintf(panelTitle,"<No Panel Title>");

        //check to see if a panel with the same title already exists in the UIR. If it does, append
        //a number to the title to be able to differentiate it.
        if( (duplicateItem = ListFindItem(*panelListType,panelTitle,FRONT_OF_LIST,PanelCompare)) == 0 )
        {
            strcpy(panelStruct.panelName,panelTitle);
        }
        else
        {
            char tempPanelName[MAX_STRING_SIZE];
            int duplicateCounter = 0;
            size_t duplicateTemp = duplicateItem;

            while( (duplicateTemp = ListFindItem(*panelListType,panelTitle,duplicateTemp,PanelCompare)) !=0)
            {
                duplicateItem = duplicateTemp;
                duplicateTemp++;
                duplicateCounter++;
            }

            sprintf(tempPanelName,"%s(%d)",panelTitle,duplicateCounter);

            //change the title of the panel
            SetPanelAttribute(currentPanelHandle,ATTR_TITLE,tempPanelName);
            strcpy(panelStruct.panelName,tempPanelName);
        }

        strcpy(panelStruct.panelCBName,panelCBName);

        panelStruct.panelID = panelID;
        panelStruct.panelHandle = currentPanelHandle;
        panelStruct.ctrlArray = NULL;
        panelStruct.numCtrls = 0;

        EnumerateCtrls(&panelStruct);
        strcpy(panelStruct.panelCBName,panelCBName);

        InstallPanelCallback(panelStruct.panelHandle,RuntimePanelCB,panelListType);

        ListInsertItem (*panelListType, &panelStruct, END_OF_LIST);
        panelID++;
    } // while LoadPanel

	if(!validUIR)
	{
		char errorString[MAX_STRING_SIZE];
		sprintf(errorString,"Error loading UIR at %s.",panelPath);
		MessagePopup("Loading Error",errorString);
		ListDispose (*panelListType);
		free(panelListType);
		return 0;
	}

    ListInsertItem (*list,&UIRItem,END_OF_LIST);
    SetBreakOnLibraryErrors (breakprevState);

    //Add the loaded UIR to the tree control
    AddUIRtoTree(panelPath);

    return 1;
}

int EnumerateCtrls(PANELSTRUCT* panelStruct)
{
    int ctrlID;
    int numControls;
    int counter = 0;
    int panelHandle = panelStruct->panelHandle;
    int CBlenght =0;

    GetPanelAttribute (panelHandle, ATTR_NUM_CTRLS, &numControls);

    //return if the panel contains no controls
    if(!numControls) return 0;

    if(!panelStruct->ctrlArray)
    {
        panelStruct->ctrlArray = calloc(numControls,sizeof(CTRLSTRUCT));
        panelStruct->numCtrls = numControls;
    }

    GetPanelAttribute(panelHandle,ATTR_PANEL_FIRST_CTRL,&ctrlID);

    //install callbacks for the panel controls
    for(counter=0;counter<numControls;counter++)
    {
        char** pCtrlCBName = &panelStruct->ctrlArray[counter].ctrlCBName;

        //allocate memory only if previously unallocated
        if(!*pCtrlCBName)
        {
            GetCtrlAttribute(panelHandle,ctrlID,ATTR_CALLBACK_NAME_LENGTH,&CBlenght);
            *pCtrlCBName = (char*) calloc(CBlenght+1,sizeof(char));
        }

        GetCtrlAttribute(panelHandle,ctrlID,ATTR_CALLBACK_NAME,*pCtrlCBName);
        panelStruct->ctrlArray[counter].ctrlID = ctrlID;

        InstallCtrlCallback(panelHandle, ctrlID, RuntimeCtrlCB, panelStruct->ctrlArray);

        GetCtrlAttribute(panelHandle,ctrlID,ATTR_NEXT_CTRL,&ctrlID);
    }

    return 0;
}

// Changes the parent panel associated with the panel, depending on
// whether the panel already has a parent or not.Search the list for
// the panel handle since we need to reload the panel to release the window
void SetPanelParenthood(int panelHandle,int isChildPanel)
{
    int foundPosition =0 ;
    size_t uirListSize =0;
    size_t panelListSize =0;
    int uirCounter,panelCounter;
    UIRSTRUCT uirStruct;
    PANELSTRUCT *panelStruct;

    ListType* uirList = GetUIRList();

    uirListSize =  ListNumItems(*uirList);

    //search the UIR list of the correct panel handle and obtain the
    //path to the UIR so the panel can be reloaded as the child/non-child
    //panel, which ever is required.
	for(uirCounter =1; uirCounter<=uirListSize; uirCounter++)
	{
		ListGetItem(*uirList,&uirStruct,uirCounter);
		panelListSize = ListNumItems(*uirStruct.panelList);

		panelStruct = ListGetDataPtr(*uirStruct.panelList);

		for(panelCounter=0; panelCounter<panelListSize; panelCounter++)
		{
			if (panelStruct[panelCounter].panelHandle == panelHandle)
			{
			 	foundPosition = 1;
			 	DiscardPanel(panelHandle);

			 	int nParentPanel = (isChildPanel == 0) ? GetChildPanel(SUB_PANEL) : 0;
				panelHandle = LoadPanel(nParentPanel, uirStruct.UIRPath, panelStruct[panelCounter].panelID);

			 	panelStruct[panelCounter].panelHandle = panelHandle;

			 	EnumerateCtrls(&panelStruct[panelCounter]);

			 	InstallPanelCallback(panelHandle,RuntimePanelCB,uirStruct.panelList);
			 	DisplayPanel(panelHandle);
			 	break;
			}
		}
       	if (foundPosition)
        	break;
    }
}


//Tree callbacks
int CVICALLBACK TreeCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int column;
    int index;
    int area;
    char itemTag[260];
    char itemLabel[260];
    ListType *UIRList = NULL;
    switch (event)
    {
        case EVENT_RIGHT_CLICK:

        GetIndexFromPoint (panel, PANEL_TREE, MakePoint (eventData2, eventData1), &index, &area, &column);
        if(area == VAL_ITEM_LABEL_AREA)
        {
           CallBackData *data = (CallBackData*)calloc(1,sizeof(CallBackData));
           data->panelHandle = panel;
           data->treeItemIndex = index;
           data->point = MakePoint (eventData2, eventData1);

           PostDeferredCall(DeferredMenuCB,(void*)data);
        }
        break;
        case EVENT_MARK_STATE_CHANGE:

        GetTreeItemAttribute (panel, PANEL_TREE, eventData2, ATTR_LABEL_TEXT, itemLabel);
        GetTreeItemTag (panel, PANEL_TREE, eventData2, itemTag);

        UIRList = GetUIRList();
        Assert(UIRList !=NULL);

        //check to see if item that was selected in a panel or a uir
        if ( strncmp (itemTag, PANEL_TAG, strlen(PANEL_TAG)) == 0)
        {
            int parentIndex;
            size_t item = 0;
            UIRSTRUCT *UIRItem = NULL;
            PANELSTRUCT *panelStruct = NULL;

            GetTreeItemParent (panel, PANEL_TREE, eventData2, &parentIndex);
            GetTreeItemAttribute (panel, PANEL_TREE, parentIndex, ATTR_LABEL_TEXT, itemLabel);
            item = ListFindItem (*UIRList, itemLabel, FRONT_OF_LIST, UIRPathCompare);

            UIRItem = ListGetPtrToItem(*UIRList,item);


            GetTreeItemAttribute(panel,PANEL_TREE,eventData2,ATTR_LABEL_TEXT,itemLabel);

            item = ListFindItem(*(UIRItem->panelList),itemLabel,FRONT_OF_LIST,PanelCompare);
            panelStruct = ListGetPtrToItem(*(UIRItem->panelList),item);

            if(eventData1)
            {
                DisplayPanel(panelStruct->panelHandle);
            }
            else
            {
                HidePanel(panelStruct->panelHandle);
            }

        }
        break;
    }
    return 0;
}

