#include "bootloader.h"
#include "wpadsvn.h"
#include "subsystem.h"
#include "errno.h"

s_settings settings;
s_gameSettings gameSetting;

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

bool init_usbfs()
{    

	s32 ret;

	
	/* Initialize system */
	Sys_Init();

	/* Initialize DIP module */
	ret = Disc_Init();
	
	if (ret < 0) {
		printf("[+] ERROR:\n");
		printf("    Could not initialize DIP module! (ret = %d)\n", ret);

		return false;
	}

	return true;
}

void initVars(){

}

//---------------------------------------------------------------------------------
int main( int argc, char **argv ){
//---------------------------------------------------------------------------------

	VIDEO_Init();
	
	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);
	
	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);
	
	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

/*
	int i = 0;
	for(i=0; i<5; i++){
			usleep(1000);
			VIDEO_WaitVSync();
	}
*/
	
	/* Load Custom IOS */
	int ret = IOS_ReloadIOS(249);
	/* Check if Custom IOS is loaded */
	if (ret < 0) {
		//printf("[+] ERROR:\n");
		//printf("    Custom IOS could not be loaded! (ret = %d)\n", ret);

		return 0;
	}	
	
	if(!init_usbfs()){
		return 0;
	}
	
	Sys_Init();
	Subsystem_Init();
		
		char gameID[7];
                SETTINGS_Load();
                strcpy(gameID, settings.lastplayed);
                if(getGameSettings(gameID, &gameSetting))
					apply_settings();
	
	/* Set WBFS mode */
	Disc_SetWBFS(WBFS_DEVICE_USB,(u8*)gameID);

	s32 retval;

	/* Open disc */
	retval = Disc_Open();
	if (retval < 0) {
		return false;
	}

	retval = Disc_WiiBoot();
    if (retval < 0) {
        SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
    }
	
	
	SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	
	return 0;
} 

