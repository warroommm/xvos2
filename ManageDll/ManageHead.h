
#ifndef _MANAGE_LIB_FOR_C_H
#define _MANAGE_LIB_FOR_C_H
#include <windows.h>
#include <winuser.h>

#ifdef __cplusplus
	extern "C"{
#endif


//int InitUIForDLL (HWND hParent);

void DiscardUIObjectsForDLLM (void);

int InitUI(int nParent, intptr_t hParent, int msg);		
//int ShowUI(int bshow);
int InitUiTab(int nParent);
//int ShowUIList(int bshow);
int GetDBData(void** pOutData);
		
#ifdef __cplusplus
	}
#endif

#endif
