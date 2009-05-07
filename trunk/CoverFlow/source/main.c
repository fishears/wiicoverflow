#include "coverflow.h"

#include "version.h"

//static char prozent[MAX_CHARACTERS + 16];
//static char timet[MAX_CHARACTERS + 16];
static u8 CalculateFrameRate();

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

//hook types for ocarina
char hooks[3][13] =
{{"VI (Default)"},
{"Wii Pad"},
{"GC Pad"}};

/* Gamelist buffer */
static struct discHdr *gameList = NULL;

static wbfs_t *hdd = NULL;

/* WBFS device */
static s32 my_wbfsDev = WBFS_DEVICE_USB;

s_self self;
s_pointer pointer;

#ifdef TEST_MODE
int COVER_COUNT = 29;
#else
int COVER_COUNT = 0;
#endif


float SCROLL_SPEED = 0.050;

bool imageNotFound = false;

//char* _title;
//char* _msg;

/*--------------------------------------*/
#include "settings.h"


Mtx GXmodelView2D;

//int self.array_size = 0;


//void Download_Cover(struct discHdr *header, int v, int max);
//void batchDownloadCover();
int WindowPrompt(char* title, char* txt, struct Button* choice_a, struct Button* choice_b);

int buffer_window_min = 0;
int buffer_window_max = 0;
int oldmin = 0;
int oldmax = 0;

void UpdateBufferedImages()
{
	int i;
	int index = 0;
	
	for(i = (-1*(COVER_COUNT/2.0)); i < (COVER_COUNT/2.0); i++)
	{
		index = i+(COVER_COUNT/2.0);
		if(index < self.gameCnt)
		{
			/*Some logic to avoid drawing everything*/
			if(abs(self.shift+i) <= BUFFER_WINDOW)
			{
				//Is this cover already loaded?
				if(!BUFFER_IsCoverReady(index))
				{
					//Is this cover already queued up?
					if(!BUFFER_IsCoverQueued(index))
					{
						//Request this cover
						struct discHdr *header = &gameList[index];
		
						BUFFER_RequestCover(index, header);
					}
				}
			}
			else
			{
				if(BUFFER_IsCoverReady(index))
				{
					//TODO Fix this
					//calling this fixes the fuked up graphics when you load
					//too many covers, but it causes a code dump eventually... HELP
					BUFFER_RemoveCover(index);
				}
			}
		}
	}
	
}

void quit()
{
	//we should free all allocated textures (SCO);
	GRRLIB_Exit();
	exit(0);
}

s32 __Menu_EntryCmp(const void *a, const void *b)
{
	struct discHdr *hdr1 = (struct discHdr *)a;
	struct discHdr *hdr2 = (struct discHdr *)b;

	/* Compare strings */
	return strcmp(hdr1->title, hdr2->title);
}

s32 GetEntries(void)
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
	Paint_Progress(self.progress, NULL);
	
	/* Get header list */
	ret = WBFS_GetHeaders(buffer, cnt, sizeof(struct discHdr));
	if (ret < 0)
		goto err;

	self.progress+=0.05;
	Paint_Progress(self.progress, NULL);
	
	/* Sort entries */
	qsort(buffer, cnt, sizeof(struct discHdr), __Menu_EntryCmp);

	self.progress+=0.05;
	Paint_Progress(self.progress, NULL);
	
	/* Free memory */
	if (gameList)
		free(gameList);

	/* Set values */
	gameList = buffer;
	self.gameCnt  = cnt;
	COVER_COUNT = self.gameCnt;
	
	Init_Covers();

	self.progress+=0.05;
	Paint_Progress(self.progress, NULL);
	
	/* Reset variables */
	self.gameSelected = self.gameStart = 0;

	return 0;

err:
	/* Free memory */
	if (buffer)
		free(buffer);

	return ret;
}

bool Init_Game_List(void)
{

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
	{
		return false;
	}
}

void DragSlider(int xPos)
{

	int min_loc = 126;
	int max_loc = 439;

	self.shift = -1*(change_scale(xPos, min_loc, max_loc, -1*(COVER_COUNT/2.0), COVER_COUNT/2.0));
}

int DiscWait()
{
    u32 cover = 0;
	s32 ret = 0;

	while(!(cover & 0x2))
	{
		//TODO Add GUI For Cancel Button
		
		ret = WDVD_GetCoverStatus(&cover);
		if (ret < 0)
			return ret;
	}

	return ret;
}

