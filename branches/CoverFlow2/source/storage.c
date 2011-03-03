#include "storage.h"
#include "TrackedMemoryManager.h"
#include "mload_modules.h"
#include "sdhc.h"
#include "usbstorage.h"
#include "restart.h"

extern s_self self;
extern s_settings settings;
extern s_title* titleList;
extern int COVER_COUNT;
extern s_coverFlip coverFlip[];
extern s_pointer      pointer;
extern s_path dynPath;
extern s_gameSettings gameSetting;
s32 all_gameCnt;
s32 gameCnt;

bool init_usbfs()
{
   // __Disc_SetLowMem();
	s32 ret;

	Paint_Progress(self.progress, TX.initSys);
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

	SDHC_Init();
	USBStorage_Init();

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

//  DIR_ITER* dir = diropen("SD:/codes");
	DIR_ITER* dir = diropen(dynPath.dir_codes);
    if(dir == NULL)
		{
			//mkdir("SD:/codes", S_ISVTX);
			mkdir(dynPath.dir_codes, S_ISVTX);
        }
        else
		{
            dirclose(dir);
        }
	//dir = diropen(dynPath.dir_usb_loader);
	dir = diropen(dynPath.dir_usb_loader);
	if (dir == NULL) {

		mkdir(dynPath.dir_usb_loader, S_ISVTX);

		result = chdir(dynPath.dir_usb_loader);

		if(result == 0){
			//WindowPrompt("Cover download","result = 0", &okButton, NULL);
			mkdir("disks", S_ISVTX);
			//WindowPrompt("Cover download","result = 0", &okButton, NULL);
			mkdir("covers", S_ISVTX);
			mkdir("games", S_ISVTX);
                        mkdir("3dcovers", S_ISVTX);
                        mkdir("HQcovers", S_ISVTX);
            mkdir("txtcodes", S_ISVTX);

		}
		else{
			WindowPrompt(TX.error, TX.errorReadDir, &okButton, NULL);
		}
	}
	else{

		dirclose(dir);

		result = chdir(dynPath.dir_usb_loader);

		if(result == 0){
			dir = diropen("disks");
			if(dir == NULL) {
				mkdir("disks", S_ISVTX);
			}
			else{
				dirclose(dir);
			}

			dir = diropen("covers");
			if(dir == NULL) {
				mkdir("covers", S_ISVTX);
			}
			else{
				dirclose(dir);
			}

			dir = diropen("3dcovers");
			if(dir == NULL) {
				mkdir("3dcovers", S_ISVTX);
			}
			else{
				dirclose(dir);
			}

                        dir = diropen("HQcovers");
			if(dir == NULL) {
				mkdir("HQcovers", S_ISVTX);
			}
			else{
				dirclose(dir);
			}

			dir = diropen("txtcodes"); //dir for storing wiird cheat files
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

	char fbuff[255];
	FILE* fp;

	sprintf(fbuff, "%s/gamelist.xml", dynPath.dir_usb_loader);
	fp = fopen(fbuff, "r");

	if(fp == NULL)
		createEmptyGameSettingsFile();
	else
		fclose(fp);

	sprintf(fbuff, "%s/wiicoverflow.xml", dynPath.dir_usb_loader);
	fp = fopen(fbuff, "r");

	if(fp == NULL)
		createEmptyWiiCoverFlowFile();
	else
		fclose(fp);
}


bool check_write_access(){

    char fbuff[255];
	FILE* fp;

	sprintf(fbuff, "%s/temp.txt", dynPath.dir_usb_loader);
	fp = fopen(fbuff, "wb");

	if(fp==NULL)
    {
        WindowPrompt(TX.error, TX.errorSD, &okButton,0);
        return false;
    }
    else
    {
        fclose(fp);
		chdir(dynPath.dir_usb_loader);
        remove("temp.txt");
        return true;
    }
}

void initWBFS(){

	int retries = 10;
	int retried =0;
	int ret;
        u32 timeout = 30;
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

  //force to USB for now
  self.my_wbfsDev = WBFS_DEVICE_USB;

	ret = WBFS_Init(self.my_wbfsDev,timeout);
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
			if (retries<5)				// from NeoGamma 4 (don't reload cIOS everytime)
				{
				  SDHC_Close();	// unmount SD for reloading IOS
                                  USBStorage_Deinit();	// unmount USB for reloading IOS
				  switch (settings.cios)
				  {
					case ios249:
						IOS_ReloadIOS(249);
						break;
					case ios222:
						IOS_ReloadIOS(222);
						//Fat_MountSDHC();
						load_ehc_module();
						break;
					default:
						IOS_ReloadIOS(249);
						break;
				  }
				  SDHC_Init(); 		// now mount SD:/
                                  USBStorage_Init(); 	// and mount USB:/
				}
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
	buffer = (struct discHdr *)CFMemAlign(32, len);
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
		CFFree(self.gameList);
#ifdef FAVTEST
        /* check for favorite mode */
        if(settings.favorites)
            cnt = filterFavorites(buffer,cnt);
#endif
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
		CFFree(buffer);

	return ret;
}


s32 GetEntriesSilent()
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
	buffer = (struct discHdr *)CFMemAlign(32, len);
	if (!buffer)
		return -1;

	/* Clear buffer */
	memset(buffer, 0, len);

	/* Get header list */
	ret = WBFS_GetHeaders(buffer, cnt, sizeof(struct discHdr));
	if (ret < 0)
		goto err;

	/* Sort entries */
	qsort(buffer, cnt, sizeof(struct discHdr), __Menu_EntryCmp);

	/* Free memory */
	if (self.gameList)
		CFFree(self.gameList);
#ifdef FAVTEST
        /* check for favorite mode */
        if(settings.favorites)
            cnt = filterFavorites(buffer,cnt);
#endif
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

	/* Reset variables */
	self.gameSelected = self.gameStart = 0;
	int i = 0;
	for(i = 0; i < self.gameCnt; i++)
	{
		coverFlip[i].flip = false;
		coverFlip[i].angle = 0.0;
	}

	return 0;

err:
	/* Free memory */
	if (buffer)
		CFFree(buffer);

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
/* this needs to contain the code to look up the saved setting for the device to mount, if there, mount it
 * if not, get a list of available devices/partitions and let the user select one, mount it, if it mounts
 * save the setting and get the gamelist, otherwise ask the user again...
 */

  	u32 timeout = 30;
	s32 ret;
	static int first_time = 1;
	int save_wbfsDev = wbfsDev;

	//if (CFG.device == CFG_DEV_USB) {
		wbfsDev = WBFS_DEVICE_USB; //force USB device for now
	//	printf("\n");
	//	printf_(gt("[ USB Mass Storage Device ]"));
	//	printf("\n\n");
		goto mount_dev;
	//}
/*
	if (CFG.device == CFG_DEV_SDHC) {
		wbfsDev = WBFS_DEVICE_SDHC;
		printf("\n");
		printf_(gt("[ SD/SDHC Card ]"));
		printf("\n\n");
		goto mount_dev;
	}
*/
	restart:

	/* Ask user for device */
	for (;;) {
		const char *devname = "Unknown";
                char options[30];
		/* Set device name */
		devname = get_dev_name(wbfsDev);
                GRRLIB_Rectangle(88, 20, 464, 440, 0x44444499, true);
		CFreeTypeGX_DrawText(ttf14pt, 190,140, "Select WBFS Device:", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                sprintf(options,"< %s >\n\n", devname);
                CFreeTypeGX_DrawText(ttf14pt, 190,160, options, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);


                while(1)
                {
                    WPAD_ScanPads();
                    GetWiimoteData();
                    if((WPAD_ButtonsDown(0) & WPAD_BUTTON_B) || (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)) //b or home to exit
                    {
                        if (!first_time && WBFS_Selected()) {
				wbfsDev = save_wbfsDev;
				return;
			}
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_LEFT)
                    {
			if ((--wbfsDev) < WBFS_MIN_DEVICE)
				wbfsDev = WBFS_MAX_DEVICE;
                    }

                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_RIGHT)
                    {
			if ((++wbfsDev) > WBFS_MAX_DEVICE)
				wbfsDev = WBFS_MIN_DEVICE;
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
			break;
                    DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF); //HAND!
                    GRRLIB_Render();
                }
	}

	mount_dev:
	//CFG.device = CFG_DEV_ASK; // next time ask

	WindowPrompt("Mounting device", "please wait...",0,0);

	/* Initialize WBFS */
	ret = WBFS_Init(wbfsDev, timeout);
        char tTemp[50];
	if (ret < 0) {
                memset(tTemp,0,50);
                sprintf(tTemp,"ret = %d", ret);
		WindowPrompt("ERROR!",tTemp,&okButton,0);
		//printf_(gt("Make sure USB port 0 is used!\n"
		//		"(The one nearest to the edge)"));

		/* Restart wait */
		Restart_Wait();
	}
	usleep(100000); // 100ms

	// Mount usb fat partition if not already
	// This is after device init, because of the timeout handling
	Fat_MountUSB();

	all_gameCnt = gameCnt = 0;
	/* Try to open device */
	//WBFS_Open();
	if (strcasecmp(settings.partition, "ask") == 0) { // TODO set partition to "ask" if user wants to change partition
		goto jump_to_selection;
	}
	WBFS_OpenNamed(settings.partition);

	while (!WBFS_Selected()) {

                GRRLIB_Rectangle(88, 20, 464, 440, 0x44444499, true);
                sprintf(tTemp,"Partition: %s not found! Try another:", settings.partition);
		CFreeTypeGX_DrawText(ttf14pt, 190,140, tTemp, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                CFreeTypeGX_DrawText(ttf14pt, 190,160, "A=select B=change part", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);

/*
		printf_(gt("Partition: %s not found!"), CFG.partition);
		printf("\n");
		printf_(gt("Select a different partition"));
		printf("\n");
		if (!CFG.disable_format) {
			printf_(gt("or format a WBFS partition."));
			printf("\n");
		}
		printf("\n");
*/

/*
		printf_h(gt("Press %s button to select a partition."), (button_names[CFG.button_confirm.num]));
		printf("\n");
		printf_h(gt("Press %s button to change device."), (button_names[CFG.button_cancel.num]));
		printf("\n");
		printf_h(gt("Press %s button to exit."), (button_names[CFG.button_exit.num]));
		printf("\n\n");
*/

		// clear button states
		WPAD_Flush(WPAD_CHAN_ALL);
                while(1)
                {
                    WPAD_ScanPads();
                    GetWiimoteData();
                    if((WPAD_ButtonsDown(0) & WPAD_BUTTON_B) || (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)) //b or home to exit
                    {
                        goto restart;
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
			break;
                    DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF); //HAND!
                    GRRLIB_Render();
                }

		// Select or Format partition
		jump_to_selection:
		Menu_Partition(false);
	}

	/* Get game list */
	GetEntries();

	first_time = 0;
}
void Menu_Partition(bool must_select)
{
	PartList plist;
	partitionEntry *entry;
	PartInfo *pinfo;

	int i;
	s32 ret = 0;

//rescan:

	/* Get partition entries */
	ret = Partition_GetList(wbfsDev, &plist);
	if (ret < 0) {
		WindowPrompt("ERROR:","No partitions found!",&okButton,0);
		return;
	}

	//struct Menu menu;
	//char active[MAX_PARTITIONS_EX];
	//menu_init(&menu, plist.num);
	//menu_init_active(&menu, active, MAX_PARTITIONS_EX);

	//menu_begin(&menu);
	/* Clear console */
        int invalid_ext = -1;
	for (i = 0; i < plist.num; i++) {
		partitionEntry *entry = &plist.pentry[i];
		if (part_is_extended(entry->type) && plist.pinfo[i].wbfs_i && i < 4) {
			invalid_ext = i;
		}
	}
        for (;;) {
		
                char options[40];
                GRRLIB_Rectangle(88, 20, 464, 440, 0x44444499, true);
		CFreeTypeGX_DrawText(ttf14pt, 190,140, "Select Partition:", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                
                while(1)
                {
                    WPAD_ScanPads();
                    GetWiimoteData();
                    if((WPAD_ButtonsDown(0) & WPAD_BUTTON_B) || (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)) //b or home to exit
                    {
                        if (must_select) {
			if (WBFS_Selected()) return;
                        WindowPrompt("Error","No partition selected",&okButton,0);
			sleep(2);
                        } else {
			return;
		}
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_LEFT)
                    {
			if ((--i) < 0)
				i = plist.num;
                    }

                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_RIGHT)
                    {
			if ((++i) > plist.num)
				i = 0;
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
			entry = &plist.pentry[i];
		pinfo = &plist.pinfo[i];
		int idx = pinfo->wbfs_i;
		int part_fs = PART_FS_WBFS;
		if (pinfo->fat_i) {
			WindowPrompt("Opening", "FAT partition...",0,0);
			idx = pinfo->fat_i;
			part_fs = PART_FS_FAT;
		}
		if (pinfo->ntfs_i) {
			WindowPrompt("Opening", "NTFS partition...",0,0);
			idx = pinfo->ntfs_i;
			part_fs = PART_FS_NTFS;
		}

		if (idx) {
			ret = WBFS_OpenPart(part_fs, idx, entry->sector,
					entry->size, settings.partition);
			if (ret == 0 && WBFS_Selected()) {

				return;
			}
		}
                    PartInfo *pinfo = &plist.pinfo[i];
                    sprintf(options,"< %d >\n\n", pinfo->fs_type);
                    CFreeTypeGX_DrawText(ttf14pt, 190,160, options, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                    DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF); //HAND!
                    GRRLIB_Render();
                }
	}


/*
	printf_x(gt("Select a partition"));
	printf(":\n\n");
	DefaultColor();
	printf_("[ %s ]\n\n", get_dev_name(wbfsDev));

	printf_("P# Size(GB) Type Mount Used\n");
	printf_("-----------------------------\n");
	//       P#1  400.00 FAT1  usb:
	//       P#2  400.00 FAT2  wbfs: [USED]
	//       P#3  400.00 WBFS1
	//       P#4  400.00 NTFS

	int invalid_ext = -1;
	for (i = 0; i < plist.num; i++) {
		partitionEntry *entry = &plist.pentry[i];
		active[i] = part_valid_data(entry);
		MENU_MARK();
		print_part(i, &plist);
		printf("\n");
		if (part_is_extended(entry->type) && plist.pinfo[i].wbfs_i && i < 4) {
			invalid_ext = i;
		}
	}
	printf("\n");
	printf_h(gt("Press %s button to select."), (button_names[CFG.button_confirm.num]));
	printf("\n");
	printf_h(gt("Press %s button to go back."), (button_names[CFG.button_cancel.num]));
	printf("\n");
	if (!CFG.disable_format) {
		printf_h(gt("Press %s button to format WBFS."), (button_names[NUM_BUTTON_PLUS]));
		printf("\n");
		printf_h(gt("Press %s button to delete FS."), (button_names[NUM_BUTTON_MINUS]));
		printf("\n");
		if (invalid_ext >= 0) {
			printf_("\n");
			printf_(gt("NOTE: partition P#%d is type EXTEND but\n"
					"contains a WBFS filesystem. This is an\n"
					"invalid setup. Press %s to change the\n"
					"partition type from EXTEND to data."), invalid_ext+1, (button_names[CFG.button_other.num]));
			printf("\n");
			printf_h(gt("Press %s button to fix EXTEND/WBFS."), (button_names[CFG.button_other.num]));
			printf("\n");
		}
	}

	u32 buttons = Wpad_WaitButtonsCommon();

	menu_move(&menu, buttons);

	// B button
	if (buttons & CFG.button_cancel.mask) {
		if (must_select) {
			if (WBFS_Selected()) return;
			printf("\n");
			printf_(gt("No partition selected!"));
			printf("\n");
			sleep(2);
		} else {
			return;
		}
	}

	// A button
	if (buttons & CFG.button_confirm.mask) {
		i = menu.current;
		entry = &plist.pentry[i];
		pinfo = &plist.pinfo[i];
		int idx = pinfo->wbfs_i;
		int part_fs = PART_FS_WBFS;
		if (pinfo->fat_i) {
			printf(gt("Opening FAT partition..."));
			printf("\n");
			idx = pinfo->fat_i;
			part_fs = PART_FS_FAT;
		}
		if (pinfo->ntfs_i) {
			printf(gt("Opening NTFS partition..."));
			printf("\n");
			idx = pinfo->ntfs_i;
			part_fs = PART_FS_NTFS;
		}
		__console_flush(0);
		if (idx) {
			ret = WBFS_OpenPart(part_fs, idx, entry->sector,
					entry->size, CFG.partition);
			if (ret == 0 && WBFS_Selected()) {
				if (must_select) {
					// called from global options
					__Menu_GetEntries();
				}
				return;
			}
		}
	}

	// +/- button
	if ((buttons & WPAD_BUTTON_MINUS || buttons & WPAD_BUTTON_PLUS)
			&& !CFG.disable_format)
	{
		i = menu.current;
		entry = &plist.pentry[i];
		if (part_valid_data(entry)) {
			const char *act = gt("format");
			if (buttons & WPAD_BUTTON_MINUS) act = gt("delete");
			printf("\n");
			printf_x(gt("Are you sure you want to %s\n"
					"this partition?"), act);
			printf("\n\n");
			printf_("");
			print_part(i, &plist);
			printf("\n\n");
			if (Menu_Confirm(0)) {
				printf("\n");
				Menu_Format_Partition(entry, buttons & WPAD_BUTTON_MINUS);
				goto rescan;
			}
		}
	}
	// 1 button
	if (buttons & CFG.button_other.mask
		&& !CFG.disable_format
		&& invalid_ext >= 0)
	{
		printf("\n");
		printf_x(gt("Are you sure you want to FIX\n"
				"this partition?"));
		printf("\n\n");
		printf_("");
		print_part(invalid_ext, &plist);
		printf("\n\n");
		if (Menu_Confirm(0)) {
			printf("\n");
			printf_(gt("Fixing EXTEND partition..."));
			ret = Partition_FixEXT(wbfsDev, invalid_ext, plist.sector_size);
			printf("%s\n", ret ? gt("FAIL") : gt("OK"));
			printf_(gt("Press any button..."));
			Wpad_WaitButtonsCommon();
			goto rescan;
		}
	}

	goto loop;
*/
}
/*




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
//				u32 retv;
// NOT USED ANYMORE NEED TO REPLACE
//				retv = Partition_GetEntries(partitions, &sector_size);
//XFATX
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
*/
int filterFavorites(struct discHdr *list, int cnt)
{
	int i;
	for (i=0; i<cnt;) {
		if (!isFavorite(list[i].id)) {
			memcpy(list+i, list+i+1, (cnt-i-1) * sizeof(struct discHdr));
			cnt--;
		}
                else
			i++;

	}
	return cnt;
}

bool isFavorite(u8 *id)
{
        char titleID[7];
	sprintf(titleID, "%s", id);
	if(getGameSettings(titleID, &gameSetting))
        {
            if(gameSetting.favorite==1)
                return true;
        }
        return false;
}

const char *get_dev_name(int dev)
{
	switch (dev) {
		case WBFS_DEVICE_USB: return "USB Mass Storage Device";
		case WBFS_DEVICE_SDHC: return "SD/SDHC Card";
	}
	return "Unknown";
}