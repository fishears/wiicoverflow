#include "bootMenu.h"
#include "capp_dol.h"

#include "processor.h"
#include "asm.h"

extern s_self self;
extern s_settings settings;
extern s_gameSettings gameSetting;

bool Menu_Boot(){

	WPAD_Rumble(0,0);
	self.rumbleAmt = 0;
	
	#ifndef TEST_MODE
	struct discHdr *header = NULL;
	//int i = 0;

	/* No game list */
	if (!self.gameCnt)
		return false;

	/* Selected game */
	header = &self.gameList[self.gameSelected];
	
	char titleID[7];
	sprintf(titleID, "%s", header->id);
	setGameSettings(titleID,  &gameSetting, 1); // so we store last played setting ;)
		
	BUFFER_KillBuffer();
	Sleep(300);
	
	freeResources();
	
	#ifndef DOL_TEST
	/* Set WBFS mode */
	Disc_SetWBFS(WBFS_DEVICE_USB,header->id);
		
	s32 ret;
	
	/* Open disc */
	ret = Disc_Open();
	if (ret < 0) {
		return false;
	}

	ret = Disc_WiiBoot();
    if (ret < 0) {
        SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
    }

	#else
	/*
	//TODO No really sure how args need to be set up...
	char* buffer;
	buffer = malloc(strlen("bootloader.dol") + 1 + 6 + 2);
	sprintf(buffer, "bootloader.dol%c%s%c%c", '\0', header->id, '\0','\0');
	
	struct __argv argv;
	bzero(&argv, sizeof(argv));
	argv.argvMagic = ARGV_MAGIC;
	
	//length is bootloader length + null + header length + null + null
	argv.length = strlen(buffer);
	argv.commandLine = malloc(argv.length);
	strcpy(argv.commandLine, buffer);
	
	argv.argc = 2;
	argv.argv = & argv.commandLine;
	
	argv.endARGV = argv.argv + argv.length;
			
	run_dol(bootloader_dol, &argv);
	*/
	
	char cmdLine[256];
	
	
	sprintf(cmdLine, "%s %s %d %d %d %d %d", "bootloader.dol",
			header->id, settings.ocarina, settings.hooktype, settings.language,
			settings.video, settings.vipatch);
	
	CAPPDOL_Launch(bootloader_dol, (const char *)cmdLine);	

	#endif

	#endif
	
	return true;
}

bool LaunchGame()
{
	bool done = false;
	while(!done)
	{
		//self.dummy = false;
		draw_covers();
		done = draw_selected_two(true, false);
		
		GRRLIB_Render();
		//self.dummy = false;
	}
	
	/*Fade to black*/
	//TODO Fade to black instead of just drawing black
	
	int i = 0;
	int fade = 0x00;
	
	for(i = 0; i <= 25; i++){
		fade+=10;
		
		draw_covers();
		draw_selected_two(true, false);
		GRRLIB_2D_Init();
		GRRLIB_FillScreen(0x00000000|fade);
		GRRLIB_Render();
	}
	

	if(!Menu_Boot())
	{
		self.selected = false;
		self.animate_flip = 0;
	}
	else
	{
		return 0;
	}
	
	return false;
}
