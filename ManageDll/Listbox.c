#include <ansi_c.h>
#include <userint.h>
#include "ManageDll.h"
#include "../MonitorDll/Tlib.h"

extern intptr_t ghParent;
extern int gmsg;

int CVICALLBACK ListboxCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int index = 0;
	char gValue[256];
	switch (event)
	{
		case EVENT_COMMIT:

			break;
		case EVENT_LEFT_DOUBLE_CLICK:
			if (GetCtrlIndex(panel, control, &index) >= 0)
			{
			}
			if (GetCtrlVal(panel, control, gValue) >= 0)
			{
				//PutWave(szValue);
				//unsigned int wParam = index;
				
				SendMessage ((HWND)ghParent, gmsg, (WPARAM)index, (LPARAM)&gValue);
				//printf("send over.\n");
			}
			break;
		case EVENT_LEFT_CLICK_UP:
			if (GetCtrlIndex(panel, control, &index) >= 0)
			{
				//printf("list box :%d\n", index);
			}
			if (GetCtrlVal(panel, control, gValue) >= 0)
			{
				//printf("list box val:%s\n", szValue);
			}
			break;
	}
	return 0;
}
