#include <windows.h>
#include <winuser.h>
#include <ansi_c.h>
#include <utility.h>
#include <cvirte.h>		
#include <userint.h>
#include "Thd.h"

#include "TLib.h"
#include "toolbox.h"
#include <analysis.h>
#include "sg/sgProcess.h"
#include "GraphEx.h"

#include "netcomm.h"

char gszWaveFile[256]={"cfg/abinary.txt"};

#include "graphcursors.h"
static GraphCursorCtrlHandle demoGcc = 0x00010000;
																																			  
#include <stdbool.h>   //让c支持bool型

static int	dllPanel = 0;
static int	gnX = 0;
static int	gnY = 0;

static int	gnCnt = 0;
static const int Max_Cnt = 16;
static int* gpGraph = NULL;
static int* gpSplit = NULL;
static int*	gnCursorAnno = NULL;
static int* gnPlot = NULL;
static int	ReleaseBuff();
static int	MallocBuff();   //
static int	AddSplit();

static float gWaveRectT = 0;
static float gWaveRectB = 0;
static float gWaveRectL = 0;
static float gWaveRectR = 0;
static float gFftRectT = 0;
static float gFftRectB = 0;
static float gFftRectL = 0;
static float gFftRectR = 0;

int 		GetPanel() {return dllPanel;}
int FindAnnoId(int control);

extern void RestoreGraph(int panel, int control, double xMin, double yMin, double xMax, double yMax);
#define kMessage   "示例.\n注释拖拉."

#define _EXPORT_DLL_
float* ReadData(const char* lpFile, int* nCnt, float* fMin, float* fMax) ;
int  CVICALLBACK GraphCB_Dyn(int panelHandle, int controlID, int event, void *callbackData, int eventData1, int eventData2);
int ShowWaveFile_fft(int clickbtn);
int ShowWaveFile();

