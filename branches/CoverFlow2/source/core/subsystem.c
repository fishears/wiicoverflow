#include <stdio.h>
#include <ogcsys.h>

#include "fat.h"
#include "wiipad.h"
#include "defines.h"

void Subsystem_Init(void)
{

	/* Initialize Wiimote subsystem */
	//Wpad_Init();
	
	/* Mount SDHC */
	Fat_MountSDHC();
	Fat_MountUSB();

}

void Subsystem_Close(void)
{

	/* Disconnect Wiimotes */
	Wpad_Disconnect();
	
	/* Unmount SDHC */
	Fat_UnmountSDHC();
	Fat_UnmountUSB();
}
