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

#define  ABOUTPANEL                       1
#define  ABOUTPANEL_CANVAS                2       /* control type: canvas, callback function: AboutCanvasCB */

#define  HELPPANEL                        2
#define  HELPPANEL_TEXTBOX                2       /* control type: textBox, callback function: (none) */
#define  HELPPANEL_CLOSEBUTTON            3       /* control type: command, callback function: (none) */

#define  PANEL                            3       /* callback function: PanelCB */
#define  PANEL_SPLITTER_LEFT_X            2       /* control type: splitter, callback function: (none) */
#define  PANEL_TREE                       3       /* control type: tree, callback function: TreeCB */
#define  PANEL_BOTTOM_SPLITTER            4       /* control type: splitter, callback function: (none) */
#define  PANEL_RIGHT_SPLITTER             5       /* control type: splitter, callback function: (none) */
#define  PANEL_TABLE                      6       /* control type: table, callback function: (none) */
#define  PANEL_MID_SPLITTER               7       /* control type: splitter, callback function: (none) */

#define  SUB_PANEL                        4       /* callback function: SubPanelCB */
#define  SUB_PANEL_SPLITTER               2       /* control type: splitter, callback function: (none) */
#define  SUB_PANEL_SPLITTER_2             3       /* control type: splitter, callback function: (none) */
#define  SUB_PANEL_BACK_PICBTN            4       /* control type: pictButton, callback function: BackLevelCB */
#define  SUB_PANEL_CANVAS                 5       /* control type: canvas, callback function: CanvasCB */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  MAIN                             1
#define  MAIN_FILE                        2
#define  MAIN_FILE_OPEN                   3       /* callback function: FileOpen */
#define  MAIN_FILE_CLOSE                  4       /* callback function: UIRCloseCB */
#define  MAIN_FILE_SEPARATOR              5
#define  MAIN_FILE_ABOVE_EXIT_SEPARATOR   6
#define  MAIN_FILE_EXIT                   7       /* callback function: FileExit */
#define  MAIN_VIEW                        8
#define  MAIN_VIEW_VIEW_ON                9       /* callback function: ShowViewOn */
#define  MAIN_VIEW_VIEW_GRAPH             10      /* callback function: ShowViewGraph */
#define  MAIN_VIEW_VIEW_PARA              11      /* callback function: ShowViewPara */
#define  MAIN_HELP                        12
#define  MAIN_HELP_UIRVIEWER              13      /* callback function: ViewerHelpCB */
#define  MAIN_HELP_SEPARATOR_2            14
#define  MAIN_HELP_ABOUTVIEWER            15      /* callback function: AboutViewerCB */

#define  TREE                             2
#define  TREE_PANEL                       2
#define  TREE_PANEL_MENU_ADD              3       /* callback function: TreeAdd */
#define  TREE_PANEL_MENU_MODIFY           4       /* callback function: TreeModify */
#define  TREE_PANEL_MENU_DELE             5       /* callback function: TreeDele */
#define  TREE_PANEL_SEPARATOR             6
#define  TREE_PANEL_COPY                  7       /* callback function: CopyCB */
#define  TREE_PANEL_SAVE                  8       /* callback function: SaveCB */
#define  TREE_PANEL_PRINT                 9       /* callback function: PrintCB */
#define  TREE_PANEL_ATTRIB                10
#define  TREE_PANEL_ATTRIB_SUBMENU        11
#define  TREE_PANEL_ATTRIB_MAXIMIZE       12      /* callback function: PanelAttribCanMaximizeCB */
#define  TREE_PANEL_ATTRIB_MINIMIZE       13      /* callback function: PanelAttribCanMinimizeCB */
#define  TREE_PANEL_ATTRIB_CLOSE_CTRL     14      /* callback function: CloseControlCB */
#define  TREE_PANEL_ATTRIB_MOVABLE        15      /* callback function: MovableCB */
#define  TREE_PANEL_ATTRIB_SIZABLE        16      /* callback function: SizableCB */
#define  TREE_PANEL_ATTRIB_TITLE_VISIBLE  17      /* callback function: TitleBarVisibleCB */
#define  TREE_PANEL_ATTRIB_SYSTEM_COLORS  18      /* callback function: SystemColorsCB */
#define  TREE_PANEL_ATTRIB_SCALABLE       19      /* callback function: ScalableCB */
#define  TREE_PANEL_ATTRIB_SEPARATOR_3    20
#define  TREE_PANEL_ATTRIB_CLOSE          21      /* callback function: PanelCloseCB */


     /* Callback Prototypes: */

int  CVICALLBACK AboutCanvasCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK AboutViewerCB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK BackLevelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CanvasCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK CloseControlCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CopyCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK FileExit(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK FileOpen(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MovableCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK PanelAttribCanMaximizeCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK PanelAttribCanMinimizeCB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK PanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK PanelCloseCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK PrintCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SaveCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK ScalableCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK ShowViewGraph(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK ShowViewOn(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK ShowViewPara(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SizableCB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK SubPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK SystemColorsCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TitleBarVisibleCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TreeAdd(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK TreeCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK TreeDele(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TreeModify(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK UIRCloseCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK ViewerHelpCB(int menubar, int menuItem, void *callbackData, int panel);


#ifdef __cplusplus
    }
#endif
