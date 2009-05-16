#ifndef _HomeMenu_H_
#define _HomeMenu_H_

//#define BATTMAN

#include "coverflow.h"
#include <wiiuse/wiiuse.h>

void HomeMenu_Init();

void HomeMenu_Show();

void HomeMenu_Destroy();
#ifdef BATTMAN
void Battery_Info();
#endif
void handle_ctrl_status(struct wiimote_t* wm);

#endif //_HomeMenu_H_
