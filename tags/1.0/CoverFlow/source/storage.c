#include "storage.h"

extern s_self self;
extern s_settings settings;
extern s_title* titleList;
extern int COVER_COUNT;
extern s_coverFlip coverFlip[];
extern s_pointer      pointer;

bool init_usbfs()
{    
   // __Disc_SetLowMem();
	s32 ret;

	Paint_Progress(self.progress, TX.initSys);
	
	/* Initialize system */
	//Sys_Init(); // it is called in main.c yet

	///* Initialize subsystems */
	//Wpad_Init();	

	//Paint_Progress(self.progress, "Init SDHC...");
	/* Mount SDHC */
	//Fat_MountSDHC(); //it is called in Subsystem_Init
	
	self.progress+=0.05;
	Paint_Progress(self.progress, TX.initUSBFS);
	
	/* Initialize DIP module */
	ret = Disc_Init();
	self.progress+=0.05;
	Paint_Progress(self.progress, TX.initDisc);
	
	if (ret < 0) {
		printf(TX.addError);
		printf(TX.errorDIP, ret);

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
		printf(TX.addError);
		printf(TX.errorDIP, ret);

		return false;
	}

	return true;
}

bool saveFile(char* imgPath, struct block file){
			
	/* save png to sd card for future use*/
			
	FILE *f;
	f = fopen(imgPath, "wb");
	if(f)
	{
		fwrite(file.data,1,file.size,f);
		fclose (f);
		return true;
	}
	return false;
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
                        mkdir("txtcodes",S_ISVTX);
		}
		else{
			WindowPrompt(TX.error, TX.errorReadDir, &okButton, NULL);
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
			
			dir = diropen("3dcovers");
	//		WindowPrompt("Cover download",USBLOADER_PATH "/disks/", &okButton, NULL);	
			if(dir == NULL) {
				mkdir("3dcovers", S_ISVTX);
			}
			else{
				dirclose(dir);
			}
			
			dir = diropen("txtcodes"); //dir for storing wiird cheat files
	//		WindowPrompt("Cover download",USBLOADER_PATH "/txtcodes/", &okButton, NULL);
			if(dir == NULL) {
				mkdir("txtcodes", S_ISVTX);
			}
			else{
				dirclose(dir);
			}
		}
		
		else{
			WindowPrompt(TX.error, TX.errorCreateDir, &okButton, NULL);
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

	fp = fopen(USBLOADER_PATH "/wiicoverflow.xml", "r");
	if(fp == NULL)
		createEmptyWiiCoverFlowFile();
	else
		fclose(fp);
}


bool check_write_access(){
    FILE* fp;
    fp = fopen(USBLOADER_PATH "/temp.txt","wb");
    if(fp==NULL)
    {
        WindowPrompt(TX.error, TX.errorSD, &okButton,0);
        return false;
    }
    else
    {
        fclose(fp);
		chdir(USBLOADER_PATH);
        remove("temp.txt");
        return true;
    }
}

