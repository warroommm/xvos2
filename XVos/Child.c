#include <ansi_c.h>
#include <userint.h>
#include "XVos.h"
#include "Child.h"
#include "utils.h"
#include "ChildData.h"

static char gParentId[256];
static int gCntBtn = 0;
static int* gBtn;
static int CVICALLBACK PicBtnCB(int panel, int ctrl, int event, void *callbackData, int eventData1, int eventData2);

static void DrawBtn(int panel)
{
	int bmpId;
	GetCtrlBitmap(panel, SUB_PANEL_CANVAS, 0, &bmpId);

	int bytePerRow, pixelDepth, width, height, *pColor;
	unsigned char *pBits, *pMask, *pAlph;
	AllocBitmapDataEx(bmpId, &pColor, &pBits, &pMask, &pAlph);
	GetBitmapDataEx(bmpId, &bytePerRow, &pixelDepth, &width, &height, pColor, pBits, pMask, pAlph);
	
	for (int n = 0; n < gCntBtn; n ++)
	{
		int Id2;
		NewBitmapEx(bytePerRow, pixelDepth, 60, 40, pColor, pBits+n*240, pMask, pAlph, &Id2);

		SetCtrlAttribute(panel, gBtn[n], ATTR_WIDTH, 60);
		SetCtrlAttribute(panel, gBtn[n], ATTR_HEIGHT, 40);
	
		SetCtrlBitmap(panel, gBtn[n], 0, Id2);
		DiscardBitmap(Id2);
	}
    DiscardBitmap (bmpId);
    free (pColor);
    free (pBits);
    free (pMask);
	free(pAlph);
}

static void SetBackBtn(int panel, int level)
{
	char szName[256];
	char szLabel[256];
	char sz[][16]={"xxx", "一级", "二级", "三级", "四级", "五级", "六级"};
	GetBtnName(gParentId, szName);
	
	sprintf(szLabel, "当前层级是。。。。%s,   上一级：%s", sz[level], szName);
	SetCtrlAttribute(panel, SUB_PANEL_BACK_PICBTN, ATTR_LABEL_TEXT, szLabel);
	//SetCtrlAttribute(panel, SUB_PANEL_BACK_PICBTN, ATTR_TOOLTIP_TEXT, gParentId);
}
static int CreateBtn(const char* pParentId)
{
	if (pParentId == NULL || strlen(pParentId) < 1)
		return 0;
	
	int nCnt = CountofBtnData(pParentId);
	if (nCnt == 0)
		return 0 ;
	
	int panel = GetChildPanel(SUB_PANEL);
	
	for (int n = 0; n < gCntBtn; n ++)
	{
		DiscardCtrl(panel, gBtn[n]);
	}
	if (gBtn)
		free(gBtn);
	
	gCntBtn = nCnt;
	strcpy(gParentId, pParentId);
	gBtn = malloc(sizeof(int)*gCntBtn);
	int level = CreateBtnData(panel, pParentId, gBtn);
	
	for ( n = 0; n < gCntBtn; n ++)
	{
		InstallCtrlCallback(panel, gBtn[n], PicBtnCB, &n);
	}

	DrawBtn(panel);
	SetBackBtn(panel, level);
	return nCnt;
}

void InitSubpanel()
{
	int CNT = InitData(0);	  //初始化数据
	
	CreateBtn("1");
}
void UninitSubpanel()
{
	if (gBtn)
	{
		free(gBtn);
		gCntBtn = 0;
	}
	InitData(1);		//卸载数据
}


static int BtnIndex(int control)
{
	for (int n = 0; n < gCntBtn; n ++)
	{
		if (control == gBtn[n])
			return n;
	}
	return -1;
}

