#include "bootloader.h"

s_bootloader settings;

// Language selection config
char languages[11][22] =
{{"Console Default"},
{"   Japanese"},
{"    English"},
{"    German"},
{"    French"},
{"    Spanish"},
{"    Italian"},
{"     Dutch"},
{"   S. Chinese"},
{"   T. Chinese"},
{"    Korean"}};
//video mode text
char vidmodes[6][10] =
{{ " game " },
	{ " auto ", },
	{ " pal50", },
	{ " pal60", },
	{ " ntsc ", },
        { "system"}};

/* WBFS device */
static s32 my_wbfsDev = WBFS_DEVICE_USB;

bool init_usbfs()
{    
   // __Disc_SetLowMem();

	s32 ret;

	
	/* Initialize system */
	Sys_Init();

	/* Mount SDHC */
	//Fat_MountSDHC();
	
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

	//Default Settings
	settings.video    = 0;
    settings.ocarina  = 0;
    settings.hooktype = 0;
    settings.language = 0;
    settings.video    = 0;
    settings.vipatch  = 0;
	strcpy(settings.localLanguage, "EN-US"); //Localization

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
	
	my_wbfsDev = WBFS_DEVICE_USB;
	
	
	ret = WBFS_Init(my_wbfsDev);
		
	if(argc > 0)
	{
		/*arg 1 == Game ID*/
		
		/* Set WBFS mode */
		Disc_SetWBFS(WBFS_DEVICE_USB,argv[1]);
	}
	
	/*TODO Use other args to set up CFG for patching, etc...*/
	
	/* Open disc */
	ret = Disc_Open();
	if (ret < 0) {
		return false;
	}

	ret = Disc_WiiBoot();
    if (ret < 0) {
        SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
    }
	
	
	SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	
	return 0;
} 
