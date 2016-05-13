#ifndef _UTILS_HEADER_
#define _UTILS_HEADER_

#include <toolbox.h>

//Cleanup routines
void RemoveUIRfromListIndex(size_t index);
void RemoveUIRFromList(char* UIRPath);
void CleanUpLists(void);

//Shared data functions
ListType* 	GetUIRList(void);

int 		GetMainPanel(void);
int 		GetChildPanel(int nChildID);
int 		SetDllPanel(int nDllPanel);
int			GetDllPanel();
int 		SetManagerPanel(int nMngPanel, int nMngPanellist);
int			GetManagerPanel(int IsPanellist);

//utility functions
int GetStyleNameForCtrl (int ctrlStyle, char* ctrlName);
int CVICALLBACK UIRPathCompare(void *item1, void *item2);
int CVICALLBACK PanelCompare(void *item1, void *item2);

#endif //_DATA_HEADER_
