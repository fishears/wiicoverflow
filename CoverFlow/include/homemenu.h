#ifndef _HomeMenu_H_
#define _HomeMenu_H_

//#define BATTMAN

#include "coverflow.h"
#include <wiiuse/wiiuse.h>

typedef struct {
        s32 chan; //!< Trigger controller channel (0-3, -1 for all)
        WPADData wpad; //!< Wii controller trigger data
} s_wiimote;

extern s_wiimote user_remote;

void HomeMenu_Init();

void HomeMenu_Show();

void HomeMenu_Destroy();

void Battery_Info();

#endif //_HomeMenu_H_
