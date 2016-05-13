//==============================================================================
//
// Title:		NetComm.c
// Purpose:		A short description of the implementation.
//
// Created on:	2015/12/18 at 13:58:47 by cwj.
// Copyright:	hy. All Rights Reserved.
//
//==============================================================================

#include <ansi_c.h>
#include <tcpsupp.h>
#include <userint.h>
#include <utility.h>

#include "NetComm.h"

#define CNT_THREAD 		10
const int	TCP_SRV_PORT = 10000;

extern void InitAnno_setconetent(const char* pContent);
extern void PlotGraph_external(int indexgraph, float* pdata, int len);

static int		gPanel = 0;

struct tagThdData
{
	int 	panel;
	int 	thdId;		/*tread   id*/
	unsigned int tcpId;	/*connect handle*/
	char 	isQuit;
	char 	isActive;
	short 	reserve;
};

static struct tagThdData 	gThreadData[CNT_THREAD];
static CmtThreadPoolHandle 	gThreadPool;
static CmtThreadLockHandle 	gThreadLock;

static int 			gnThreads = 0;
static double 		gLoopTime = 0.1;	/*second*/
//static unsigned int	gnTcpHandle;


static int CVICALLBACK thdTcpFunc(void* data);
static int CVICALLBACK TcpCallback(unsigned int handle, int nType, int err, void* callbackData);

static int FindAvailable() {
	for (int i = 0; i < CNT_THREAD; ++i)
		if (gThreadData[i].isActive == 0)
			return i;
	return -1;
}

int InitNetcomm(int panel)
{
	if (gPanel == 0)
	{
		gPanel = panel;
	
		if (CmtNewLock("", 0, &gThreadLock) < 0)  		// a lock to protect the gThreadData array and ..      
			return 0;
 
		if (CmtNewThreadPool(CNT_THREAD, &gThreadPool) < 0) 	// maximum of CNT_THREAD active threads
			return 0;

		for (int i = 0; i < CNT_THREAD; ++i)
		{
			gThreadData[i].thdId = 0;
			gThreadData[i].tcpId = 0;
			gThreadData[i].isQuit = 0;
			gThreadData[i].isActive = 0;
		}
	}

	return 1;
}

int UninitNetcomm()
{
	SetMouseCursor(VAL_HOUR_GLASS_CURSOR);
	
	CmtGetLock(gThreadLock);
	for (int i = 0; i < CNT_THREAD; ++i)  	
		gThreadData[i].isQuit = 1;
	CmtReleaseLock(gThreadLock);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//SetMouseCursor(VAL_HOUR_GLASS_CURSOR);

	CmtDiscardThreadPool(gThreadPool);
	CmtDiscardLock(gThreadLock);
	gPanel = 0;			//卸载通讯时，复位句柄

	SetMouseCursor(VAL_DEFAULT_CURSOR);
	
	
	return 1;
}


int Stop(int Index)
{
	if (Index < 0)
		return -1;

	if (gThreadData[Index].isActive == 1)
	{
		CmtGetLock(gThreadLock);
		gThreadData[Index].isQuit = 1;
		CmtReleaseLock(gThreadLock);

		CmtWaitForThreadPoolFunctionCompletion(gThreadPool, gThreadData[Index].thdId, 0);
		CmtReleaseThreadPoolFunctionID(gThreadPool, gThreadData[Index].thdId);

		CmtGetLock(gThreadLock);
		gThreadData[Index].isActive = 0;
		gnThreads --;
 		CmtReleaseLock(gThreadLock);
	}
	
	return 1;
}

int Startup()
{
	int ThdIndex = -1;
	if (gPanel == 0)
		return ThdIndex;
	
	CmtGetLock(gThreadLock);
	if (gnThreads < CNT_THREAD)
	{
		int slot = FindAvailable();
		if (slot > -1)
		{
			if (CmtScheduleThreadPoolFunction(gThreadPool, thdTcpFunc, &(gThreadData[slot]), &(gThreadData[slot].thdId)) >= 0)
			{
				gThreadData[slot].isQuit = 0;
				gThreadData[slot].isActive = 1;
				
				ThdIndex = gnThreads;
				gnThreads ++;
			}
		}
	}
	CmtReleaseLock(gThreadLock);

	return ThdIndex;
}

int CVICALLBACK thdTcpFunc(void* data)
{
	struct tagThdData* pData = ((struct tagThdData *) data);
	
	//TcpConnect(1);
	int ret = 0;
	CmtGetLock(gThreadLock);
	int slot = 0;
	for (int n = 0; n < CNT_THREAD; n ++)
	{
		if (gThreadData+n == pData)
			slot = n;
	}
    DisableBreakOnLibraryErrors ();
	ret = ConnectToTCPServer (&pData->tcpId, TCP_SRV_PORT+slot, NULL/*"10.10.1.4"*/, TcpCallback, 0, 0);
    EnableBreakOnLibraryErrors ();
	if (ret < 0)
		pData->isQuit = 1;
	CmtReleaseLock(gThreadLock);
	
	//...
	while (!pData->isQuit)
	{
		ProcessSystemEvents ();
	}

	//TcpConnect(0);
	if (ret == 0)
	{
		CmtGetLock(gThreadLock);
		if (DisconnectFromTCPServer(pData->tcpId) == 0)
			pData->tcpId = 0;
		CmtReleaseLock(gThreadLock);
	}
	return ret;
}

static int CVICALLBACK TcpCallback(unsigned int handle, int nType, int err, void* callbackData)
{
	float buff[8*1024];
	int buf_len = sizeof(buff)/sizeof(float);
	
	switch (nType)
	{
		case TCP_DISCONNECT:
			buff[0] = 0;
			//buff = 0x1;
			break;
		case TCP_DATAREADY:
			DisableBreakOnLibraryErrors ();
			if (1)
			{
				int readtimes = 0;
				memset(buff, 0x0, buf_len);
				
				const int consttotal = buf_len;
				int total = consttotal;
 				while (total > 0)
				{
					int read = ClientTCPRead (handle, &buff[consttotal - total], total*sizeof(float), 0);
					total -= read/sizeof(float);
					
					readtimes ++;
				}
				
				if (1)	//graph
				{
					PlotGraph_external(handle%8, buff, consttotal);
				}
				else
				{//trace
					/*20160108*/
					//char szTmp[32];
					//sprintf(szTmp, "%d_%d:%.2f..%.2f", handle, readtimes, buff[0], buff[consttotal-1]);
					
					//static char trace[1024]={0};
					//strcat(trace, szTmp);
					//if (handle%8 == 7)
					//{
					//	InitAnno_setconetent(trace);
					//	memset(trace, 0x0, sizeof(trace));
					//}
					//else
					//{
					//	strcat(trace, "|");
					//}
				}
			}
			else
			{
				while (1)
				{
					if (ClientTCPRead (handle, buff, sizeof(buff), 200) < 0)		//超时200ms
					{
						break;
					}

					char szTmp[16];
					if (handle%8 == 7)
						sprintf(szTmp, "%d_%d\n", handle, buff);
					else
						sprintf(szTmp, "%d_%d,", handle, buff);
					//InitAnno_setconetent(szTmp);
					printf(szTmp);
				}
			}
			EnableBreakOnLibraryErrors ();
			break;
	}
		
	return 0;
}

int ThdFuncDo()
{
	Delay(gLoopTime);
	return 0;
}
