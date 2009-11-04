/*
 *  OSK.h
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef _OSK_H
#define	_OSK_H

#include "defines.h"
#include <wiikeyboard/keyboard.h>

#ifdef	__cplusplus
extern "C" {
#endif


typedef struct _keytype {
	char ch, chShift, chalt, chalt2;
} Key;

int  showOSK(char *kbtitle);
void setCharSet();

#ifdef	__cplusplus
}
#endif

#endif	/* _OSK_H */

