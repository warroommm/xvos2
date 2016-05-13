
#ifndef _THD_LIB_FOR_C_H
#define _THD_LIB_FOR_C_H
#include <windows.h>
#include <winuser.h>

#ifdef __cplusplus
	extern "C"{
#endif


//int InitUIForDLL (HWND hParent);

void DiscardUIObjectsForDLLF (void);

int InitUIall(int nParentHandle);		
int ShowUIall(int bshow);
int PutWave(const char* pWaveFile);
		
#ifdef __cplusplus
	}
#endif

#endif
