#include "bootloader.h"
#include "wpadsvn.h"
#include "subsystem.h"
#include "errno.h"

s_settings settings;

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

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

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


	
	
	VIDEO_Init();
	
	// This function initialises the attached controllers
	//WPAD_Init();
	
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


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[2;0H");
	

	printf("ITALY ROXXXX!\n");
	/*
	int j=0;
	printf("argc: %d\n", argc);
	for(;j<argc;j++)
	{
		printf("ARG[%d] = %s\n",j,argv[j]);
	}
	*/
	
	int i = 0;
	for(i=0; i<5; i++){
			usleep(1000);
			VIDEO_WaitVSync();
	}
	
	//Default Settings
	settings.video    = 0;
    settings.ocarina  = 0;
    settings.hooktype = 0;
    settings.language = 0;
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
	
		Sys_Init();
	Subsystem_Init();
	//initWBFS();
	
	my_wbfsDev = WBFS_DEVICE_USB;
	
	
	ret = WBFS_Init(my_wbfsDev);
		
	if(argc > 0)
	{
		/*arg 1 == Game ID*/
		
		/* Set WBFS mode */
		Disc_SetWBFS(WBFS_DEVICE_USB,argv[1]);
	}
	else{
	
		  //TODO:
		  // Implement system settings load and per settings game, this is just a proof of concept
		  // on how boot games without the need of parameters (since we suck and cannot get it work)
		  // In my opinion this way is also good because it uses standard call for getting parameters.
		  // If an option is added in settings we don't need to add to the parameter list, just use
		  // the normal calls for getting parameter.
		  // Do not forget to call system settings if per game settings are not found!
		
		  char gameID[7];
	
		  FILE *fp;
		  mxml_node_t *xml;

		  fp = fopen(USBLOADER_PATH "/wiicoverflow.xml", "r");
		  
		  if(fp == NULL){
			 
			printf("wait for succablank %d\n", errno);
			printf(USBLOADER_PATH "/wiicoverflow.xml");
			VIDEO_WaitVSync();
			usleep(10000);
			 
			 return -1;
			}
		  
		  xml = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
		 
		 
		  fclose(fp);
				
		  if(xml != NULL)
		  {
		  
				printf("xml != null\n");
				VIDEO_WaitVSync();
				usleep(5000);

		  
			  mxml_node_t *node;
			  mxml_node_t *next_n;
				
			  node = mxmlFindElement(xml,xml, "wiicoverflow", NULL, NULL, MXML_DESCEND); 
			  if(node == NULL) return -1;
			  
			  next_n = mxmlFindElement(node, node, "graphics", NULL, NULL, MXML_DESCEND); 
			  next_n = mxmlFindElement(node, node, "general", NULL, NULL, MXML_DESCEND);
			  next_n = mxmlFindElement(node, node, "game", NULL, NULL, MXML_DESCEND);
			  
			    if(next_n != NULL)
				  {
					if(mxmlElementGetAttr(next_n,"lastplayed")){
						  strcpy(gameID, mxmlElementGetAttr(next_n,"lastplayed"));
						  Disc_SetWBFS(WBFS_DEVICE_USB, gameID);
						  }
				  }
		 }
		
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

