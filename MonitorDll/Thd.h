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

#define  PANEL                            1       /* callback function: MainCallback */
#define  PANEL_SPLITTER_H                 2       /* control type: splitter, callback function: (none) */
#define  PANEL_NUMERIC_Y1                 3       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC                    4       /* control type: numeric, callback function: (none) */
#define  PANEL_CB_RESTORE                 5       /* control type: command, callback function: RestoreCB */
#define  PANEL_COMMANDBUTTON              6       /* control type: command, callback function: QuitCommandBtn02 */
#define  PANEL_BUTTON_SHOW                7       /* control type: command, callback function: BtnShowCallback */
#define  PANEL_BUTTON_SHOW_TXT            8       /* control type: command, callback function: BtnShowTxtCallback */
#define  PANEL_INCBUTTON                  9       /* control type: command, callback function: MoveBtnCB */
#define  PANEL_DECBUTTON                  10      /* control type: command, callback function: MoveBtnCB */
#define  PANEL_NUMERIC_Y2                 11      /* control type: numeric, callback function: (none) */
#define  PANEL_SPLITTER_V                 12      /* control type: splitter, callback function: (none) */


     /* Control Arrays: */

#define  CTRLARRAY                        1

     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK BtnShowCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK BtnShowTxtCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MoveBtnCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitCommandBtn02(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RestoreCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
