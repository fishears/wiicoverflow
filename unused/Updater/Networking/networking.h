/*
 *  networking.h
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by:  scognito, F1SHE4RS, afour98, blackbird399, LoudBob11
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 *	Written by dhewg/bushing modified by dimok and LoudBob11
 */
 
#ifndef _NETWORKING_H_
#define _NETWORKING_H_

#include "coverflow.h"

#define NETWORKBLOCKSIZE       5*1024      //5KB

void Initialize_Network(void);
bool IsNetworkInit(void);
char * GetNetworkIP(void);
char * GetIncommingIP(void);

bool ShutdownWC24();
s32 network_request(const char * request);
s32 network_read(u8 *buf, u32 len);
s32 download_request(const char * url);
void CloseConnection();
int CheckUpdate();
int checkForNews();

void HaltNetworkThread();
void ResumeNetworkWait();
void ResumeNetworkThread();
void InitNetworkThread();
void ShutdownNetworkThread();
#endif
