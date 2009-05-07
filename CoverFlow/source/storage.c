#include "storage.h"


extern s_self self;

bool init_usbfs()
{    
   // __Disc_SetLowMem();
	s32 ret;

	/* Initialize system */
	Sys_Init();

	///* Initialize subsystems */
	Wpad_Init();

	/* Mount SDHC */
	Fat_MountSDHC();
	
	self.progress+=0.05;
	Paint_Progress(self.progress, NULL);
	
	/* Initialize DIP module */
	ret = Disc_Init();
	self.progress+=0.05;
	Paint_Progress(self.progress, NULL);
	
	if (ret < 0) {
		printf("[+] ERROR:\n");
		printf("    Could not initialize DIP module! (ret = %d)\n", ret);

		return false;
	}

	return true;
}