static void MovePanel(bool bInit)
{
	if (bInit)
	{
		GetPanelAttribute (dllPanel, ATTR_WIDTH, &gnX);
		GetPanelAttribute (dllPanel, ATTR_HEIGHT, &gnY);
		
		SetCtrlAttribute (dllPanel, PANEL_NUMERIC, ATTR_VISIBLE, 	0);
		SetCtrlAttribute (dllPanel, PANEL_NUMERIC_Y1, ATTR_VISIBLE, 	0);
		SetCtrlAttribute (dllPanel, PANEL_NUMERIC_Y2, ATTR_VISIBLE, 	0);
		SetCtrlAttribute (dllPanel, PANEL_CB_RESTORE, ATTR_VISIBLE, 	0);
	}
	else
	{
		int nX, nY;
		GetPanelAttribute (dllPanel, ATTR_WIDTH, &nX);
		GetPanelAttribute (dllPanel, ATTR_HEIGHT, &nY);
		OperateSplitter(dllPanel, PANEL_SPLITTER_V, nX - gnX);
		gnX = nX;
		
		int nBaseY = 34;		     	//124是预留上 面 的 高度
		int nGrid = (nY-nBaseY) / gnCnt;    
		for (int n = 0; n < gnCnt; n ++)
		{
			SetCtrlAttribute (dllPanel, gpGraph[n], ATTR_TOP, nBaseY + n*nGrid);
			
			SetCtrlAttribute (dllPanel, gpGraph[n], ATTR_HEIGHT, nGrid);

			SetCtrlAttribute (dllPanel, gpSplit[n], ATTR_TOP, nBaseY - 6 + (n+1)*nGrid);
		}
		gnY = nY;
	}
}
static void SetAnno(int nGID, int nIndex, int nMode)
{
	if (nMode == 0)
	{
	    //SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_OFFSET_MODE, 	VAL_TOP_LEFT_OFFSET);

		SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_CAPTION_ALWAYS_IN_VIEW, FALSE);
		SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_CAPTION_BGCOLOR, VAL_TRANSPARENT);	//
	    SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_LINE_COLOR, 		0x008800);		//VAL_TRANSPARENT
	    SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_GLYPH_COLOR, 	0x990000);		//VAL_TRANSPARENT
	    SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_CAPTION_COLOR, 	0x101010);
	}
	else if (nMode == 1)
	{
		SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_CAPTION_ALWAYS_IN_VIEW, TRUE);
	    
		SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_CAPTION_BGCOLOR, VAL_TRANSPARENT);
	    SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_LINE_COLOR, 		VAL_TRANSPARENT);	//
	    SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_GLYPH_COLOR, 	VAL_TRANSPARENT);	//
	    SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_CAPTION_COLOR, 	0x101010);
	}
	else
	{
	    //SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_OFFSET_MODE, 	VAL_TOP_LEFT_OFFSET);
		SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_CAPTION_ALWAYS_IN_VIEW, FALSE);

		SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_CAPTION_BGCOLOR, VAL_TRANSPARENT);
	    SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_LINE_COLOR, 		0x008800);		//VAL_TRANSPARENT
	    SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_GLYPH_COLOR, 	0x990000);		//VAL_TRANSPARENT
	    SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_CAPTION_COLOR, 	0xFF00FF);
		
		SetAnnotationAttribute(dllPanel, nGID, nIndex, ATTR_ANNOTATION_GLYPH_STYLE, 	VAL_EMPTY_CIRCLE);

	}
}
static void CreateGraph(int nCnt)
{
	//SetCtrlAttribute (dllPanel, PANEL_GRAPH, ATTR_GRAPH_BGCOLOR, 0xFFAAAA);
	//SetCtrlAttribute (dllPanel, PANEL_GRAPH, ATTR_PLOT_BGCOLOR, 	0x0000fa);			//网 格 内 的 背 景 色
	
	if (nCnt > 0)   //初始化 动态 graph
	{
		gnCnt = nCnt;
		
		int ret = MallocBuff();
		if (ret == 0)
			return;
		
		if (ret == 1)
		{
			int nBaseY = 34;
			int nHeight = (gnY-nBaseY)/gnCnt;
			int nSpanY = 6;

			for (int n = 0; n < Max_Cnt; n ++)
			{
			    gpGraph[n] = NewCtrl (dllPanel, CTRL_GRAPH, "", nBaseY + n*(nHeight+nSpanY), 0);
			    InstallCtrlCallback (dllPanel, gpGraph[n], GraphCB_Dyn, 0);

			    SetCtrlAttribute (dllPanel, gpGraph[n], ATTR_WIDTH, 		gnX-0);
				SetCtrlAttribute (dllPanel, gpGraph[n], ATTR_HEIGHT, 		nHeight);
			    SetCtrlAttribute (dllPanel, gpGraph[n], ATTR_GRAPH_BGCOLOR, VAL_TRANSPARENT/*VAL_BLACK*/);
				SetCtrlAttribute (dllPanel, gpGraph[n], ATTR_GRID_COLOR, 	VAL_DK_GRAY);		//网 格 线 颜 色
				SetCtrlAttribute (dllPanel, gpGraph[n], ATTR_GRID_STYLE,	VAL_DOT);
				SetCtrlAttribute (dllPanel, gpGraph[n], ATTR_PLOT_BGCOLOR, 	VAL_WHITE);			//网 格 内 的 背 景 色
				SetCtrlAttribute (dllPanel, gpGraph[n], ATTR_ENABLE_ZOOM_AND_PAN, 1);
			
				gpSplit[n] = NewCtrl (dllPanel, CTRL_HORIZONTAL_SPLITTER, "Split01", nBaseY - nSpanY + (n+1)*(nHeight+nSpanY), 0); 
		   	 	SetCtrlAttribute (dllPanel, gpSplit[n], ATTR_WIDTH, gnX);

				
				{//每一个graph都先添加一个annotation,并隐藏
					char szName[64];
					sprintf(szName, "anno:%d", n);
					gnCursorAnno[n] = AddGraphAnnotation(dllPanel, gpGraph[n], 100.0, 0.1, szName, 0, 0.10);
					SetAnno(gpGraph[n], gnCursorAnno[n], 1);
				}
			}
			AddSplit();
		}

		for (int n = 0; n < Max_Cnt; n++)
    	{
			int show = (n < nCnt) ? 1 : 0;
			int showSplit = (n < nCnt-1) ? 1 : 0;
			SetCtrlAttribute (dllPanel, gpGraph[n], ATTR_VISIBLE, 	show);
    		SetCtrlAttribute (dllPanel, gpSplit[n], ATTR_VISIBLE, 	showSplit);
		}
	}
}
static void InitAnno_Test()
{
	CreateGraph(2);
	
	if (!demoGcc)
	{
		if ((demoGcc = GCC_New (dllPanel, gpGraph[0], 0, GCC_VAL_POS_BELOW_CENTER, 2)) < 0)
			return;
				
	  	GCC_Refresh (demoGcc);
		
		GCC_SetAttribute (demoGcc, GCC_ATTR_INDEX_VISIBLE, 0);
		GCC_SetAttribute (demoGcc, GCC_ATTR_ZOOM_VISIBLE, 0);
		GCC_SetAttribute (demoGcc, GCC_ATTR_COLOR_VISIBLE, 0);
		GCC_SetAttribute (demoGcc, GCC_ATTR_NAME_VISIBLE, 0);
		GCC_SetAttribute (demoGcc, GCC_ATTR_COORDINATES_VISIBLE, 0);
		GCC_SetAttribute (demoGcc, GCC_ATTR_SELECT_VISIBLE, 0);
		GCC_SetAttribute (demoGcc, GCC_ATTR_SETTINGS_VISIBLE, 0);
		GCC_SetAttribute (demoGcc, GCC_ATTR_COMPASS_VISIBLE, 0);
		
		char name[64]={"a"};
		int ret = GCC_AddCursor(demoGcc, name);
		strcpy(name, "b");
		ret = GCC_AddCursor(demoGcc, name);
    	if (demoGcc)
			GCC_Discard (demoGcc);
	}
	else
	{
		for (int n = 0; n < gnCnt; n ++)
		{
			SetCtrlAttribute (dllPanel, gpGraph[n], ATTR_NUM_CURSORS, 1);
		
			SetCursorAttribute(dllPanel, gpGraph[n], 1, ATTR_CROSS_HAIR_STYLE, VAL_VERTICAL_LINE);
			SetCursorAttribute(dllPanel, gpGraph[n], 1, ATTR_CURSOR_MODE, VAL_SNAP_TO_POINT);
		}
	}
}
void InitAnno_setconetent(const char* pContent)
{
}

