#include "data.h"
#include "utils.h"
#include "XVos.h"
#include "menu.h"

//List searching functions

// allows searching for the item that contains the specified UIR Path in the
// UIR link list
int CVICALLBACK UIRPathCompare(void *item1, void *item2)
{
   UIRSTRUCT *uirStruct = NULL;

   uirStruct = (UIRSTRUCT*)item2;

   return strncmp((const char*)item1, uirStruct->UIRPath, strlen(uirStruct->UIRPath));
}

// allows searching for item containing the specified panel name in the panel linked list
int CVICALLBACK PanelCompare(void *item1, void *item2)
{
   PANELSTRUCT * panelStruct = NULL;

   panelStruct = (PANELSTRUCT*)item2;

   return strncmp((const char*)item1,panelStruct->panelName,strlen((const char*)item1));
}


//Cleanup functions

// This function searches for a UIR in the UIR List based on the path
// and removes it from the linked list.
void RemoveUIRFromList(char* UIRPath)
{
    size_t foundIndex = 0;
    ListType* UIRList = GetUIRList();

    foundIndex = ListFindItem(*UIRList,UIRPath,FRONT_OF_LIST,UIRPathCompare);

    if(foundIndex)
    {
        RemoveUIRfromListIndex(foundIndex);
    }

}

// Removes UIRs from the link list based on its
// location in the list
void RemoveUIRfromListIndex(size_t foundIndex)
{
    UIRSTRUCT UIRStruct;
    PANELSTRUCT panelStruct;
    ListType * panelList;
    size_t numItems =0;
    int panelCounter,ctrlCounter;
    ListType* UIRList = GetUIRList();

    ListRemoveItem(*UIRList,&UIRStruct,foundIndex);

    panelList = UIRStruct.panelList;

    numItems = ListNumItems(*panelList);

    for(panelCounter=0;panelCounter<numItems;panelCounter++)
    {

       ListRemoveItem (*panelList, &panelStruct, FRONT_OF_LIST);

       for(ctrlCounter=0;ctrlCounter<panelStruct.numCtrls;ctrlCounter++)
       {
           free(panelStruct.ctrlArray[ctrlCounter].ctrlCBName);
       }

       DiscardPanel(panelStruct.panelHandle);
       free(panelStruct.ctrlArray);
    }

    ListDispose(*panelList);
    free(panelList);
}



//Cleans up the entire UIR linked list
void CleanUpLists(void)
{
  size_t listNumItems;
  int UIRcounter;
  ListType* UIRList = GetUIRList();

  listNumItems = ListNumItems (*UIRList);

  for (UIRcounter=1; UIRcounter<=listNumItems; UIRcounter++)
  {
     	RemoveUIRfromListIndex(FRONT_OF_LIST);
  }

  ListDispose(*UIRList);

  // saves MRU list and cleans up any objects
  // associated with it
  SaveMRUList();
}