void Graphic_Settings_Menu(void)
{
	bool doloop = true;
	//bool dummy = false;

	/*Render and control Settings*/
	do{

		WPAD_ScanPads();
		
		ir_t ir; // The struct for infrared
		
		WPAD_IR(WPAD_CHAN_0, &ir); // Let's get our infrared data
		wd = WPAD_Data(WPAD_CHAN_0);

		pointer.p_x = ir.sx-200;
		pointer.p_y = ir.sy-250;
		pointer.p_ang = ir.angle/2; // Set angle/2 to translate correctly

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			doloop = false;
		}
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
		{
			doloop = false;
		}
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
		{
			if (Button_Select(&settingsButton, pointer.p_x, pointer.p_y))
			{
				doloop = false;
			}
			else if (Button_Select(&resetButton, pointer.p_x, pointer.p_y))
			{
				SETTINGS_Init();
			}
			else if (Button_Select(&windowdownButton, pointer.p_x, pointer.p_y))
			{
				if (SETTING_drawWindow > 1)
					SETTING_drawWindow -= 1;
			}
			else if (Button_Select(&windowupButton, pointer.p_x, pointer.p_y))
			{
				if (SETTING_drawWindow < 100) // Allow for user to shoot self in foot
					SETTING_drawWindow += 1;
			}
			else if (Button_Select(&coverTextOnButton, pointer.p_x, pointer.p_y) || Button_Select(&coverTextOffButton, pointer.p_x, pointer.p_y))
			{
				SETTING_coverText = (SETTING_coverText) ? 0 : 1;
			}
		}
		
		if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_A)
		{
			if(Button_Select(&spacingdownButton, pointer.p_x, pointer.p_y))
			{
				SETTING_coverSpacing -= 0.05;
			}
			else if(Button_Select(&spacingupButton, pointer.p_x, pointer.p_y))
			{
				SETTING_coverSpacing += 0.05;
			}
			else if(Button_Select(&angledownButton, pointer.p_x, pointer.p_y))
			{
				SETTING_coverAngle -= 1;
			}
			else if(Button_Select(&angleupButton, pointer.p_x, pointer.p_y))
			{
				SETTING_coverAngle += 1;
			}
			else if(Button_Select(&zoomdownButton, pointer.p_x, pointer.p_y))
			{
				SETTING_coverZoom -= 0.01;
			}
			else if(Button_Select(&zoomupButton, pointer.p_x, pointer.p_y))
			{
				SETTING_coverZoom += 0.01;
			}
		}
		
		/*Draw Covers*/ //PREVIEW
		draw_covers();
		// Draw menu dialog background
		GRRLIB_2D_Init();
		GRRLIB_DrawImg(115, 95, menu_bg_texture, 0, 1, 1.4, 0xFFFFFFFF);
		// Draw text
		GRRLIB_Printf(204, 60,  font_texture, SETTING_fontColor, 1.3, "Coverflow Settings (GFX)");
		GRRLIB_Printf(145, 95, font_texture, SETTING_fontColor, 1, "Zoom:");
		GRRLIB_Printf(330, 95, font_texture, SETTING_fontColor, 1, "%f", SETTING_coverZoom);
		GRRLIB_Printf(145, 143, font_texture, SETTING_fontColor, 1, "Spacing:");
		GRRLIB_Printf(330, 143, font_texture, SETTING_fontColor, 1, "%f", SETTING_coverSpacing);
		GRRLIB_Printf(145, 191, font_texture, SETTING_fontColor, 1, "Angle:");
		GRRLIB_Printf(330, 191, font_texture, SETTING_fontColor, 1, "%f", SETTING_coverAngle);
		GRRLIB_Printf(145, 239, font_texture, SETTING_fontColor, 1, "Draw Window:");
		GRRLIB_Printf(330, 239, font_texture, SETTING_fontColor, 1, "%d", SETTING_drawWindow);
		GRRLIB_Printf(145, 287, font_texture, SETTING_fontColor, 1, "Game Title:");
		
		//Button_Theme_Paint(&settingsButton, SETTING_theme);
		Button_Paint(&spacingupButton);
		Button_Paint(&spacingdownButton);
		Button_Paint(&zoomupButton);
		Button_Paint(&zoomdownButton);
		Button_Paint(&angleupButton);
		Button_Paint(&angledownButton);
		Button_Paint(&windowupButton);
		Button_Paint(&windowdownButton);
		if (SETTING_coverText)
		{
			Button_Paint(&coverTextOnButton);
		}
		else Button_Paint(&coverTextOffButton);
		Button_Paint(&resetButton);
	
		// Check for button-pointer intersections, and rumble
		if (Button_Hover(&spacingupButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&spacingdownButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&zoomupButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&zoomdownButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&windowupButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&windowdownButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&windowupButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&coverTextOnButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&coverTextOffButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&resetButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&angleupButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&angledownButton, pointer.p_x, pointer.p_y))
		{
			// Should we be rumbling?
			if (--self.rumbleAmt > 0)
				WPAD_Rumble(0,1); // Turn on Wiimote rumble
			else 
				WPAD_Rumble(0,0); // Kill the rumble
		}
		else
		{ // If no button is being hovered, kill the rumble
			WPAD_Rumble(0,0);
			self.rumbleAmt = 5;
		}

		// Draw the pointer hand
		GRRLIB_DrawImg(pointer.p_x, pointer.p_y, pointer_texture, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		// Spit it out
		GRRLIB_Render();
		
	}while(doloop);
}

