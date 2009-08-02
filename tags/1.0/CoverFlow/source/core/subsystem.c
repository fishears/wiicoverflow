#include <stdio.h>
#include <ogcsys.h>

#include "fat.h"
#include "wiipad.h"
#include "fatmounter.h"


void Subsystem_Init(void)
{

	/* Initialize Wiimote subsystem */
	//Wpad_Init();
	
	/* Mount SDHC */
	SDCard_Init();
}

void Subsystem_Close(void)
{

	/* Disconnect Wiimotes */
	Wpad_Disconnect();
	
	/* Unmount SDHC */
    SDCard_deInit();
}