int GetStyleNameForCtrl (int ctrlStyle, char* ctrlName)
{
    switch(ctrlStyle)
    {
        case CTRL_RECESSED_MENU_RING        : sprintf(ctrlName, "CTRL_RECESSED_MENU_RING                "); break;
        case CTRL_MENU_RING                 : sprintf(ctrlName, "CTRL_MENU_RING                         "); break;
        case CTRL_RING_VSLIDE               : sprintf(ctrlName, "CTRL_RING_VSLIDE                       "); break;
        case CTRL_RING_HSLIDE               : sprintf(ctrlName, "CTRL_RING_HSLIDE                       "); break;
        case CTRL_RING_FLAT_VSLIDE          : sprintf(ctrlName, "CTRL_RING_FLAT_VSLIDE                  "); break;
        case CTRL_RING_FLAT_HSLIDE          : sprintf(ctrlName, "CTRL_RING_FLAT_HSLIDE                  "); break;
        case CTRL_RING_LEVEL_VSLIDE         : sprintf(ctrlName, "CTRL_RING_LEVEL_VSLIDE                 "); break;
        case CTRL_RING_LEVEL_HSLIDE         : sprintf(ctrlName, "CTRL_RING_POINTER_VSLIDE               "); break;
        case CTRL_RING_POINTER_VSLIDE       : sprintf(ctrlName, "CTRL_RING_POINTER_VSLIDE               "); break;
        case CTRL_RING_POINTER_HSLIDE       : sprintf(ctrlName, "CTRL_RING_POINTER_HSLIDE               "); break;
        case CTRL_RING_THERMOMETER          : sprintf(ctrlName, "CTRL_RING_THERMOMETER                  "); break;
        case CTRL_RING_TANK                 : sprintf(ctrlName, "CTRL_RING_TANK                         "); break;
        case CTRL_RING_GAUGE                : sprintf(ctrlName, "CTRL_RING_GAUGE                        "); break;
        case CTRL_RING_METER                : sprintf(ctrlName, "CTRL_RING_METER                        "); break;
        case CTRL_RING_KNOB                 : sprintf(ctrlName, "CTRL_RING_KNOB                         "); break;
        case CTRL_RING_DIAL                 : sprintf(ctrlName, "CTRL_RING_DIAL                         "); break;
        case CTRL_PICTURE_RING              : sprintf(ctrlName, "CTRL_PICTURE_RING                      "); break;
        case CTRL_LIST                      : sprintf(ctrlName, "CTRL_LIST                              "); break;
        case CTRL_RAISED_BOX                : sprintf(ctrlName, "CTRL_RAISED_BOX                        "); break;
        case CTRL_RECESSED_BOX              : sprintf(ctrlName, "CTRL_RECESSED_BOX                      "); break;
        case CTRL_FLAT_BOX                  : sprintf(ctrlName, "CTRL_FLAT_BOX                          "); break;
        case CTRL_RAISED_CIRCLE             : sprintf(ctrlName, "CTRL_RAISED_CIRCLE                     "); break;
        case CTRL_RECESSED_CIRCLE           : sprintf(ctrlName, "CTRL_RECESSED_CIRCLE                   "); break;
        case CTRL_FLAT_CIRCLE               : sprintf(ctrlName, "CTRL_FLAT_CIRCLE                       "); break;
        case CTRL_RAISED_FRAME              : sprintf(ctrlName, "CTRL_RAISED_FRAME                      "); break;
        case CTRL_RECESSED_FRAME            : sprintf(ctrlName, "CTRL_RECESSED_FRAME                    "); break;
        case CTRL_FLAT_FRAME                : sprintf(ctrlName, "CTRL_FLAT_FRAME                        "); break;
        case CTRL_RAISED_ROUND_FRAME        : sprintf(ctrlName, "CTRL_RAISED_ROUND_FRAME                "); break;
        case CTRL_RECESSED_ROUND_FRAME      : sprintf(ctrlName, "CTRL_RECESSED_ROUND_FRAME              "); break;
        case CTRL_FLAT_ROUND_FRAME          : sprintf(ctrlName, "CTRL_FLAT_ROUND_FRAME                  "); break;
        case CTRL_RAISED_ROUNDED_BOX        : sprintf(ctrlName, "CTRL_RAISED_ROUNDED_BOX                "); break;
        case CTRL_RECESSED_ROUNDED_BOX      : sprintf(ctrlName, "CTRL_RECESSED_ROUNDED_BOX              "); break;
        case CTRL_FLAT_ROUNDED_BOX          : sprintf(ctrlName, "CTRL_FLAT_ROUNDED_BOX                  "); break;
        case CTRL_GRAPH                     : sprintf(ctrlName, "CTRL_GRAPH                             "); break;
        case CTRL_STRIP_CHART               : sprintf(ctrlName, "CTRL_STRIP_CHART                       "); break;
        case CTRL_PICTURE                   : sprintf(ctrlName, "CTRL_PICTURE                           "); break;
        case CTRL_TIMER                     : sprintf(ctrlName, "CTRL_TIMER                             "); break;
        case CTRL_CANVAS                    : sprintf(ctrlName, "CTRL_CANVAS                            "); break;
        case CTRL_TABLE                     : sprintf(ctrlName, "CTRL_TABLE                             "); break;
        case CTRL_NUMERIC_LS                : sprintf(ctrlName, "CTRL_NUMERIC_LS                        "); break;
        case CTRL_NUMERIC_THERMOMETER_LS    : sprintf(ctrlName, "CTRL_NUMERIC_THERMOMETER_LS            "); break;
        case CTRL_NUMERIC_TANK_LS           : sprintf(ctrlName, "CTRL_NUMERIC_TANK_LS                   "); break;
        case CTRL_NUMERIC_GAUGE_LS          : sprintf(ctrlName, "CTRL_NUMERIC_GAUGE_LS                  "); break;
        case CTRL_NUMERIC_METER_LS          : sprintf(ctrlName, "CTRL_NUMERIC_METER_LS                  "); break;
        case CTRL_NUMERIC_KNOB_LS           : sprintf(ctrlName, "CTRL_NUMERIC_KNOB_LS                   "); break;
        case CTRL_NUMERIC_DIAL_LS           : sprintf(ctrlName, "CTRL_NUMERIC_DIAL_LS                   "); break;
        case CTRL_NUMERIC_LEVEL_VSLIDE_LS   : sprintf(ctrlName, "CTRL_NUMERIC_LEVEL_VSLIDE_LS           "); break;
        case CTRL_NUMERIC_LEVEL_HSLIDE_LS   : sprintf(ctrlName, "CTRL_NUMERIC_LEVEL_HSLIDE_LS           "); break;
        case CTRL_NUMERIC_POINTER_VSLIDE_LS : sprintf(ctrlName, "CTRL_NUMERIC_POINTER_VSLIDE_LS         "); break;
        case CTRL_NUMERIC_POINTER_HSLIDE_LS : sprintf(ctrlName, "CTRL_NUMERIC_POINTER_HSLIDE_LS         "); break;
        case CTRL_COLOR_NUMERIC_LS          : sprintf(ctrlName, "CTRL_COLOR_NUMERIC_LS                  "); break;
        case CTRL_STRING_LS                 : sprintf(ctrlName, "CTRL_STRING_LS                         "); break;
        case CTRL_TEXT_BOX_LS               : sprintf(ctrlName, "CTRL_TEXT_BOX_LS                       "); break;
        case CTRL_SQUARE_COMMAND_BUTTON_LS  : sprintf(ctrlName, "CTRL_SQUARE_COMMAND_BUTTON_LS          "); break;
        case CTRL_PICTURE_COMMAND_BUTTON_LS : sprintf(ctrlName, "CTRL_PICTURE_COMMAND_BUTTON_LS         "); break;
        case CTRL_SQUARE_BUTTON_LS          : sprintf(ctrlName, "CTRL_SQUARE_BUTTON_LS                  "); break;
        case CTRL_PICTURE_TOGGLE_BUTTON_LS  : sprintf(ctrlName, "CTRL_PICTURE_TOGGLE_BUTTON_LS          "); break;
        case CTRL_SQUARE_PUSH_BUTTON_LS     : sprintf(ctrlName, "CTRL_SQUARE_PUSH_BUTTON_LS             "); break;
        case CTRL_SQUARE_TEXT_BUTTON_LS     : sprintf(ctrlName, "CTRL_SQUARE_TEXT_BUTTON_LS             "); break;
        case CTRL_ROUND_LED_LS              : sprintf(ctrlName, "CTRL_ROUND_LED_LS                      "); break;
        case CTRL_SQUARE_LED_LS             : sprintf(ctrlName, "CTRL_SQUARE_LED_LS                     "); break;
        case CTRL_TOGGLE_HSWITCH_LS         : sprintf(ctrlName, "CTRL_TOGGLE_HSWITCH_LS                 "); break;
        case CTRL_TOGGLE_VSWITCH_LS         : sprintf(ctrlName, "CTRL_TOGGLE_VSWITCH_LS                 "); break;
        case CTRL_RING_LS                   : sprintf(ctrlName, "CTRL_RING_LS                           "); break;
        case CTRL_RECESSED_MENU_RING_LS     : sprintf(ctrlName, "CTRL_RECESSED_MENU_RING_LS             "); break;
        case CTRL_MENU_RING_LS              : sprintf(ctrlName, "CTRL_MENU_RING_LS                      "); break;
        case CTRL_POPUP_MENU_RING_LS        : sprintf(ctrlName, "CTRL_POPUP_MENU_RING_LS                "); break;
        case CTRL_RING_LEVEL_VSLIDE_LS      : sprintf(ctrlName, "CTRL_RING_LEVEL_VSLIDE_LS              "); break;
        case CTRL_RING_LEVEL_HSLIDE_LS      : sprintf(ctrlName, "CTRL_RING_LEVEL_HSLIDE_LS              "); break;
        case CTRL_RING_POINTER_VSLIDE_LS    : sprintf(ctrlName, "CTRL_RING_POINTER_VSLIDE_LS            "); break;
        case CTRL_RING_POINTER_HSLIDE_LS    : sprintf(ctrlName, "CTRL_RING_POINTER_HSLIDE_LS            "); break;
        case CTRL_RING_THERMOMETER_LS       : sprintf(ctrlName, "CTRL_RING_THERMOMETER_LS               "); break;
        case CTRL_RING_TANK_LS              : sprintf(ctrlName, "CTRL_RING_TANK_LS                      "); break;
        case CTRL_RING_GAUGE_LS             : sprintf(ctrlName, "CTRL_RING_GAUGE_LS                     "); break;
        case CTRL_RING_METER_LS             : sprintf(ctrlName, "CTRL_RING_METER_LS                     "); break;
        case CTRL_RING_KNOB_LS              : sprintf(ctrlName, "CTRL_RING_KNOB_LS                      "); break;
        case CTRL_RING_DIAL_LS              : sprintf(ctrlName, "CTRL_RING_DIAL_LS                      "); break;
        case CTRL_PICTURE_RING_LS           : sprintf(ctrlName, "CTRL_PICTURE_RING_LS                   "); break;
        case CTRL_LIST_LS                   : sprintf(ctrlName, "CTRL_LIST_LS                           "); break;
        case CTRL_RAISED_BOX_LS             : sprintf(ctrlName, "CTRL_RAISED_BOX_LS                     "); break;
        case CTRL_RECESSED_BOX_LS           : sprintf(ctrlName, "CTRL_RECESSED_BOX_LS                   "); break;
        case CTRL_SMOOTH_VERTICAL_BOX_LS    : sprintf(ctrlName, "CTRL_SMOOTH_VERTICAL_BOX_LS            "); break;
        case CTRL_SMOOTH_HORIZONTAL_BOX_LS  : sprintf(ctrlName, "CTRL_SMOOTH_HORIZONTAL_BOX_LS          "); break;
        case CTRL_RECESSED_NARROW_FRAME     : sprintf(ctrlName, "CTRL_RECESSED_NARROW_FRAME             "); break;
        case CTRL_GRAPH_LS                  : sprintf(ctrlName, "CTRL_GRAPH_LS                          "); break;
        case CTRL_STRIP_CHART_LS            : sprintf(ctrlName, "CTRL_STRIP_CHART_LS                    "); break;
        case CTRL_PICTURE_LS                : sprintf(ctrlName, "CTRL_PICTURE_LS                        "); break;
        case CTRL_TABLE_LS                  : sprintf(ctrlName, "CTRL_TABLE_LS                          "); break;
        case CTRL_NUMERIC                   : sprintf(ctrlName, "CTRL_NUMERIC                           "); break;
        case CTRL_NUMERIC_THERMOMETER       : sprintf(ctrlName, "CTRL_NUMERIC_THERMOMETER               "); break;
        case CTRL_NUMERIC_TANK              : sprintf(ctrlName, "CTRL_NUMERIC_TANK                      "); break;
        case CTRL_NUMERIC_GAUGE             : sprintf(ctrlName, "CTRL_NUMERIC_GAUGE                     "); break;
        case CTRL_NUMERIC_METER             : sprintf(ctrlName, "CTRL_NUMERIC_METER                     "); break;
        case CTRL_NUMERIC_KNOB              : sprintf(ctrlName, "CTRL_NUMERIC_KNOB                      "); break;
        case CTRL_NUMERIC_DIAL              : sprintf(ctrlName, "CTRL_NUMERIC_DIAL                      "); break;
        case CTRL_NUMERIC_VSLIDE            : sprintf(ctrlName, "CTRL_NUMERIC_VSLIDE                    "); break;
        case CTRL_NUMERIC_HSLIDE            : sprintf(ctrlName, "CTRL_NUMERIC_HSLIDE                    "); break;
        case CTRL_NUMERIC_FLAT_VSLIDE       : sprintf(ctrlName, "CTRL_NUMERIC_FLAT_VSLIDE               "); break;
        case CTRL_NUMERIC_FLAT_HSLIDE       : sprintf(ctrlName, "CTRL_NUMERIC_FLAT_HSLIDE               "); break;
        case CTRL_NUMERIC_LEVEL_VSLIDE      : sprintf(ctrlName, "CTRL_NUMERIC_LEVEL_VSLIDE              "); break;
        case CTRL_NUMERIC_LEVEL_HSLIDE      : sprintf(ctrlName, "CTRL_NUMERIC_LEVEL_HSLIDE              "); break;
        case CTRL_NUMERIC_POINTER_VSLIDE    : sprintf(ctrlName, "CTRL_NUMERIC_POINTER_VSLIDE            "); break;
        case CTRL_NUMERIC_POINTER_HSLIDE    : sprintf(ctrlName, "CTRL_NUMERIC_POINTER_HSLIDE            "); break;
        case CTRL_COLOR_NUMERIC             : sprintf(ctrlName, "CTRL_COLOR_NUMERIC                     "); break;
        case CTRL_STRING                    : sprintf(ctrlName, "CTRL_STRING                            "); break;
        case CTRL_TEXT_MSG                  : sprintf(ctrlName, "CTRL_TEXT_MSG                          "); break;
        case CTRL_TEXT_BOX                  : sprintf(ctrlName, "CTRL_TEXT_BOX                          "); break;
        case CTRL_SQUARE_COMMAND_BUTTON     : sprintf(ctrlName, "CTRL_SQUARE_COMMAND_BUTTON             "); break;
        case CTRL_OBLONG_COMMAND_BUTTON     : sprintf(ctrlName, "CTRL_OBLONG_COMMAND_BUTTON             "); break;
        case CTRL_ROUND_COMMAND_BUTTON      : sprintf(ctrlName, "CTRL_ROUND_COMMAND_BUTTON              "); break;
        case CTRL_ROUNDED_COMMAND_BUTTON    : sprintf(ctrlName, "CTRL_ROUNDED_COMMAND_BUTTON            "); break;
        case CTRL_PICTURE_COMMAND_BUTTON    : sprintf(ctrlName, "CTRL_PICTURE_COMMAND_BUTTON            "); break;
        case CTRL_ROUND_BUTTON              : sprintf(ctrlName, "CTRL_ROUND_BUTTON                      "); break;
        case CTRL_SQUARE_BUTTON             : sprintf(ctrlName, "CTRL_SQUARE_BUTTON                     "); break;
        case CTRL_ROUND_FLAT_BUTTON         : sprintf(ctrlName, "CTRL_ROUND_FLAT_BUTTON                 "); break;
        case CTRL_SQUARE_FLAT_BUTTON        : sprintf(ctrlName, "CTRL_SQUARE_FLAT_BUTTON                "); break;
        case CTRL_ROUND_RADIO_BUTTON        : sprintf(ctrlName, "CTRL_ROUND_RADIO_BUTTON                "); break;
        case CTRL_SQUARE_RADIO_BUTTON       : sprintf(ctrlName, "CTRL_SQUARE_RADIO_BUTTON               "); break;
        case CTRL_CHECK_BOX                 : sprintf(ctrlName, "CTRL_CHECK_BOX                         "); break;
        case CTRL_ROUND_PUSH_BUTTON         : sprintf(ctrlName, "CTRL_ROUND_PUSH_BUTTON                 "); break;
        case CTRL_SQUARE_PUSH_BUTTON        : sprintf(ctrlName, "CTRL_SQUARE_PUSH_BUTTON                "); break;
        case CTRL_ROUND_PUSH_BUTTON2        : sprintf(ctrlName, "CTRL_ROUND_PUSH_BUTTON2                "); break;
        case CTRL_SQUARE_PUSH_BUTTON2       : sprintf(ctrlName, "CTRL_SQUARE_PUSH_BUTTON2               "); break;
        case CTRL_SQUARE_TEXT_BUTTON        : sprintf(ctrlName, "CTRL_SQUARE_TEXT_BUTTON                "); break;
        case CTRL_OBLONG_TEXT_BUTTON        : sprintf(ctrlName, "CTRL_OBLONG_TEXT_BUTTON                "); break;
        case CTRL_ROUND_TEXT_BUTTON         : sprintf(ctrlName, "CTRL_ROUND_TEXT_BUTTON                 "); break;
        case CTRL_ROUNDED_TEXT_BUTTON       : sprintf(ctrlName, "CTRL_ROUNDED_TEXT_BUTTON               "); break;
        case CTRL_PICTURE_TOGGLE_BUTTON     : sprintf(ctrlName, "CTRL_PICTURE_TOGGLE_BUTTON             "); break;
        case CTRL_ROUND_LIGHT               : sprintf(ctrlName, "CTRL_ROUND_LIGHT                       "); break;
        case CTRL_SQUARE_LIGHT              : sprintf(ctrlName, "CTRL_SQUARE_LIGHT                      "); break;
        case CTRL_ROUND_LED                 : sprintf(ctrlName, "CTRL_ROUND_LED                         "); break;
        case CTRL_SQUARE_LED                : sprintf(ctrlName, "CTRL_SQUARE_LED                        "); break;
        case CTRL_HSWITCH                   : sprintf(ctrlName, "CTRL_HSWITCH                           "); break;
        case CTRL_VSWITCH                   : sprintf(ctrlName, "CTRL_VSWITCH                           "); break;
        case CTRL_GROOVED_HSWITCH           : sprintf(ctrlName, "CTRL_GROOVED_HSWITCH                   "); break;
        case CTRL_GROOVED_VSWITCH           : sprintf(ctrlName, "CTRL_GROOVED_VSWITCH                   "); break;
        case CTRL_TOGGLE_HSWITCH            : sprintf(ctrlName, "CTRL_TOGGLE_HSWITCH                    "); break;
        case CTRL_TOGGLE_VSWITCH            : sprintf(ctrlName, "CTRL_TOGGLE_VSWITCH                    "); break;
        case CTRL_RING                      : sprintf(ctrlName, "CTRL_RING                              "); break;
        case CTRL_TREE_LS                   : sprintf(ctrlName, "CTRL_TREE_LS                           "); break;
        case CTRL_TREE                      : sprintf(ctrlName, "CTRL_TREE                              "); break;
        case CTRL_POPUP_MENU_RING           : sprintf(ctrlName, "CTRL_POPUP_MENU_RING                   "); break;
        case CTRL_DIGITAL_GRAPH             : sprintf(ctrlName, "CTRL_DIGITAL_GRAPH                     "); break;
        case CTRL_DIGITAL_GRAPH_LS          : sprintf(ctrlName, "CTRL_DIGITAL_GRAPH_LS                  "); break;
        case CTRL_HORIZONTAL_SPLITTER       : sprintf(ctrlName, "CTRL_HORIZONTAL_SPLITTER               "); break;
        case CTRL_VERTICAL_SPLITTER         : sprintf(ctrlName, "CTRL_VERTICAL_SPLITTER                 "); break;
        case CTRL_HORIZONTAL_SPLITTER_LS    : sprintf(ctrlName, "CTRL_HORIZONTAL_SPLITTER_LS            "); break;
        case CTRL_VERTICAL_SPLITTER_LS      : sprintf(ctrlName, "CTRL_VERTICAL_SPLITTER_LS              "); break;
        case CTRL_ACTIVEX                   : sprintf(ctrlName, "CTRL_ACTIVEX                           "); break;
        case CTRL_TABS                      : sprintf(ctrlName, "CTRL_TABS                              "); break;
        default                             : sprintf(ctrlName, "Unknown control                        "); break;
    }
    return 0;
}


