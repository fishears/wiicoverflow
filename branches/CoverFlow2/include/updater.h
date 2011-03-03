/*
 *  updater.h
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11, alexcarlosantao
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 *  This file contains the main() entry point to the application
 *  Build configuration settings can be found in "coverflow.h"
 */

#ifndef _UPDATER_H
#define	_UPDATER_H

#include "coverflow.h"

bool ShutdownWC24();
void* networkinitcallback(void *arg);
void initNetworkThread();
void shutdownNetworkThread();
bool checkForUpdate();
bool checkForNews();
bool promptForUpdate();
bool downloadUpdate();

s32 network_request(const char * request);
s32 network_read(u8 *buf, u32 len);
s32 download_request(const char * url);

bool existFile(char * path);
void CloseConnection();

bool getNewsFile();

#endif
