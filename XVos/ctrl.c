//	This file contains various utility functions used for the controls that are
//	loaded in the panels at run-time.


#include "data.h"
#include "utils.h"
#include "pnl.h"

extern int gbCtrlCB;

//searches the CTRLSTRUCT array for a particular control ID and
//returns the callback name associated with it
char* FindCtrlCBName(CTRLSTRUCT* ctrlStruct,int ctrlID, int numCtrls)
{
    int counter;
    char* CBName = NULL;
    for(counter=0; counter< numCtrls; counter++)
    {
		if(ctrlStruct[counter].ctrlID == ctrlID)
		{
			CBName = ctrlStruct[counter].ctrlCBName;
			break;
		}
    }

    return CBName;
}

//callback associated with the controls on a panel that is loaded at run-time
int CVICALLBACK RuntimeCtrlCB (int panelHandle, int control, int event, void *callbackdata, int eventData1, int eventData2)
{
	char childWindowMenu[MAX_STRING_SIZE];
	int isChildPanel;
	int numCtrls;
	int menuBarHandle;
	int menuID;
	int ctrlType;
	char ctrlString[MAX_STRING_SIZE];
	char menuString[MAX_STRING_SIZE];
	CTRLSTRUCT* ctrlArray = callbackdata;
	int releaseID =0;

	switch(event)
	{
		case EVENT_RIGHT_CLICK:
			if(ctrlArray == NULL)
				return 0;

			menuBarHandle = NewMenuBar (0);
			menuID = NewMenu(menuBarHandle,"",-1);

			GetCtrlAttribute(panelHandle,control,ATTR_CTRL_STYLE,&ctrlType);

			GetStyleNameForCtrl(ctrlType, ctrlString);
			sprintf(menuString,"Control Type: %s",ctrlString);

			NewMenuItem(menuBarHandle,menuID,menuString,-1,0,NULL,NULL);

			GetPanelAttribute(panelHandle,ATTR_NUM_CTRLS,&numCtrls);

			strcpy(ctrlString, FindCtrlCBName(ctrlArray,control,numCtrls));

			sprintf(menuString,"Ctrl Callback: %s",strlen(ctrlString)==0?"<null>":ctrlString);

			NewMenuItem(menuBarHandle,menuID,menuString,-1,0,NULL,NULL);

			InsertSeparator(menuBarHandle, menuID, -1);
			GetPanelAttribute (panelHandle, ATTR_PANEL_PARENT, &isChildPanel);

			if(!isChildPanel)
			   sprintf(childWindowMenu,"Confine Window");
			else
			   sprintf(childWindowMenu,"Release Window");

			releaseID = NewMenuItem (menuBarHandle, menuID, childWindowMenu, -1, 0, 0, 0);

			if(releaseID == RunPopupMenu(menuBarHandle,menuID,panelHandle,eventData1,eventData2,0,0,0,0))
			{
			   SetPanelParenthood(panelHandle,isChildPanel);
			}

			DiscardMenu(menuBarHandle,menuID);
			DiscardMenuBar(menuBarHandle);

			//indicate to the panel that this is a control callback
			gbCtrlCB = 1;
 		break;
	}

	return 0;
}