HDC GetHdc(int control)
{
	int handle1;
	GetPanelAttribute (dllPanel, ATTR_SYSTEM_WINDOW_HANDLE, &handle1);
	
    HDC hdc = GetDC((HWND)handle1);	       //获取指定窗口的设备场景 
	return hdc;
}
// 求数组的最大值和最小值，返回值在maxValue和minValue
static int CalcMaxMin(float* buff, int len, int* posMax, int* posMin)
{
	float vMax = buff[0];
	float vMin = buff[0];
	for (int i = 1; i < len; i ++)
	{
		if (vMax < buff[i])
		{
			vMax = buff[i];
			*posMax = i;
			//printf("max pData[%3d]=%.3f\n", i, vMax);
		}
		else if (buff[i] < vMin)
		{
			vMin = buff[i];
			*posMin = i;
			//printf("MIN pData[%3d]=%.3f\n", i, vMin);
		}
	}
	return 0;
}

/*static void AnnotationCalc(float* waveform, int nLines)
{
	//20160113
    int i = 0;
	int nNo = 1;
	char pTxt[256];
    while (i < nLines) 
 	{
		//区 域 小 值
        while (waveform[i]>waveform[i+1] && i < nLines-1)
            i ++;
        if (i == nLines-1)
            break;
		
        sprintf(pTxt, "小值(%d) \nX: %d, Y: %.3f", nNo, i, waveform[i]);
        nNo = AddGraphAnnotation(dllPanel, gpGraph[1], i, waveform[i], pTxt, -25, -65);
		
        //区 域 大 值.
        while (waveform[i]<waveform[i+1] && i < nLines-1)
            i ++;
        if (i == nLines-1)
            break;
        sprintf(pTxt, "大值(%d) \nX: %d, Y: %.3f", nNo+1, i, waveform[i]);
        nNo = AddGraphAnnotation(dllPanel, PANEL_GRAPH, i, waveform[i], pTxt, -25, 65); 
	}
}
*/
#ifdef _EXPORT_DLL_
int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			if (InitCVIRTE (hinstDLL, 0, 0) == 0)
				return 0;	// out of memory
			break;
		case DLL_PROCESS_DETACH:
			if (!CVIRTEHasBeenDetached())		// Do not call CVI functions if cvirte.dll has already been detached.
			{
				ReleaseBuff();
				CloseCVIRTE ();
			}
			break;
	}

	return 1;
}
#else
int main(int argc, char* argv[])
{
	if (InitCVIRTE(0, argv, 0) == 0)
		return -1;
	
	if ((dllPanel = LoadPanel(0, "Thd.uir", PANEL)) < 0)
		return -1;

	MovePanel(true);
	InitAnno_Test();

	DisplayPanel(dllPanel);
	RunUserInterface();
	ReleaseBuff();
	DiscardPanel(dllPanel);
	return 0;
}
#endif

int InitUIall(int nParentPanel)
{
	if (dllPanel > 0)
		return dllPanel;
	
	if ((dllPanel = LoadPanelEx(nParentPanel, "Thd.uir", PANEL, __CVIUserHInst)) < 0)
		return -1;
	
	SetCtrlAttribute(dllPanel, PANEL_INCBUTTON, ATTR_VISIBLE, 0);
	MovePanel(true);
	InitAnno_Test();

	DisplayPanel(dllPanel);
	//HidePanel(dllPanel);
	
	//MovePos(true);
	//RunUserInterface();
	
	//DiscardPanel(dllPanel);
	return dllPanel;
}
int ShowUIall(int bshow)
{
	if (bshow)
		DisplayPanel(dllPanel);
	else
		HidePanel(dllPanel);
	return 0;
}

int InitUIForDLL (HWND hParent)
{
	/* Call this function from the appropriate place in your code */
	/* to load and display startup panels.                        */

	if ((dllPanel = LoadPanelEx (1, "Thd.uir", PANEL, __CVIUserHInst)) < 0)
		return -1;
	
	int hWnd;  
	GetPanelAttribute (dllPanel, ATTR_SYSTEM_WINDOW_HANDLE, &hWnd); 
	SetParent((HWND)hWnd, hParent);
	DWORD dwStyle = GetWindowLong((HWND)hWnd, GWL_STYLE);
	SetWindowLong((HWND)hWnd, GWL_STYLE, dwStyle | WS_CHILD);

	DisplayPanel (dllPanel);
	//MovePos(true);

	//RunUserInterface();		//cwj add;
	//DiscardPanel(dllPanel);
	/* Uncomment this call to RunUserInterface or call it from elsewhere */
	/* in your code to begin issuing user interface events to panels     */
	/* RunUserInterface (); */
	return hWnd;
}

void DiscardUIObjectsForDLLF (void)
{
	/* Discard the panels loaded in InitUIForDLL */
	/* Call this function from the appropriate place in your code */
	/* to discard UI objects.                                     */

	if (dllPanel > 0)
	{
		DiscardPanel (dllPanel);
		dllPanel = 0;
	}
}

