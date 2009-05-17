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

s32 GetEntries()
{
	struct discHdr *buffer = NULL;

	u32 cnt, len;
	s32 ret;

	/* Get list length */
	ret = WBFS_GetCount(&cnt);
	if (ret < 0)
		return ret;

	/* Buffer length */
	len = sizeof(struct discHdr) * cnt;

	/* Allocate memory */
	buffer = (struct discHdr *)memalign(32, len);
	if (!buffer)
		return -1;

	/* Clear buffer */
	memset(buffer, 0, len);

	self.progress+=0.05;
	sprintf(self.debugMsg, localStr("M998", "Getting game list") );
	Paint_Progress(self.progress, NULL);
	
	/* Get header list */
	ret = WBFS_GetHeaders(buffer, cnt, sizeof(struct discHdr));
	if (ret < 0)
		goto err;

	self.progress+=0.05;
	sprintf(self.debugMsg, localStr("M997", "Sorting game list") );
	Paint_Progress(self.progress, NULL);
	
	/* Sort entries */
	qsort(buffer, cnt, sizeof(struct discHdr), __Menu_EntryCmp);

	self.progress+=0.05;
	Paint_Progress(self.progress, NULL);
	
	/* Free memory */
	if (self.gameList)
		free(self.gameList);

	/* Set values */
	self.gameList = buffer;
	self.gameCnt  = cnt;
	COVER_COUNT = self.gameCnt;
	
	Init_Covers();

	self.progress+=0.05;
	Paint_Progress(self.progress, NULL);
	
	/* Reset variables */
	self.gameSelected = self.gameStart = 0;

	//Init Filter
	//InitFilter(&gameFilter, gameList);

	return 0;

err:
	/* Free memory */
	if (buffer)
		free(buffer);

	return ret;
}

bool Init_Game_List(){

	Paint_Progress(self.progress, NULL);
	
	/* Try to open device */
	if (WBFS_Open() >= 0) {
		/* Get game list */
		
		self.progress+=0.05;
		Paint_Progress(self.progress, NULL);
		GetEntries();
		return true;
	}
	else
		return false;
}


void initUSBFS(){

	int ret;

USB_RETRY:
	if(!Init_Game_List())
	{
		while(1)
		{
			WPAD_ScanPads();
			GRRLIB_DrawImg(115, 95, menu_bg_texture, 0, 1, 1, 0xFFFFFFFF);
			GRRLIB_Printf(190, 140, font_texture, settings.fontColor, 1, localStr("M095", "USB Error - NO WBFS Parition.") );
			GRRLIB_Printf(190, 160, font_texture, settings.fontColor, 1, localStr("M096", "Hold 1 And 2 to Format, B to Exit"));
			GRRLIB_Render();
				
			if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_1 && WPAD_ButtonsHeld(0) & WPAD_BUTTON_2)
			{
				//TODO ADD WBFS Format code
				WPAD_ScanPads();
				GRRLIB_DrawImg(115, 95, menu_bg_texture, 0, 1, 1, 0xFFFFFFFF);
				GRRLIB_Printf(190, 140, font_texture, settings.fontColor, 1, localStr("M097", "Finding Partitions..."));
				GRRLIB_Render();
				
				sleep(1);
				
				partitionEntry partitions[MAX_PARTITIONS];
				u32 cnt, sector_size = 2000;
				char txtBuff[MAX_PARTITIONS][256];
				bool valid[MAX_PARTITIONS];
				u32 retv;
				
				retv = Partition_GetEntries(partitions, &sector_size);
				
				for(cnt = 0; cnt < MAX_PARTITIONS; cnt++)
				{
					partitionEntry *entry = &partitions[cnt];
					
					f32 size = entry->size * (sector_size / GB_SIZE);
					
					if(size) {
						sprintf(txtBuff[cnt], localStr("M098", "Partition %d: %.2fGB"), cnt+1, size);
						valid[cnt] = true;
					}
					else
					{
						sprintf(txtBuff[cnt], localStr("M099", "Partition %d: (Can't be formatted)"), cnt+1);
						valid[cnt] = false;
					}
				}
				
				for(cnt = 0; cnt < MAX_PARTITIONS; cnt++)
				{
					if(valid[cnt])
					{
						partitionEntry *entry = &partitions[cnt];
						
						if(entry->size) 
						{
							if(WindowPrompt(localStr("M100", "Do you want to format:"), txtBuff[cnt], &okButton, &noButton))
							{
								WPAD_ScanPads();
								GRRLIB_DrawImg(115, 95, menu_bg_texture, 0, 1, 1, 0xFFFFFFFF);
								GRRLIB_Printf(190, 140, font_texture, settings.fontColor, 1, localStr("M134", "Formatting Partition %s"), txtBuff[cnt]);
								GRRLIB_Printf(190, 140, font_texture, settings.fontColor, 1, localStr("M067", "Please Wait...") );
								GRRLIB_Render();
								
								ret = WBFS_Format(entry->sector, entry->size); 
							
								if(ret < 0)
								{
									WindowPrompt(localStr("M003", "Error:"), localStr("M101", "Failed formatting!"), &okButton, 0);
								}
								else
								{
									WindowPrompt(localStr("M102", "Success:"), localStr("M103", "Format Complete."), &okButton, 0);
								}
								
								goto USB_RETRY;
							}
						}
					}
				}			
						
				goto USB_RETRY;
			}
			
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
			{
				GRRLIB_Exit();
				SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
			}
		}
	}
}