void Settings_Menu(void)
{
	bool doloop = true;

	/*Render and control Settings*/
	do{
		WPAD_ScanPads();
		
		ir_t ir; // The struct for infrared
		WPAD_IR(WPAD_CHAN_0, &ir); // Let's get our infrared data
		wd = WPAD_Data(WPAD_CHAN_0);

		pointer.p_x = ir.sx-200;
		pointer.p_y = ir.sy-250;
		pointer.p_ang = ir.angle/2; // Set angle/2 to translate correctly


		// Handle button events
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME || WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
		{
			doloop = false;
		}
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
		{
			if (Button_Select(&settingsButton, pointer.p_x, pointer.p_y))
			{
				doloop = false; // Clicked the setting button, exit to main screen
			}
			else if (Button_Select(&cheatonButton, pointer.p_x, pointer.p_y) || Button_Select(&cheatoffButton, pointer.p_x, pointer.p_y))
			{
				CFG.ocarina = (CFG.ocarina) ? 0 : 1; // Clicked the Ocarina button, toggle state
			}
			else if (Button_Select(&vidtvonButton, pointer.p_x, pointer.p_y) || Button_Select(&vidtvoffButton, pointer.p_x, pointer.p_y))
			{
				CFG.vipatch = (CFG.vipatch) ? 0 : 1; // Clicked the VIPATCH button, toggle state
			}
			else if (Button_Select(&graphicsButton, pointer.p_x, pointer.p_y))
			{
				Graphic_Settings_Menu(); // Clicked the Graphics Setting button, launch menu routine
			}
			else if (Button_Select(&langdownButton, pointer.p_x, pointer.p_y))
			{ // Clicked on the language buttons
				if (CFG.language > 0)
				{
					CFG.language --;
				}
				else
				{
					CFG.language = (CFG_LANG_COUNT - 1);
				}
			}
			else if (Button_Select(&langupButton, pointer.p_x, pointer.p_y))
			{
				if (CFG.language < (CFG_LANG_COUNT - 1))
				{
					CFG.language ++;
				}
				else
				{
					CFG.language = 0;
				}
			}
			else if (Button_Select(&downloadButton, pointer.p_x, pointer.p_y))
			{
				// Clicked on the Download Covers button
				if (WindowPrompt("Cover download","This operation can't be canceled, continue?", &okButton, &cancelButton))
				{
					batchDownloadCover(gameList);
				}
			}
			else if (Button_Select(&viddownButton, pointer.p_x,pointer.p_y))
			{
				// Clicked on the video down button
				if (CFG.video > 0)
				{
					CFG.video --;
				}
				else
				{
					CFG.video = (CFG_VIDEO_COUNT -1);
				}
			}
			else if (Button_Select(&vidupButton, pointer.p_x,pointer.p_y))
			{
				// Clicked on the video up button
				if (CFG.video <(CFG_VIDEO_COUNT -1))
				{
					CFG.video ++;
				}
				else
				{
					CFG.video = 0;
				}
			}
			else if (Button_Select(&themeWhiteButton, pointer.p_x, pointer.p_y) || Button_Select(&themeBlackButton, pointer.p_x, pointer.p_y))
			{
				// Clicked on the Theme button, toggle state
				SETTING_theme = (SETTING_theme) ? 0 : 1;
				if (SETTING_theme)
				{	// black fonts for white theme
					SETTING_fontColor = 0x000000FF;
				}
				else
				{   // white fonts for black theme
					SETTING_fontColor = 0xFFFFFFFF;
				}
				
			}
		}
		
		// Draw screen background
		draw_covers();
		// Draw menu dialog background
		GRRLIB_2D_Init();
		GRRLIB_DrawImg(115, 95, menu_bg_texture, 0, 1, 1.4, 0xFFFFFFFF);
		// Draw text
		GRRLIB_Printf(204, 60,  font_texture, SETTING_fontColor, 1.3, "Coverflow Settings");
		GRRLIB_Printf(145, 93,  font_texture, SETTING_fontColor, 1, "Ocarina:");
		GRRLIB_Printf(145, 128, font_texture, SETTING_fontColor, 1, "Language:");
		GRRLIB_Printf(330, 128, font_texture, SETTING_fontColor, 1, "%s",languages[CFG.language]);
		GRRLIB_Printf(145, 157, font_texture, SETTING_fontColor, 1, "Video mode:");
		GRRLIB_Printf(365, 155, font_texture, SETTING_fontColor, 1, "%s",vidmodes[CFG.video]);
		GRRLIB_Printf(145, 189, font_texture, SETTING_fontColor, 1, "VIDTV patch:");
		GRRLIB_Printf(145, 221, font_texture, SETTING_fontColor, 1, "Graphics:");
		GRRLIB_Printf(145, 260, font_texture, SETTING_fontColor, 1, "Missing Covers?:");
		GRRLIB_Printf(145, 300, font_texture, SETTING_fontColor, 1, "Theme:");
		GRRLIB_Printf(218, 340, font_texture, SETTING_fontColor, 1.15, "Press B to Cancel");

		// Draw stateless buttons
		Button_Paint(&langupButton);
		Button_Paint(&langdownButton);
		Button_Paint(&vidupButton);
		Button_Paint(&viddownButton);
		Button_Paint(&graphicsButton);
		Button_Paint(&downloadButton);
		// Draw stateful buttons
		Button_Toggle_Paint(&cheatoffButton, &cheatonButton, CFG.ocarina);
		Button_Toggle_Paint(&vidtvoffButton, &vidtvonButton, CFG.vipatch);
		Button_Toggle_Paint(&themeBlackButton, &themeWhiteButton, SETTING_theme);
//		Button_Theme_Paint(&settingsButton, SETTING_theme);
		
		// Check for button-pointer intersections, and rumble
		if (Button_Hover(&langupButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&langdownButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&vidupButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&viddownButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&cheatoffButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&cheatonButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&vidtvoffButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&vidtvonButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&themeBlackButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&themeWhiteButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&graphicsButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&downloadButton, pointer.p_x, pointer.p_y))
		{
			// Should we be rumbling?
			if (--self.rumbleAmt > 0)
				WPAD_Rumble(0,1); // Turn on Wiimote rumble
			else 
				WPAD_Rumble(0,0); // Kill the rumble
		}
		else
		{ // If no button is being hovered, kill the rumble
			WPAD_Rumble(0,0);
			self.rumbleAmt = 5;
		}
		
		// Draw the pointer hand
		GRRLIB_DrawImg(pointer.p_x, pointer.p_y, pointer_texture, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		// Spit it out
		GRRLIB_Render();
		
	}while(doloop);
}


bool Menu_Install(void)
{

	if(!WindowPrompt ("Install new Game?", "Place disk in drive and hit ok.",&okButton,&cancelButton))
		return false;
	
    static struct discHdr headerdisc ATTRIBUTE_ALIGN(32);
	
	WindowPrompt ("Initializing DVD Drive", "Please Wait...",0,0);
	
	/* Disable WBFS mode */
	Disc_SetWBFS(0, NULL);
	
    int ret, choice = 0;
	char *name;
	static char buffer[MAX_CHARACTERS + 4];

	ret = Disc_Wait();
	if (ret < 0) {
		WindowPrompt ("Error reading Disc",0,&cancelButton,0);
		return false;
	}
	ret = Disc_Open();
	if (ret < 0) {
		WindowPrompt ("Could not open Disc",0,&cancelButton,0);
		return false;
	}

	ret = Disc_IsWii();
	
	if (ret < 0) {
		choice = WindowPrompt ("Not a Wii Disc","Insert a Wii Disc!",&okButton,&cancelButton);

		if (!choice) {
			return false;
		}
		else
		{
			return Menu_Install();
		}
	}
	
	Disc_ReadHeader(&headerdisc);
	name = headerdisc.title;
	if (strlen(name) < (22 + 3)) {
			memset(buffer, 0, sizeof(buffer));
			sprintf(name, "%s", name);
		} else {
			strncpy(buffer, name,  MAX_CHARACTERS);
			strncat(buffer, "...", 3);
			sprintf(name, "%s", buffer);
	}

	ret = WBFS_CheckGame(headerdisc.id);
	if (ret) {
		WindowPrompt ("Game is already installed:",name,&cancelButton,0);
		return false;
	}
	hdd = GetHddInfo();
	if (!hdd) {
		WindowPrompt ("No HDD found!","Error!!",&cancelButton,0);
		return false;
		}

	f32 freespace, used;

	WBFS_DiskSpace(&used, &freespace);
	u32 estimation = wbfs_estimate_disc(hdd, __WBFS_ReadDVD, NULL, ONLY_GAME_PARTITION);
	f32 gamesize = ((f32) estimation)/1073741824;
	char gametxt[50];
	
	sprintf(gametxt, "Installing game %.2fGB:", gamesize);
	
	char ttext[50];
	char tsize[50];
	sprintf(ttext, "Install %s?", name);
	sprintf(tsize, "Game Size: %.2fGB", gamesize);
	
	
	if(WindowPrompt (ttext,tsize,&okButton,&cancelButton))
	{
		if (gamesize > freespace) {
			char errortxt[50];
			sprintf(errortxt, "Game Size: %.2fGB, Free Space: %.2fGB", gamesize, freespace);
			WindowPrompt("Not enough free space!",errortxt,&cancelButton, 0);
			return false;
		}
		else {
			ret = ProgressWindow(hdd, gametxt, name);
			if (ret != 0) {
				WindowPrompt ("Install error!",0,&cancelButton,0);
				return false;
			} else {
				BUFFER_ClearCovers();
				Sleep(300);
				GetEntries();
				Sleep(300);
				UpdateBufferedImages();
				Sleep(100);
				
				WindowPrompt ("Successfully installed:",name,&okButton,0);
				return true;
			}
		}
	}
	
	return false;
}

void AddGame(void)
{
	Menu_Install();
}

bool Menu_Delete(void)
{
	struct discHdr *header = NULL;
 	char gameName[31]; 
	
	/* No game list */
	if (!self.gameCnt)
		return false;

	/* Selected game */
	header = &gameList[self.gameSelected];

	if(strlen(get_title(header)) < 30) {
		sprintf(gameName, "%s", get_title(header));
	}
	else
	{
		strncpy(gameName, get_title(header), 27);
		gameName[27] = '\0';
		strncat(gameName, "...", 3);
	}

	if(WindowPrompt("Do you want to delete:", gameName, &yesButton, &noButton))
	{
		if(0 > WBFS_RemoveGame(header->id))
		{
			WindowPrompt("Delete Failed.", "Could not delete game.", &okButton, 0);
		}
		else
		{
			BUFFER_ClearCovers();
			Sleep(300);
			GetEntries();
			Sleep(300);
			UpdateBufferedImages();
			Sleep(100);
			
			WindowPrompt("Successfully deleted.", "Press Ok to Continue.", &okButton, 0);
			return true;
		}
	}
	
	return false;
}
		

bool Menu_Boot(void)
{
	#ifndef TEST_MODE
	struct discHdr *header = NULL;
	//int i = 0;

	/* No game list */
	if (!self.gameCnt)
		return false;

	/* Selected game */
	header = &gameList[self.gameSelected];

    GRRLIB_Exit();
	
	free(cover_texture.data);
	free(back_texture.data);
	free(empty_texture.data);
	free(no_disc_texture.data);
	free(current_cover_texture.data);
	free(font_texture.data);

	BUFFER_ClearCovers();
	BUFFER_KillBuffer();
	Sleep(300);
	
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
	//TODO No really sure how args need to be set up...
	char* buffer;
	buffer = malloc(strlen("bootloader.dol") + 1 + 6 + 1);
	sprintf(buffer, "bootloader.dol%c%s%c", '\0', header->id, '\0');
	
	struct __argv argv;
	bzero(&argv, sizeof(argv));
	argv.argvMagic = ARGV_MAGIC;
	//length is bootloader length + null + header length + null + null
	argv.length = strlen(buffer) + 1;
	argv.commandLine = malloc(argv.length);
	strcpy(argv.commandLine, buffer);
	
	argv.commandLine[argv.length - 1] = '\x00';
	argv.argc = 2;
	argv.argv = & argv.commandLine;
	
	argv.endARGV = argv.argv + 1;
			
	run_dol(bootloader_dol, &argv);
	#endif

	#endif
	
	return true;
}


int Net_Init(char *ip){
	
	s32 res;
	
    while ((res = net_init()) == -EAGAIN)
		usleep(100 * 1000); //100ms
	
    if (if_config(ip, NULL, NULL, true) < 0) {
		WindowPrompt ("ERROR!","Cannot get local IP address.", &okButton, 0);
		usleep(1000 * 1000 * 1); //1 sec
		return false;
	}
	return true;
}

void initVars(){

	pointer.p_x = 0;
	pointer.p_y = 0;
	pointer.p_ang = 0;
	
	self.shift = 0;
	self.select_shift = 0;
	self.gameCnt = 0;
	self.gameSelected = 0;
	self.gameStart = 0;
	self.selected = false;
	self.animate_flip = 0.0;
	self.animate_rotate = 0.0;
	self.array_size = 0;
	self.rumbleAmt = 0;
	self.progress = 0.0;
	self.firstTimeDownload = true;
	self.inetOk = false;
}

bool LaunchGame()
{
	bool done = false;
	while(!done)
	{
		draw_covers();
		
		done = draw_selected_two(gameList, true, false);
		
		GRRLIB_Render();
	}
	
	/*Fade to black*/
	//TODO Fade to black instead of just drawing black
	GRRLIB_FillScreen(0x000000FF);
	GRRLIB_Render();
	
	Menu_Boot();
	
	return false;
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
int main( int argc, char **argv )
{
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
	#ifndef TEST_MODE
	
	/* Load Custom IOS */
	int ret = IOS_ReloadIOS(249);
	/* Check if Custom IOS is loaded */
	if (ret < 0)
	{
		printf("[+] ERROR:\n");
		printf("    Custom IOS could not be loaded! (ret = %d)\n", ret);
		return 0;
	}
	#endif
	
	SETTINGS_Init();
	
	GRRLIB_Init();
    GRRLIB_FillScreen(0x000000FF);
    GRRLIB_Render();
	
	u8 FPS = 0; // frames per second counter
	
	initVars();
	
    loader_main_texture = GRRLIB_LoadTexture(loading_main_png);
    progress_texture    = GRRLIB_LoadTexture(progress_png);

	self.progress += .1;

	sprintf(self.debugMsg, "Loading textures");
	Paint_Progress(self.progress,self.debugMsg);
	
	LoadTextures();		// load textures
	Init_Buttons();		// load buttons so they can be used for error msgs

	self.progress += .1;

	sprintf(self.debugMsg, "Init USB");
	Paint_Progress(self.progress,self.debugMsg);
	
	#ifndef TEST_MODE
	if(!init_usbfs()){
		WindowPrompt ("ERROR!","Cannot init USBFS, quitting.", &okButton, 0);
		return 0;
	}
		
	//LOAD CONFIG
	// need to clean up CFG and move to the XML based settings
	strcpy(CFG.images_path, USBLOADER_PATH);
	CFG.widescreen = 0;
	CFG.download = 1;
	
	sprintf(self.debugMsg, "Initializing WBFS");
	Paint_Progress(self.progress,self.debugMsg);
	
	my_wbfsDev = WBFS_DEVICE_USB;
	
	checkDirs();

  INIT_RETRY:
	/* Initialize WBFS */
	ret = WBFS_Init(my_wbfsDev);
	
	if(ret < 0)
	{
		while(1)
		{
			WPAD_ScanPads();
			GRRLIB_DrawImg(115, 95, menu_bg_texture, 0, 1, 1, 0xFFFFFFFF);
			GRRLIB_Printf(190, 140, font_texture, SETTING_fontColor, 1, "USB Error - Drive not found");
			GRRLIB_Printf(190, 160, font_texture, SETTING_fontColor, 1, "Press A to Retry, B to Exit");
			GRRLIB_Render();
				
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
			{
				goto INIT_RETRY;
			}
			
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
			{
				GRRLIB_Exit();
				SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
			}
		}
	}
	
	//bool flip = true;
  USB_RETRY:
	if(!Init_Game_List())
	{
		while(1)
		{
			WPAD_ScanPads();
			GRRLIB_DrawImg(115, 95, menu_bg_texture, 0, 1, 1, 0xFFFFFFFF);
			GRRLIB_Printf(190, 140, font_texture, SETTING_fontColor, 1, "USB Error - Drive not found");
			GRRLIB_Printf(190, 160, font_texture, SETTING_fontColor, 1, "Press A to Retry, B to Exit");
			GRRLIB_Render();
				
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
			{
				goto USB_RETRY;
			}
			
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
			{
				GRRLIB_Exit();
				SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
			}
		}
	}
	
	#else
	self.gameCnt = 29;
	Init_Covers();
	#endif
	
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

	BUFFER_InitBuffer(BUFFER_THREAD_COUNT);
	UpdateBufferedImages();
	
	int wait = 300; //ms
	float prog = 2.1/300.0;
	
	sprintf(self.debugMsg, "Initializing Threaded Image Buffer...");
	while(wait > 0)
	{
		wait--;
		self.progress += prog;
		Paint_Progress(self.progress, self.debugMsg);
		Sleep(1);
	}
	
	SETTINGS_Load();	// load user settings from xml file in SD:/usb-loader/

	// set the background
	sprintf(self.debugMsg, "Setting background theme...");
	Paint_Progress(self.progress,self.debugMsg);
	Sleep(300);

	sprintf(self.debugMsg, "Freeing unused textures...");
	Paint_Progress(self.progress,self.debugMsg);
	free(progress_texture.data);
	Sleep(300);
	
	self.selected = false;
	
	bool select_ready = false;
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	
	#ifdef TEST_MODE
	PAD_Init();
	#endif
	
	bool dragging = false;
	
	self.animate_count = 50;
	self.animate_slide_x = 0;
	
	GRRLIB_FillScreen(0x000000FF);
	GRRLIB_Render();

	// gui loop
	while(1) 
	{

		WPAD_ScanPads();
		
		#ifdef TEST_MODE
		PAD_ScanPads();
		#endif
		
		ir_t ir; // The struct for infrared
		
		WPAD_IR(WPAD_CHAN_0, &ir); // Let's get our infrared data
		wd = WPAD_Data(WPAD_CHAN_0);

		pointer.p_x = ir.sx-200;
		pointer.p_y = ir.sy-250;
		pointer.p_ang = ir.angle/2; // Set angle/2 to translate correctly

		//DrawBackground(SETTING_theme);

		// Check for 'HOME' button press
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			SETTINGS_Save();
			
			GRRLIB_FillScreen(0x000000FF);
			GRRLIB_Render();
			
			//BUFFER_ClearCovers();
			BUFFER_KillBuffer();
			Sleep(500);
			quit();
		}
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B || PAD_ButtonsDown(0) & PAD_BUTTON_B)
		{
			if(self.selected && self.animate_flip >= 1.0)
			{
				self.selected = false;
			}
		}
		
		if(dragging) // if the user is dragging the slider
		{
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_A) //with the A button
			{
				DragSlider(pointer.p_x);
			}
			else
			{
				dragging = false; // they let go
			}
		}
		
		// Check for the A button action
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A || PAD_ButtonsDown(0) & PAD_BUTTON_A)
		{
			//First Check if any UI buttons or slider are selected
			if(Button_Select(&addButton, pointer.p_x, pointer.p_y))
			{
				AddGame();
			}
			else if(Button_Select(&settingsButton, pointer.p_x, pointer.p_y))
			{
				Settings_Menu();
			}
			else if(Button_Select(&slideButton, pointer.p_x, pointer.p_y))
			{
				dragging = true;
			}
			else // user clicked A somewhere on the screen
			{
				if(self.gameCnt) // any games?
				{
					if(!self.selected && self.animate_flip <= 0.0)
					{
						// nothing yet selected and nothing animating
						if(pointer.p_x < 400 && pointer.p_x > 200 && pointer.p_y > 60 && pointer.p_y < 380)
						{
							// pointer is in the center cover target area
							if(select_ready && self.select_shift == 0.0)
							{
								// center cover seleted, load the game details
								self.selected = true;
								LoadCurrentCover(self.gameSelected, gameList);
							}
						}
						else if(pointer.p_x < 200 && pointer.p_y > 60 && pointer.p_y < 380)
						{
							// the pointer is within the left set of covers, start to scroll
							self.select_shift = (-4)*((200-pointer.p_x)/200.0); // range is -4 to 0 rouhgly
						}
						else if(pointer.p_x > 400 && pointer.p_y > 60 && pointer.p_y < 380)
						{
							// the pointer is within the right set of covers, start to scroll
							self.select_shift = 5*(pointer.p_x-345.0)/280.0; // range is 1 to 5 roughly
						}
					}
					// Game is selected and finished animating the detail dialog
					if(self.selected && self.animate_flip == 1.0)
					{
						if(Button_Select(&loadButton, pointer.p_x, pointer.p_y))
						{
							// User clicked on the load game, so save settings before launchin
							SETTINGS_Save();
							
							#ifdef ANIMATE_TEST
							if(!LaunchGame())
								return 0;
							#else
							//TODO Prompt to boot game...

							if(!Menu_Boot())
							{
								self.selected = false;
								self.animate_flip = 0;
							}
							else
							{
								return 0;
							}
							#endif
						}
						else if(Button_Select(&deleteButton, pointer.p_x, pointer.p_y))
						{
							// User clicked delete button
							Menu_Delete();
							self.selected = false;
						}
						else if(Button_Select(&backButton, pointer.p_x, pointer.p_y))
						{
							// User clicked back button
							self.selected = false;
						}
					}
				}
			}
		}

		// Check for non-A activity
		// Nothing is selected and nothing is flipped
		if (!self.selected && self.animate_flip == 0)
		{
			// Check for Left pad
			if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT || PAD_ButtonsDown(0) & PAD_BUTTON_LEFT)
			{	
				select_ready = false;
                                if(!((int)self.shift+.5 >= (COVER_COUNT/2.0)))
				{
					self.shift += SCROLL_SPEED; // real Apple like flow direction ;-)
				}
			} // now check for right
			else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT ||	PAD_ButtonsDown(0) & PAD_BUTTON_RIGHT)
			{
				select_ready = false;
                                if(!((int)self.shift-1 <= (-1)*(COVER_COUNT/2.0)))
				{
					self.shift -= SCROLL_SPEED; // real Apple like flow direction ;-)
				}
			} // Check for UP for zoom in
			else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP || PAD_ButtonsDown(0) & PAD_BUTTON_UP)
			{	
				if (SETTING_coverZoom > .69) // seriously, that the number ;-)
				{
					// limit how far we can zoom in
					SETTING_coverZoom = .69;
				}
				else
				{
					// no limit to zooming out, you can go really far out
					SETTING_coverZoom += 0.03;
				}
			} // Check for DOWN for zoom out
			else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN || PAD_ButtonsDown(0) & PAD_BUTTON_DOWN)
			{	
				SETTING_coverZoom -= 0.03;
			}
			else
			{
				if(abs(self.select_shift) > SCROLL_SPEED)
				{
					int mult = abs((int)self.select_shift);
					if(self.select_shift > 0)
					{
						self.select_shift -= mult*SCROLL_SPEED;
						if(!((int)self.shift-1 <= (-1)*(COVER_COUNT/2.0)))
						{
							self.shift -= mult*SCROLL_SPEED;
						}
					}
					else
					{
						self.select_shift += mult*SCROLL_SPEED;
						if(!((int)self.shift+.5 >= (COVER_COUNT/2.0)))
						{
							self.shift += mult*SCROLL_SPEED;
						}
					}
					
				}
				else if(abs(((int)self.shift * 10000.0) - (self.shift*10000.0))/10000.0 > (SCROLL_SPEED+SCROLL_SPEED/2.0))
				{
					select_ready = false;
					if((int)((int)(self.shift+0.5) - (int)self.shift) == 0)
					{
						self.shift -= SCROLL_SPEED;
					}
					else
					{
						self.shift += SCROLL_SPEED;
					}
				}
				else
				{
					self.select_shift = 0;
					self.shift = (int)self.shift;
					select_ready = true;
				
					// Draw Game Title
					if(SETTING_coverText)
					{	
						float t = 1.0; // add a configurable text size later
						draw_game_title(self.gameSelected, t , gameList);
					}
				}
					
			}
		}
	
		UpdateBufferedImages();
		
		// This is the main routine to draw the covers
		draw_covers();

		// Draw the selected game art
		if(self.selected || self.animate_flip != 0)
		{
			#ifndef ANIMATE_TEST
			draw_selected(gameList);
			#else
			draw_selected_two(gameList, false, Button_Hover(&loadButton, pointer.p_x, pointer.p_y));
			#endif
		}
		else
		{
			// Draw the slider and corner buttons
			DrawSlider(SETTING_theme);
			Button_Theme_Paint(&addButton, SETTING_theme);
			Button_Theme_Paint(&settingsButton, SETTING_theme);
		}
		
		// Check for button-pointer intersections, and rumble
		if (Button_Hover(&addButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&settingsButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&slideButton, pointer.p_x, pointer.p_y))
		{
			// Should we be rumbling?
			if (--self.rumbleAmt > 0)
				WPAD_Rumble(0,1); // Turn on Wiimote rumble
			else 
				WPAD_Rumble(0,0); // Kill the rumble
		}
		else
		{ // If no button is being hovered, kill the rumble
			WPAD_Rumble(0,0);
			self.rumbleAmt = 5;
		}
		
	