/*static int DeleGPlot(int dele, double x1, double y1, double x2, double y2)
{
	static int Inc = 0, Dec = 0;
	char szCaption[128];
	static int plotId = -1;
	if (dele == 0)	//add
	{
		plotId = PlotRectangle(dllPanel, PANEL_GRAPH, x1, y1, x2, y2, VAL_DK_CYAN, VAL_TRANSPARENT);
		
		sprintf(szCaption, "%3d _add: %d", Inc++, plotId);
	}
	else if (plotId > 0)
	{
		int del = DeleteGraphPlot (dllPanel, PANEL_GRAPH, plotId, VAL_IMMEDIATE_DRAW);
		
		sprintf(szCaption, "%3d _Del: %d", Dec++, del);
	}

	return 0;
}*/

int CVICALLBACK MainCallback (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:
			//MovePos(true);
			break;
		case EVENT_LOST_FOCUS:
			break;
		case EVENT_PANEL_MOVING:
			break;
		case EVENT_PANEL_SIZING:
		case EVENT_PANEL_SIZE:
			//MovePos(false);
			MovePanel(false);
			break;
		case EVENT_MOUSE_POINTER_MOVE:
			break;
		case EVENT_CLOSE:
#ifdef _EXPORT_DLL_
			if (dllPanel > 0)
			{
				DiscardPanel (dllPanel);
				dllPanel = 0;
			}
#endif			
			QuitUserInterface(0);

			break;
	}
	return 0;
}

float* ReadData(const char* lpFile, int* nCnt, float* fMin, float* fMax)
{
	float* pOutData = NULL;
	
	FILE* fp = fopen(lpFile, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);			//将文件指针移动文件结尾
		int bytes = ftell(fp);			//求出当前文件指针距离文件开始的字节数
		*nCnt = bytes/4 + (bytes%4 ? 1 : 0);
		pOutData = (float*)malloc(*nCnt*sizeof(float));
		
		{
			fseek(fp, 0, SEEK_SET);
			
			float f1 = 0.0;
			for (int n = 0; n < *nCnt; n ++)
			{
				fread(&f1, 4, 1, fp);
				pOutData[n] = f1;
				
				*fMin = min(*fMin, f1);
				*fMax = max(*fMax, f1);
			}
		
			if (*fMax > 0 && *fMax < 1.0)		//便于最大值的显示
			{
				*fMax += 0.01;
			}
			else if (*fMax > 1.0)
			{
				*fMax += 0.1;
			}
		}
		
		fclose(fp);
	}
	return pOutData;
}

float* ReadData_ext(const char* lpFile, int* nCnt, float* fMin, float* fMax)
{
	float* pOutFft = NULL;
	FILE* fp = fopen(lpFile, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);			//将文件指针移动文件结尾
		int bytes = ftell(fp);			//求出当前文件指针距离文件开始的字节数
		int lines = bytes/4 + (bytes%4 ? 1 : 0);
		double* pImag = (double*)malloc(lines*sizeof(double));  //虚部
		double* pWave = (double*)malloc(lines*sizeof(double));  //作实部数据
		pOutFft = (float*)malloc(lines*sizeof(float));
		
		{
			fseek(fp, 0, SEEK_SET);
			
			float f1 = 0.0;
			for (int n = 0; n < lines; n ++)
			{
				fread(&f1, 4, 1, fp);
				pWave[n] = f1;
				pImag[n] = 0.0;
			}
			ComputeFFT(pWave, pImag, lines);
			for (int i = 0; i < lines; i ++) {
				float fReal = pWave[i];
				float fImag = pImag[i];
				fReal  = fabs(fReal) * fabs(fReal);
				fImag  = fabs(fImag) * fabs(fImag);
				pOutFft[i] = sqrt( fReal + fImag) / (lines/2);
				
				*fMin = min(*fMin, pOutFft[i]);
				*fMax = max(*fMax, pOutFft[i]);
			}
		}
		free(pImag);
		free(pWave);
		
		float fLines = (float)lines / 2.56;		//谱线数 = 采样点数 / 2.56,  分析带宽 = 采样率 / 2.56
		*nCnt = fLines;
		
		if (*fMax > 0 && *fMax < 1.0)		//便于最大值的显示
		{
			*fMax += 0.01;
		}
		else if (*fMax > 1.0)
		{
			*fMax += 0.1;
		}
		
		fclose(fp);
	}
	return pOutFft;
}

