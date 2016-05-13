#ifndef _PANEL_HEADER_
#define _PANEL_HEADER_

#include "data.h"

int LoadUIR(char* panelPath);
int EnumerateCtrls(PANELSTRUCT* panelStruct);
void SetPanelParenthood(int panelHandle, int isChildPanel);

#endif