// Shared data functions
ListType* GetUIRList(void)
{
     static ListType list = NULL;
     if (list==NULL)
        list = ListCreate (sizeof(UIRSTRUCT));
     return &list;
}

int GetMainPanel()
{
    static int mainPanel = 0;

    if (0 == mainPanel)
    {
        mainPanel = LoadPanel(0, UIRNAME, PANEL);
    }

    return mainPanel;
}

int GetChildPanel(int nChildID)
{
    static int childPanel = 0;

    if (0 == childPanel)
    {
         int mainHandle = GetMainPanel();
         childPanel 	= LoadPanel (mainHandle, UIRNAME, nChildID);	//SUB_PANEL
    }

    return childPanel;
}

static int DllPanel(int nNewPanel)
{
	static int panelDll = -1;
	if (nNewPanel > 0)
		panelDll = nNewPanel;
	return panelDll;
}
int SetDllPanel(int nNewPanel)
{
	return DllPanel(nNewPanel);
}
int GetDllPanel()
{
	return DllPanel(-1);
}

static int ManagerPanel(int nPanel)
{
	static int panelMng = -1;
	if (nPanel > 0)
	{
		panelMng = nPanel;
	}
	return panelMng;
}
static int ManagerPanelList(int nPanelList)
{
	static int panelMngList = -1;
	if (nPanelList > 0)
	{
		panelMngList = nPanelList;
	}
	return panelMngList;
}

int SetManagerPanel(int nPanel, int nMngPanellist)
{
	ManagerPanelList(nMngPanellist);
	
	return ManagerPanel(nPanel);
}
int GetManagerPanel(int IsPanellist)
{
	if (IsPanellist)
		return ManagerPanelList(-1);
	return ManagerPanel(-1);
}