static int PlotGraph(int controlId, float* pdata, int len, float xMax, float yMin, float yMax, int nTestTest)
{
	int plotHandle = -1;
	SetAxisScalingMode(dllPanel, controlId, VAL_LEFT_YAXIS, VAL_MANUAL, yMin, yMax);
	SetAxisScalingMode(dllPanel, controlId, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, xMax);
	//SetCtrlAttribute (dllPanel, controlId, ATTR_SCROLL_MODE, VAL_CONTINUOUS);
	
	if (nTestTest)
	{
		int block = 8192;
		for (int n = 0; n < len/block; n ++)
		{
			//PlotStripChartPoint (dllPanel, controlId, (double)pdata[n]);
			DeleteGraphPlot (dllPanel, controlId, -1, VAL_IMMEDIATE_DRAW);
			Sleep(1000);
			plotHandle = PlotY (dllPanel, controlId, pdata+block*n, block, VAL_FLOAT, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, (yMin == 0.0) ? VAL_GREEN : VAL_RED);
			//Sleep(1000);
		}
	}
	else
	{
		//清 除 以 前 Graph中 绘 制 的 波 形
		DeleteGraphPlot (dllPanel, controlId, -1, VAL_IMMEDIATE_DRAW);

		//在 Graph中 绘 制 波 形
		plotHandle = PlotY (dllPanel, controlId, pdata, len, VAL_FLOAT, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, (yMin == 0.0) ? VAL_GREEN : VAL_RED);
	}
	return plotHandle;
}
void PlotGraph_external(int indexgraph, float* pdata, int len)
{
	if (gnCnt < 8)
		return;
	PlotGraph(gpGraph[indexgraph], pdata, len, 8192, -17, 13, 1);
}

int CVICALLBACK QuitCommandBtn02 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			{
				int 	lines;
				float fMax=0.0, fMin = 0.0;
				float* 	pData = ReadData(gszWaveFile, &lines, &fMin, &fMax);		//原始数据 wave
				if (pData)
				{
					double* pdbData= malloc(lines* sizeof(double));
					for (int n = 0; n < lines; n ++)
						pdbData[n] = pData[n];

					//清除以前Graph中绘制的波形
					/*20160113*/
					int nid = gpGraph[0];//PANEL_GRAPH
					DeleteGraphPlot (dllPanel, nid, -1, VAL_IMMEDIATE_DRAW);

					SinePattern(lines, 1.0, 0, 1.0, (double*)pdbData);
					
					free(pdbData);
					free(pData);
				}
			}
			break;
	}
	return 0;
}


int CVICALLBACK BtnShowCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			ShowWaveFile();		//wave
			break;
	}
	return 0;
}

static int yOffset(float value)
{
	int nY;
	double m1, m2;
	GetAxisScalingMode(dllPanel, gpGraph[1], VAL_LEFT_YAXIS, NULL, &m1, &m2);
	GetCtrlAttribute(dllPanel, gpGraph[1], ATTR_HEIGHT, &nY);

	int nPlotPos = (value * nY) / m2;
	int ret = nY - nPlotPos;
	if (ret > 150)
		return -100;
	if (ret > 100)
		return -60;
	if (ret > 50)
		return -20;
	else
		return -5;
}
int CVICALLBACK BtnShowTxtCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			ShowWaveFile_fft(1);	//fft
			break;
	}
	
	return 0;
}

int ShowWaveFile()
{
	int lines = 0;
	float fMax=0.0, fMin = 0.0;
	
	float* pData = NULL;
	pData = ReadData(gszWaveFile, &lines, &fMin, &fMax);			//原始数据 wave
	if (pData)
	{
		if (gnCnt > 0)
		{
			//gnPlot[0] = PlotGraph(gpGraph[0], pData, lines, 8192, -17, 13, 0);
			
			gWaveRectT = fMax;
			gWaveRectB = fMin;
			gWaveRectL = 0;
			gWaveRectR = lines;
			gnPlot[0] = PlotGraph(gpGraph[0], pData, lines, lines, fMin, fMax, 0);  
		}
		free(pData);
	}
	return 1;
}
int ShowWaveFile_fft(int clickbtn)
{
	int lines = 0;
	float fMax=0.0, fMin = 0.0;

	float* pData = NULL;
	if (gnCnt < 2)
		return 0;
	
	pData = ReadData_ext(gszWaveFile, &lines, &fMin, &fMax);	   //wave -->fft
	if (pData)
	{
			gFftRectT = fMax;
			gFftRectB = fMin;
			gFftRectL = 0;
			gFftRectR = lines;
		gnPlot[1] = PlotGraph(gpGraph[1], pData, lines, lines, fMin, fMax, 0);
		
		static int nAnno2 = 0, nAnno3 = 0;
		static int nz = 1;
		if (nz %2 == 0)
		{
			if (nAnno3)
				DeleteGraphAnnotation(/*panel*/dllPanel, gpGraph[1], nAnno3);
			if (nAnno2)
				DeleteGraphAnnotation(/*panel*/dllPanel, gpGraph[1], nAnno2);
		}
		else
		{
			char szMsg[64];
			int segment = lines / 4;
			int max, min;
			CalcMaxMin(pData+nz*segment, segment, &max, &min);

			int pointMax = nz*segment + max;
			int pointMin = nz*segment + min;
			int offset = yOffset(pData[pointMax]);
			
			sprintf(szMsg, "MAX:%.3f", pData[pointMax]);
			nAnno2 = AddGraphAnnotation(dllPanel, gpGraph[1], pointMax, pData[pointMax], szMsg, 10, offset);
			SetAnno(gpGraph[1], nAnno2, 2);

			sprintf(szMsg, "Min:%.2f", pData[pointMin]);
			nAnno3 = AddGraphAnnotation(dllPanel, gpGraph[1], pointMin, pData[pointMin], szMsg, 0, -50);
			SetAnno(gpGraph[1], nAnno3, 2);
		}
		nz = !nz;

		free(pData);
	}
	return 1;
}

