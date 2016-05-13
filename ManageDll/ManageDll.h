/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2016. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: PanelCB */
#define  PANEL_SPLITTER_V                 2       /* control type: splitter, callback function: (none) */
#define  PANEL_CMDBUTTON_SQLITE           3       /* control type: command, callback function: BtnSqliteCB */
#define  PANEL_CMDBUTTON_JSON             4       /* control type: command, callback function: BtnJsonCB */
#define  PANEL_LISTBOX                    5       /* control type: listBox, callback function: ListboxCB */
#define  PANEL_TREE                       6       /* control type: tree, callback function: TreeCB */
#define  PANEL_SPLITTER_H                 7       /* control type: splitter, callback function: (none) */
#define  PANEL_SPLITTER_H2                8       /* control type: splitter, callback function: (none) */
#define  PANEL_COMMANDBUTTON              9       /* control type: command, callback function: BtnOkCB */

#define  PANEL_LIST                       2       /* callback function: ListPanelCB */
#define  PANEL_LIST_SPLITTER_V            2       /* control type: splitter, callback function: (none) */
#define  PANEL_LIST_TAB                   3       /* control type: tab, callback function: TabCallback */
#define  PANEL_LIST_SPLITTER_H            4       /* control type: splitter, callback function: (none) */

#define  PANEL_MENU                       3
#define  PANEL_MENU_STRING_NAME           2       /* control type: string, callback function: (none) */
#define  PANEL_MENU_CB_OK                 3       /* control type: command, callback function: (none) */
#define  PANEL_MENU_CB_CANCEL             4       /* control type: command, callback function: (none) */

     /* tab page panel controls */
#define  TAB_DEVICE_SPLITTER_V_1          2       /* control type: splitter, callback function: (none) */
#define  TAB_DEVICE_TREE                  3       /* control type: tree, callback function: DeviceTreeCB */
#define  TAB_DEVICE_SPLITTER_H_1          4       /* control type: splitter, callback function: (none) */

     /* tab page panel controls */
#define  TAB_LIB_SPLITTER_V_2             2       /* control type: splitter, callback function: (none) */
#define  TAB_LIB_SPLITTER_H_2             3       /* control type: splitter, callback function: (none) */
#define  TAB_LIB_TABLE                    4       /* control type: table, callback function: (none) */

     /* tab page panel controls */
#define  TAB_UPDATE_SPLITTER_V_3          2       /* control type: splitter, callback function: (none) */
#define  TAB_UPDATE_STRING                3       /* control type: string, callback function: (none) */
#define  TAB_UPDATE_CMDBUTTON_FILE        4       /* control type: command, callback function: (none) */
#define  TAB_UPDATE_CMDBUTTON_SCAN        5       /* control type: command, callback function: (none) */
#define  TAB_UPDATE_CMDBUTTON_UPDATE      6       /* control type: command, callback function: (none) */
#define  TAB_UPDATE_SPLITTER_H_3          7       /* control type: splitter, callback function: (none) */
#define  TAB_UPDATE_TABLE_UPDATE          8       /* control type: table, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK BtnJsonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK BtnOkCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK BtnSqliteCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DeviceTreeCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ListboxCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ListPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TabCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TreeCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