#ifdef DEBUG
                //display loaded IOS with revision number
                GRRLIB_Printf(50, 20, font_texture, 0xAA0000FF, 1, "IOS%d rev%d", IOS_GetVersion(), IOS_GetRevision());
		// spit the FPS
		GRRLIB_Printf(250, 20, font_texture, 0xAA0000FF, 1, "--DEBUG Build r%d--",SVN_VERSION);
		GRRLIB_Printf(500, 20, font_texture, 0x808080FF, 1, "FPS: %d", FPS);
//		GRRLIB_Printf(500, 40, font_texture, 0x808080FF, 1, "Pointer: %d, %d", ir.sx, ir.sy);
#endif
		// Draw the pointing hand
		GRRLIB_DrawImg(pointer.p_x, pointer.p_y, pointer_texture, pointer.p_ang, 1, 1, 0xFFFFFFFF);
        GRRLIB_Render();

        FPS = CalculateFrameRate();
		
		//Sleep(1);
	}
	
    GRRLIB_Exit(); 
	
	SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	
	return 0;
} 


/**
 * This function calculates the number of frames we render each second.
 * It must be called right after GRRLIB_Render.
 * @return The number of frames per second.
 */
static u8 CalculateFrameRate() {
    static u8 frameCount = 0;
    static u32 lastTime;
    static u8 FPS = 0;
    u32 currentTime = ticks_to_millisecs(gettime());
	
    frameCount++;
    if(currentTime - lastTime > 1000) {
        lastTime = currentTime;
        FPS = frameCount;
        frameCount = 0;
    }
    return FPS;
}