static int nThd[8]= {-1,-1,-1,-1,-1,-1,-1,-1};
int CVICALLBACK RestoreCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			{
				if (1)		//测试 tcp
				{
					if (gnCnt == 8)
					{
						static int nswitch = -1;
						if (nswitch == -1)
						{
							nswitch = 0;
							InitNetcomm(dllPanel);
						
							for (int n = 0; n < gnCnt; n ++)
								nThd[n] = Startup();
						}
						else
						{
							for (int n = 0; n < gnCnt; n ++)
								Stop(nThd[n]);
							UninitNetcomm();
							nswitch = -1;
						}
					}
				}
				else
				{
					if (gnCnt > 1)
						RestoreGraph(panel, gpGraph[1], 0,0,0,0);
				}
			}
			break;
	}
	return 0;
}

int CVICALLBACK GraphCB_Dyn(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	static int flagDrag = 0;
	if (event == EVENT_COMMIT)  
	{
		if (eventData2 == VAL_CURSOR)
		{
			flagDrag = 0;
			ctrl_cursor(panel, control, eventData2, eventData1, 9);
		}
		else if (eventData2 == VAL_ANNOTATION)
		{
			flagDrag = 0;
		}
		else
			flagDrag = 0;
	}
	else if (event == EVENT_VAL_CHANGED)
	{
		if (eventData2 == VAL_CURSOR)
		{
			flagDrag = 2;
			
			ctrl_cursor(panel, control, eventData2, eventData1, 2);
		}
		else if (eventData2 == VAL_ANNOTATION)
		{
			flagDrag = 3;
		}
	}
	else if (event == EVENT_GOT_FOCUS)
	{
	}
	else if (event == EVENT_LOST_FOCUS)
	{
		//GraphClickFlag(panel, control, 0);
	}
	else if (event == EVENT_LEFT_DOUBLE_CLICK)
	{
		if (control == gpGraph[0])
		{
			double x1 = 0, x2 = 8192, y1 = -17, y2 = 13;
			if (gWaveRectR > 0.0)
			{
				x1 = gWaveRectL;
				x2 = gWaveRectR;
				y1 = gWaveRectB;
				y2 = gWaveRectT;
			}
			SetAxisScalingMode(panel, control, VAL_LEFT_YAXIS, VAL_MANUAL, y1, y2);
			SetAxisScalingMode(panel, control, VAL_BOTTOM_XAXIS, VAL_MANUAL, x1, x2);
		}
		else
		{
			double x1 = 0, x2 = 3200, y1 = 0, y2 = 0.23;
			if (gWaveRectR > 0.0)
			{
				x1 = gFftRectL;
				x2 = gFftRectR;
				y1 = gFftRectB;
				y2 = gFftRectT;
			}
			SetAxisScalingMode(panel, control, VAL_LEFT_YAXIS, VAL_MANUAL, y1, y2);
			SetAxisScalingMode(panel, control, VAL_BOTTOM_XAXIS, VAL_MANUAL, x1, x2);
			//RestoreGraph(panel, control,0,0,0,0);
		
			GraphClickFlag(panel, control, 0);  			//双击 是 否 要 清 除 点 击 标 志 待 观 察 
		}
		
		int AnnoId = FindAnnoId(control);
		if (AnnoId > 0)
		{
			SetCursorAttribute(panel, control, AnnoId, ATTR_CURSOR_ENABLED, 1);
		}
	}
	else if (event == EVENT_LEFT_CLICK )
	{
		int AnnoId = FindAnnoId(control);
		if (AnnoId > 0)
		{
			int posX=11, cursorX=0;
			double x, y;

			if (GetGraphCursor (panel, control, AnnoId, &x, &y) >= 0)
				cursorX = x;
		
			if (GetGraphCoordsFromPoint(panel, control, MakePoint(eventData2, eventData1), &x, &y) > 0)
				posX = x;

			if (abs(posX-cursorX) > 10)
			{
				flagDrag = 1;
			
				SetCursorAttribute(panel, control, AnnoId, ATTR_CURSOR_ENABLED, 0);
			}
			else
			{
				SetCursorAttribute(panel, control, AnnoId, ATTR_CURSOR_ENABLED, 1);
			}
		}

		GraphCB_ex(panel, control, eventData2, eventData1, 0);
	}
	else if (event == EVENT_LEFT_CLICK_UP)
	{
		if (flagDrag == 1)
			GraphCB_ex(panel, control, eventData2, eventData1, 2);
		
	}
	else if (event == EVENT_MOUSE_POINTER_MOVE)
	{
		if (flagDrag == 1)
			GraphCB_ex(panel, control, eventData2, eventData1, 1);
	}
    else if (event == EVENT_KEYPRESS)
  	{
		int swallowEvent = 0;

		int modifyKey 	= eventData1 & VAL_MODIFIER_KEY_MASK;
        int virtualKey 	= eventData1 & VAL_VKEY_MASK;
       	int asciiKey 	= eventData1 & VAL_ASCII_KEY_MASK;

		if (asciiKey != 0 && (eventData1 & VAL_MENUKEY_MODIFIER) == 0)
   		{
            if (islower(asciiKey))		//修 改
				SetKeyPressEventKey(eventData2, 0, toupper(asciiKey), 0, 0, 0);
            if (ispunct(asciiKey))		//替 换
				SetKeyPressEventKey(eventData2, 0, ' ', 0, 0, 0);
            if (isdigit(asciiKey))		//屏 蔽
                swallowEvent = 1;
  		}
        else if (virtualKey > 0) 
       	{
            switch (virtualKey)
           	{
                case VAL_UP_ARROW_VKEY:
                case VAL_DOWN_ARROW_VKEY:
                case VAL_LEFT_ARROW_VKEY:
                case VAL_RIGHT_ARROW_VKEY:
                case VAL_HOME_VKEY:
                case VAL_END_VKEY:
                    swallowEvent = 1;
          	}
        }
  	}
	return 0;
}

