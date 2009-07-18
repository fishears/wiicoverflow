#include <stdio.h>
#include <ogcsys.h>

#include "fat.h"
#include "fatmounter.h"
#include "wiipad.h"


void Subsystem_Init(void)
{

	/* Initialize Wiimote subsystem */
	Wpad_Init();
	
	/* Mount SDHC */
	//Fat_MountSDHC();
	SDCard_Init();
	USBDevice_Init();
}

void Subsystem_Close(void)
{

	/* Disconnect Wiimotes */
	Wpad_Disconnect();
	
	/* Unmount SDHC */
	//Fat_UnmountSDHC();
	USBDevice_deInit();
	SDCard_deInit();
}