static int MouseIsOverCtrl (int panel, int control)
{
    int height, width;
    
	int x, y;
   	GetRelativeMouseState (panel, control, &x, &y, NULL, NULL, NULL);
	if ((x < 0) || (y < 0))
        return FALSE;
        
   if (control == 0)   //panel
	{
        GetPanelAttribute (panel, ATTR_HEIGHT, &height);
        GetPanelAttribute (panel, ATTR_WIDTH, &width);
	}
    else	 //ctrl
	{
        GetCtrlAttribute (panel, control, ATTR_HEIGHT, &height);
        GetCtrlAttribute (panel, control, ATTR_WIDTH, &width);
	}

    if ((y < height) && (x < width))
        return TRUE;
    else
        return FALSE;
}
static int MouseIsOver(int panel, int iscontrol)
{
	if (iscontrol == 0)	//it's panel
		return MouseIsOverCtrl(panel, 0);
	
	//continue do that it's control.
	for (int n = 0; n < gCntBtn; n ++)
	{
		int In = MouseIsOverCtrl(panel, gBtn[n]);
		if (In)
			return TRUE;
	}
	return FALSE;
}

int CVICALLBACK PictureCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int n = 0;
	return 0;
}

int CVICALLBACK PicBtnCB(int panel, int ctrl, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_COMMIT)  
	{
		int index = BtnIndex(ctrl);
		if (index < 0)
			return 0;
		index %= 4;

		char szId[256];
		GetCtrlAttribute(panel, ctrl, ATTR_TOOLTIP_TEXT, szId);
		if (CreateBtn(szId) > 0)
		{
			int nJpg = 0;
			char szJpg[128];
			sprintf(szJpg, "res\\b%02d.jpg", index+1);		//name of bmp
			GetBitmapFromFile(szJpg, &nJpg);
			
			CanvasDrawBitmap (panel, SUB_PANEL_CANVAS, nJpg, VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT);
			DiscardBitmap (nJpg);
		}
	}
	else if (event == EVENT_VAL_CHANGED)
	{
	}
	else if (event == EVENT_GOT_FOCUS)
	{
		//printf("got focus\n");
	}
	else if (event == EVENT_LOST_FOCUS)
	{
		//printf("lost focus\n");
	}
	else if (event ==EVENT_MOUSE_POINTER_MOVE)
	{
			int x, y;
		   	GetRelativeMouseState (panel, gBtn[0], &x, &y, NULL, NULL, NULL);
			
			//printf("MOUSE MOVE:%d,%d\n", x, y);
			SetMouseCursor (VAL_POINTING_FINGER_CURSOR);
	}
	else if (event == EVENT_LEFT_DOUBLE_CLICK)
	{
	}
	else if (event == EVENT_LEFT_CLICK )
	{
	}
	else if (event == EVENT_LEFT_CLICK_UP)
	{
	}
	return 0;
}

int CVICALLBACK BackLevelCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if (strcmp(gParentId, "1") == 0)
			{
				return 1;
			}
			break;
		case EVENT_LEFT_CLICK:
			{
				char pPID[256];
				if (GetBtnPID(gParentId, pPID) > 0)
				{
					if (CreateBtn(pPID) > 0)
					{
						srand(time(NULL));
						int nJpg = rand() % 4;
						char szJpg[128];
						sprintf(szJpg, "res\\b%02d.jpg", nJpg+1);		//name of bmp
						GetBitmapFromFile(szJpg, &nJpg);

						CanvasDrawBitmap (panel, SUB_PANEL_CANVAS, nJpg, VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT);
						DiscardBitmap (nJpg);
					}
				}
			}
			break;
		case EVENT_LEFT_DOUBLE_CLICK:
			break;
	}
	return 0;
}

int CVICALLBACK CanvasCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:
			break;
		case EVENT_LOST_FOCUS:
 			break;
		case EVENT_CLOSE:
			break;
       case EVENT_LEFT_CLICK:
		   break;
	   case EVENT_MOUSE_POINTER_MOVE:
		{
			//int x, y;
		   	//GetRelativeMouseState (panel, gBtn[0], &x, &y, NULL, NULL, NULL);
			SetMouseCursor (VAL_DEFAULT_CURSOR);
			//printf("mouse move:%d,%d\n", x, y);
			
                if (MouseIsOver (panel, 1))
                {
					//SetMouseCursor (VAL_CROSS_HAIR_CURSOR);
				}
                else            
                {
					//SetMouseCursor (VAL_POINTING_FINGER_CURSOR);
				}
		}
		break;
	}
	return 0;
}