int CVICALLBACK GraphCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	//static int lcFlag = 0;		//left_click flag
	//static double beginX, beginY;
	//static int pixelX1, pixelY1;//left click's position of pixel;
	if (event == EVENT_COMMIT)  
	{
	}
	else if (event == EVENT_GOT_FOCUS)
	{
		//char szCaption[128];
		//sprintf(szCaption, "focus");
	
	}
	else if (event == EVENT_LOST_FOCUS)
	{
		GraphClickFlag(panel, control, 0);
	}
	else if (event == EVENT_LEFT_DOUBLE_CLICK)
	{
		RestoreGraph(panel, control,0,0,0,0);
		
		GraphClickFlag(panel, control, 0);  			//双击 是 否 要 清 除 点 击 标 志 待 观 察          
	}
	else if (event == EVENT_LEFT_CLICK )
	{
		if (1)
			GraphCB_ex(panel, control, eventData2, eventData1, 0);
		if (0)
		{
			int 	plotL, plotT, plotX, plotY;	//左边 顶边 宽度 高度
			int 	grapL, grapT, grapX, grapY;
			GetCtrlAttribute(dllPanel, control, ATTR_PLOT_AREA_TOP, &plotT);
			GetCtrlAttribute(dllPanel, control, ATTR_PLOT_AREA_LEFT, &plotL);
			GetCtrlAttribute(dllPanel, control, ATTR_PLOT_AREA_WIDTH, &plotX);
			GetCtrlAttribute(dllPanel, control, ATTR_PLOT_AREA_HEIGHT, &plotY);
			
			GetCtrlAttribute(dllPanel, control, ATTR_TOP, &grapT);
			GetCtrlAttribute(dllPanel, control, ATTR_LEFT, &grapL);
			GetCtrlAttribute(dllPanel, control, ATTR_WIDTH, &grapX);
			GetCtrlAttribute(dllPanel, control, ATTR_HEIGHT, &grapY);
			
			char 	szCaption[128];
			int 	posX, posY, lDown, rDown, mKey;
			GetRelativeMouseState(dllPanel, control, &posX, &posY, &lDown, &rDown, &mKey);
 			sprintf(szCaption, "plot:%2d, %d, %2d, %d\ngrap:%2d, %d, %2d, %d\nget:  %d,        %d\npos:  %d,        %d", plotL, plotL+plotX, plotT, plotT+plotY, grapL, grapL+grapX, grapT, grapT+grapY, posX, posY, eventData2, eventData1);

			SetAnnotationAttribute(panel, control, 1, ATTR_ANNOTATION_CAPTION, szCaption);
			
			double x1, y1;
			int ret = GetGraphCoordsFromPoint(dllPanel, control, MakePoint(eventData2, eventData1), &x1, &y1);
			sprintf(szCaption, "left:%d,%d\n top:%d,%d,   %d", grapL, plotL, grapT, plotT, ret); 
			SetAnnotationAttribute(panel, control, 2, ATTR_ANNOTATION_CAPTION, szCaption);
		}
	}
	else if (event == EVENT_LEFT_CLICK_UP)
	{
		GraphCB_ex(panel, control, eventData2, eventData1, 2);
	}
	else if (event == EVENT_MOUSE_POINTER_MOVE)
	{
		GraphCB_ex(panel, control, eventData2, eventData1, 1);
	}
    else if (event == EVENT_KEYPRESS)
  	{
		//if (eventData1 == ('A' | VAL_MENUKEY_MODIFIER))
		//	printf("dadadadadada,ok!!!!!!!!!!!!!!!!!!\n");
		int swallowEvent = 0;
        /* eventData1 contains the keypress value represented as a 4-byte    */
        /* integer consisting of 3 fields, 0x00MMVVAA:                       */
        /*    MM = the modifier key                                          */
        /*    VV = the virtual key, masks defined in userint.h               */
        /*    AA = the ASCII key                                             */
        /* eventData2 is a pointer to an integer which holds the actual key  */
        /* value.  This pointer can be used to change the value of the key   */
        /* before the control processes it.                                  */
		int modifyKey 	= eventData1 & VAL_MODIFIER_KEY_MASK;
        int virtualKey 	= eventData1 & VAL_VKEY_MASK;
       	int asciiKey 	= eventData1 & VAL_ASCII_KEY_MASK;
		//printf("key:%X, %X, %X(modify:%x, virtual:%x, ascii:%X)\n", callbackData, eventData1, eventData2, modifyKey, virtualKey, asciiKey);
       
        /* If the key was an ascii key and the menukey (ctrl on the PC) was  */
        /* not pressed... */
        if (asciiKey != 0 && (eventData1 & VAL_MENUKEY_MODIFIER) == 0)
   		{
            if (islower(asciiKey))		//修 改
				SetKeyPressEventKey(eventData2, 0, toupper(asciiKey), 0, 0, 0);
            if (ispunct(asciiKey))		//替 换
				SetKeyPressEventKey(eventData2, 0, ' ', 0, 0, 0);
            if (isdigit(asciiKey))		//屏 蔽
                swallowEvent = 1;
  		}
        else if (virtualKey > 0) 
       	{
            /* Check for and swallow cursor key events */
            switch (virtualKey)
           	{
                case VAL_UP_ARROW_VKEY:
                case VAL_DOWN_ARROW_VKEY:
                case VAL_LEFT_ARROW_VKEY:
                case VAL_RIGHT_ARROW_VKEY:
                case VAL_HOME_VKEY:
                case VAL_END_VKEY:
                    swallowEvent = 1;
          	}
        }
		//return swallowEvent;
  	}
	return 0;
}

