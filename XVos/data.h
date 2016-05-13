#ifndef _DATA_HEADER_
#define _DATA_HEADER_

//includes user data types and defines
#include "toolbox.h"

#define PANEL_TAG           "PANEL"
#define UIR_TAG             "UIR"
#define UIRNAME             "XVos.uir"
#define MAX_STRING_SIZE     MAX_PATHNAME_LEN

typedef struct s_CTRL
{
    int ctrlID;
    char* ctrlCBName;
}CTRLSTRUCT;

typedef struct s_PANEL
{
   int panelID;
   int panelHandle;
   char panelCBName[MAX_STRING_SIZE];
   char panelName[MAX_STRING_SIZE];
   int numCtrls;
   CTRLSTRUCT* ctrlArray;
}PANELSTRUCT;

typedef struct s_UIR
{
    char UIRPath[MAX_STRING_SIZE];
    ListType* panelList;
    char UIRName[MAX_STRING_SIZE];

}UIRSTRUCT;

typedef struct _callbackData
{
    int panelHandle;
    int treeItemIndex;
    Point point;
}CallBackData;

#endif      //_DATA_HEADER_
