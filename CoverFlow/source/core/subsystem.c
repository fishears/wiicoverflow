#include <stdio.h>
#include <ogcsys.h>

#include "fat.h"
#include "fatmounter.h"
#include "wiipad.h"
#include "defines.h"

void Subsystem_Init(void)
{

	/* Initialize Wiimote subsystem */
	//Wpad_Init();
	
	/* Mount SDHC */
	SDCard_Init();
	USBDevice_Init();

}

void Subsystem_Close(void)
{

	/* Disconnect Wiimotes */
	Wpad_Disconnect();
	
	/* Unmount SDHC */
	SDCard_deInit();
	USBDevice_deInit();
}