int CVICALLBACK MoveBtnCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if (control == PANEL_DECBUTTON)
            {
				if (1)
				{
					char	imageFileName[MAX_PATHNAME_LEN];
					GetProjectDir (imageFileName);
					strcat (imageFileName, "\\WordRpt.bmp");

					SaveCtrlDisplayToFile(panel, gpGraph[0], 1, -1, -1, imageFileName);
				}
				static int nflag = 0;
				if (nflag == 0)
					CreateGraph(8);
				else
					CreateGraph(2);
				nflag = !nflag;
				MovePanel(false);
			}
			else if (control == PANEL_INCBUTTON)
			{
#ifdef _EXPORT_DLL_
#else
				ReleaseBuff();
				QuitUserInterface(0);
#endif
			}
			break;
	}
	return 0;
}
int MallocBuff()
{
	if (gpGraph)
		return 2;

	int nCnt = Max_Cnt;
	gpGraph = malloc(sizeof(int)*nCnt);
	gpSplit = malloc(sizeof(int)*nCnt);
	gnCursorAnno = malloc(sizeof(int)*nCnt);
	gnPlot = malloc(sizeof(int)*nCnt);

	return 1;
}
int ReleaseBuff()
{
	if (gpGraph)
	{
		free(gpGraph);
		gpGraph = NULL;
	}
	if (gpSplit)
	{
		free(gpSplit);
		gpSplit = NULL;
	}
	if (gnCursorAnno)
	{
		free(gnCursorAnno);
		gnCursorAnno = NULL;
	}
	if (gnPlot)
	{
		free(gnPlot);
		gnPlot = NULL;
	}
	gnCnt = 0;
	
	
	return 1;
}
int AddSplit()
{
	int tmp = Max_Cnt;	//gnCnt;
	for (int n = 0; n < tmp-1; n ++)
	{
		//int nTopID = (n == 0) ? PANEL_GRAPH : gpGraph[n-1];
		AddCtrlToSplitter(dllPanel, gpSplit[n], VAL_TOP_ANCHOR, gpGraph[n], 0, 0);
		AddCtrlToSplitter(dllPanel, gpSplit[n], VAL_BOTTOM_ANCHOR, gpGraph[n+1], 0, 0);
	}
	
	AddCtrlToSplitter(dllPanel, PANEL_SPLITTER_H, VAL_TOP_ANCHOR, gpGraph[tmp-1], 0, 0);
	/*AddCtrlToSplitter(dllPanel, PANEL_SPLITTER_V, VAL_LEFT_ANCHOR, PANEL_GRAPH, 0, 0);*/
	for (int n = 0; n < tmp; n ++)
	{
		AddCtrlToSplitter(dllPanel, PANEL_SPLITTER_V, VAL_LEFT_ANCHOR, gpGraph[n], 0, 0);
		
		AddCtrlToSplitter(dllPanel, PANEL_SPLITTER_V, VAL_LEFT_ANCHOR, gpSplit[n], 0, 0);
	}
	return 1;
}

int FindAnnoId(int control)
{
	//20160115  目前只处理第2个graph的cursor，其他graph一律不处理。
	if (control != gpGraph[0] && control != gpGraph[1])
		return -1;
	
	
	for (int n = 0; n < Max_Cnt; n ++)
	{
		if (gpGraph[n] == control)
		{
			return gnCursorAnno[n];
		}
	}
	return -1;
}
int PutWave(const char* pWaveFile)
{
	if (pWaveFile && strlen(pWaveFile) > 5)
	{
		strcpy(gszWaveFile, pWaveFile);
		
		ShowWaveFile();		//wave
		ShowWaveFile_fft(0);	//fft
	}
	return 1;
}
