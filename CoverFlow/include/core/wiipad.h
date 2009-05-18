#ifndef _WIIPAD_H_
#define _WIIPAD_H_

#include <wpad.h>

/* Prototypes */
s32  Wpad_Init(void);
void Wpad_Disconnect(void);
u32  Wpad_GetButtons(void);
u32  Wpad_WaitButtons(void);

#endif