void initWBFS(){

	int retries = 10;
	int retried =0;
	int ret;
	bool init=true;
	
	self.progress += .1;
	sprintf(self.debugMsg, TX.initWBFS);
	Paint_Progress(self.progress,self.debugMsg);
	pointer.p_x = 0;
	pointer.p_y = 0;
	pointer.p_ang = 0;
	pointer.p_type = 0;
  INIT_RETRY:
	/* Initialize WBFS */
	ret = WBFS_Init(self.my_wbfsDev);
	if(ret < 0)
	{
		while(1)
		{
			if (retries==0)
			{
				if (!init)
				{
					Subsystem_Init();
					WDVD_Init();
					/* Initialize Wiimote subsystem */
					WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
					init=true;
				}
				WPAD_ScanPads();
				DrawCursor(pointer.p_type, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
				GRRLIB_Render();
				//tell user that no drive was found
				int NoDrive;
				if (retried<7) // if this is any bigger it'll dump
				{
					NoDrive=WindowPrompt(TX.error, TX.errorNoUSBDrv, &okButton, &cancelButton);
				}
				else
				{
					NoDrive=WindowPrompt(TX.error, TX.errorNoUSBDrv, 0, &cancelButton);
				}
				
				if(NoDrive)
				{
					retries = 10;
					retried++;
					//ask user to connect drive
					WindowPrompt(TX.error, TX.connectUSBDrv, &okButton, 0);
				}
				else //pressed cancel so exit to system
				{
					GRRLIB_Exit();
					SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
				}
			}
			//tries to connect 10 times before asking user again
			retries--;
			if (init)
			{
				Subsystem_Close();
				WDVD_Close();
				init=false;
			}
			if (retries<5) IOS_ReloadIOS(249);  // from NeoGamma 4 (don't reload cIOS everytime)
			goto INIT_RETRY;
		}
	}
	if (!init)
	{
		Subsystem_Init();
		WDVD_Init();
		/* Initialize Wiimote subsystem */
		WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	}
}

s32 __Menu_EntryCmp(const void *a, const void *b)
{
	struct discHdr *hdr1 = (struct discHdr *)a;
	struct discHdr *hdr2 = (struct discHdr *)b;

	/* Compare strings */
	
	if(self.usingTitlesTxt){
	
		char title1[MAX_TITLE_LEN];
		char title2[MAX_TITLE_LEN];
	
		sprintf(title1, "%s", hdr1->title);
		sprintf(title2, "%s", hdr2->title);
		
		getTitle(titleList, (char*)hdr1->id, title1);
		getTitle(titleList, (char*)hdr2->id, title2);
		
		return strcasecmp(title1, title2);
	}
	else
		return strcasecmp(hdr1->title, hdr2->title);
}

s32 GetEntries()
{
	self.shift = 0.0;
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
	sprintf(self.debugMsg, TX.gettingGL );
	Paint_Progress(self.progress, self.debugMsg);
	
	/* Get header list */
	ret = WBFS_GetHeaders(buffer, cnt, sizeof(struct discHdr));
	if (ret < 0)
		goto err;

	self.progress+=0.05;
	sprintf(self.debugMsg, TX.sortingGL );
	Paint_Progress(self.progress, self.debugMsg);
	
	/* Sort entries */
	qsort(buffer, cnt, sizeof(struct discHdr), __Menu_EntryCmp);

	self.progress+=0.05;
	Paint_Progress(self.progress, self.debugMsg);
	
	/* Free memory */
	if (self.gameList)
		free(self.gameList);

	/* Set values */
	self.gameList = buffer;
	self.gameCnt  = cnt;
	COVER_COUNT = self.gameCnt;
	
	if(self.gameCnt > 2)
	{
		if ( ((int)((COVER_COUNT/2.0) + 0.5)) - (COVER_COUNT/2.0) == 0) // even # of covers
			self.max_cover = (int)(COVER_COUNT/2.0);
		else // odd # of covers
			self.max_cover = (int)((COVER_COUNT/2.0) - 0.5);
		
		self.min_cover = (-1*((int)((COVER_COUNT/2.0) + 0.5)) +1);
	}
	else if (self.gameCnt == 2)
	{
		self.min_cover = 0;
		self.max_cover = 1;
	}
	else if (self.gameCnt == 1)
	{
		self.min_cover = 0;
		self.max_cover = 0;
	}
	
	Init_Covers();

	self.progress+=0.05;
	Paint_Progress(self.progress, self.debugMsg);
	
	/* Reset variables */
	self.gameSelected = self.gameStart = 0;
	int i = 0;
	for(i = 0; i < self.gameCnt; i++)
	{
		coverFlip[i].flip = false;
		coverFlip[i].angle = 0.0;
	}
	
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

	Paint_Progress(self.progress, TX.initWBFS_GL);
	
	/* Try to open device */
	if (WBFS_Open() >= 0) {
		/* Get game list */
		
		self.progress+=0.05;
		Paint_Progress(self.progress, TX.getEntryWBFS);
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
			GRRLIB_Rectangle(88, 20, 464, 440, 0x44444499, true);
			CFreeTypeGX_DrawText(ttf14pt, 190,140, TX.errorNoWBFS, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
            CFreeTypeGX_DrawText(ttf14pt, 190,160, TX.hold12B, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
			GRRLIB_Render();
				
			if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_1 && WPAD_ButtonsHeld(0) & WPAD_BUTTON_2)
			{
				//TODO ADD WBFS Format code
				WPAD_ScanPads();
				GRRLIB_Rectangle(88, 20, 464, 440, 0x44444499, true);
				CFreeTypeGX_DrawText(ttf14pt, 190,140, TX.findPartitions, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
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
						sprintf(txtBuff[cnt], TX.partition, cnt+1, size);
						valid[cnt] = true;
					}
					else
					{
						sprintf(txtBuff[cnt], TX.partitionNoFormat, cnt+1);
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
							if(WindowPrompt(TX.askFormat, txtBuff[cnt], &okButton, &noButton))
							{
								WPAD_ScanPads();
                                char tTemp[50];
								GRRLIB_Rectangle(88, 20, 464, 440, 0x44444499, true);
                                sprintf(tTemp,TX.formatPartition, txtBuff[cnt]);
								CFreeTypeGX_DrawText(ttf14pt, 190,140, tTemp, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
								CFreeTypeGX_DrawText(ttf14pt, 190,160, TX.pleaseWait, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
								GRRLIB_Render();
								
								ret = WBFS_Format(entry->sector, entry->size); 
							
								if(ret < 0)
								{
									WindowPrompt(TX.error, TX.errorFormat, &okButton, 0);
								}
								else
								{
									WindowPrompt(TX.successFormat, TX.formatComplete, &okButton, 0);
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
