//==============================================================================
//
// Title:		NetComm.h
// Purpose:		A short description of the interface.
//
// Created on:	2015/12/18 at 13:58:47 by cwj.
// Copyright:	hy. All Rights Reserved.
//
//==============================================================================

#ifndef __NetComm_H__
#define __NetComm_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include "cvidef.h"


int InitNetcomm (int panel);
int UninitNetcomm();

int Startup();		//return thread index.
int Stop(int ThdIndex);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __NetComm_H__ */
