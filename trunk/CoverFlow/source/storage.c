#include "storage.h"

extern s_self self;
extern s_settings settings;

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
		printf(localStr("M086", "[+] ERROR:\n"));
		printf(localStr("M109", "    Could not initialize DIP module! (ret = %d)\n"), ret);

		return false;
	}

	return true;
}

bool reinit_usbfs()
{
	u32 ret;
	
	/* UnMount SDHC */
	Fat_UnmountSDHC();
	
	sleep(1);
	
	/* Mount SDHC */
	Fat_MountSDHC();
				
	/* Initialize DIP module */
	ret = Disc_Init();
	
	if (ret < 0) {
		printf(localStr("M086", "[+] ERROR:\n"));
		printf(localStr("M109", "    Could not initialize DIP module! (ret = %d)\n"), ret);

		return false;
	}

	return true;
}

void saveFile(char* imgPath, struct block file){
			
	/* save png to sd card for future use*/
			
	FILE *f;
	f = fopen(imgPath, "wb");
	if (f)
	{
		fwrite(file.data,1,file.size,f);
		fclose (f);
	}
}

void checkDirs(){
	
	int result = 0;
	
	DIR_ITER* dir = diropen(USBLOADER_PATH);
	if (dir == NULL) {
		
		mkdir(USBLOADER_PATH, S_ISVTX);
		//int result = chdir("SD:/usb-loader/");
		result = chdir(USBLOADER_PATH);
		
		if(result == 0){
			//WindowPrompt("Cover download","result = 0", &okButton, NULL);
			mkdir("disks", S_ISVTX);
			//WindowPrompt("Cover download","result = 0", &okButton, NULL);
			mkdir("covers", S_ISVTX);
			mkdir("games", S_ISVTX);
		}
		else{
			WindowPrompt(localStr("M003", "ERROR!"), localStr("M110", "Can't read directories. Covers will not be saved."), &okButton, NULL);
		}
	}
	else{
	
	//	WindowPrompt("Cover download","Closing dir", &okButton, NULL);
		dirclose(dir);
		
		result = chdir(USBLOADER_PATH);
		
		if(result == 0){
			dir = diropen("disks");
	//		WindowPrompt("Cover download",USBLOADER_PATH "/disks/", &okButton, NULL);
			if(dir == NULL) {
				mkdir("disks", S_ISVTX);
			}
			else{
				dirclose(dir);
			}
			
			dir = diropen("covers");
	//		WindowPrompt("Cover download",USBLOADER_PATH "/disks/", &okButton, NULL);	
			if(dir == NULL) {
				mkdir("covers", S_ISVTX);
			}
			else{
				dirclose(dir);
			}
			/*
			dir = diropen("games");
	//		WindowPrompt("Cover download",USBLOADER_PATH "/disks/", &okButton, NULL);	
			if(dir == NULL) {
				mkdir("games", S_ISVTX);
			}
			else{
				dirclose(dir);
			}
			*/
		}
		
		else{
			WindowPrompt(localStr("M003", "ERROR!"), localStr("M111", "Can't create directories. Covers will not be saved."), &okButton, NULL);
		}
	}
}

void checkFiles(){
	
	FILE* fp;
	
	fp = fopen(USBLOADER_PATH "/gamelist.xml", "r");
	
	if(fp == NULL)
		createEmptyGameSettingsFile();
	else
		fclose(fp);
}

void initWBFS(){

	int retries = 3;
	int ret;
	
	self.progress += .1;
	sprintf(self.debugMsg, localStr("M091", "Initializing WBFS") );
	Paint_Progress(self.progress,self.debugMsg);

  INIT_RETRY:
	/* Initialize WBFS */
	ret = WBFS_Init(self.my_wbfsDev);
	
	if(ret < 0)
	{
		while(1)
		{
			WPAD_ScanPads();
			if (retries==0)
			{
				GRRLIB_DrawImg(115, 95, menu_bg_texture, 0, 1, 1, 0xFFFFFFFF);
				GRRLIB_Printf(190, 140, font_texture, settings.fontColor, 1, localStr("M092", "USB Error - Drive not found"));
				GRRLIB_Printf(190, 160, font_texture, settings.fontColor, 1, localStr("M093", "Press A to Retry, B to Exit"));
				
				GRRLIB_Render();
			}
				
			if ((WPAD_ButtonsDown(0) & WPAD_BUTTON_A)||retries)
			{
				if((WPAD_ButtonsDown(0) & WPAD_BUTTON_A))
				{
					GRRLIB_DrawImg(115, 95, menu_bg_texture, 0, 1, 1, 0xFFFFFFFF);
					GRRLIB_Printf(190, 140, font_texture, settings.fontColor, 1, localStr("M094", "Attempt to connect to USB Drive"));
					GRRLIB_Printf(190, 160, font_texture, settings.fontColor, 1, localStr("M067", "Please Wait..."));
					GRRLIB_Render();
					Sleep(1000);
				}
				
				retries--;
				Subsystem_Close();
				WDVD_Close();
				ret = IOS_ReloadIOS(249);
				Subsystem_Init();
				WDVD_Init();
				goto INIT_RETRY;
			}
			
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
			{
				GRRLIB_Exit();
				SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
			}
		}
	}
}

s32 __Menu_EntryCmp(const void *a, const void *b)
{
	struct discHdr *hdr1 = (struct discHdr *)a;
	struct discHdr *hdr2 = (struct discHdr *)b;

	/* Compare strings */
	return strcmp(hdr1->title, hdr2->title);
}
