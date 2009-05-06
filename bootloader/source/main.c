#include "coverflow.h"

//static char prozent[MAX_CHARACTERS + 16];
static char timet[MAX_CHARACTERS + 16];

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

/* Gamelist buffer */
static struct discHdr *gameList = NULL;

static wbfs_t *hdd = NULL;

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
		
	/* Set WBFS mode */
	Disc_SetWBFS(WBFS_DEVICE_USB,"RFLE69");
		
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