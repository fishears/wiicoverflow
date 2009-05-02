/*---------------------------------------------------------------------------------

	CoverFlow by Beardface

---------------------------------------------------------------------------------*/

#include "coverflow.h"

//---------------------------------------------

extern const u8		no_cover_png[];
extern const u8		back_cover_png[];
extern const u8		no_disc_png[];
extern const u8     font1_png[];
extern const u8     BMfont5_png[];
extern const u8     loading_main_png[];
extern const u8     progress_png[];
extern const u8     gradient_bg_png[];
extern const u8     slide_bar_png[];
extern const u8     usb_error_png[];
extern const u8     generic_point_png[];
extern const u8     menu_bg_png[];

static char prozent[MAX_CHARACTERS + 16];
static char timet[MAX_CHARACTERS + 16];
	
/* Gamelist buffer */
static struct discHdr *gameList = NULL;

static wbfs_t *hdd = NULL;

/* WBFS device */
static s32 my_wbfsDev = WBFS_DEVICE_USB;

float progress = 0.0;

s_self self;

#ifdef TEST_MODE
COVER_COUNT = 29;
#else
COVER_COUNT = 0;
#endif

float animate_rotate = 0.0;
//float animate_flip = 0.0;
float FLIP_SPEED   = 0.016;

float SCROLL_SPEED = 0.050;

bool firstTimeDownload = true;
bool inetOk = false;
bool imageNotFound = false;

char* _title;
char* _msg;

/*--------------------------------------*/



Mtx GXmodelView2D;
u8 ocarinaChoice = 0;
//int self.array_size = 0;
float p_ang = 0;
WPADData *wd;
GRRLIB_texImg pointer_texture;

void Download_Cover(struct discHdr *header);
int WindowPrompt(char* title, char* txt, struct Button* choice_a, struct Button* choice_b);




#ifdef BUFFER_TEST
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
		
		/*Some logic to avoid drawing everything*/
		if(abs(self.shift+i) < BUFFER_WINDOW)
		{
			//Is this cover already loaded?
			if(!BUFFER_IsCoverReady(index))
			{
				//Is this cover already queued up?
				if(!BUFFER_IsCoverQueued(index))
				{
					//Request this cover
					if(index < self.gameCnt)
					{
						struct discHdr *header = &gameList[index];
		
						BUFFER_RequestCover(index, header);
					}
				}
			}
		}
		else
		{
		//	BUFFER_RemoveCover(index);
		}
	}
	
}


#endif

void quit()
{
	exit(0);
}

void draw_game_title(int index)
{
	if(index != -1)
	{
		float tsize = 1;
		int len = 0;
		struct discHdr *header = NULL;
		
		//WindowPrompt ("Msg 2", "pre1" ,&okButton,&cancelButton);
		header = &gameList[index];
		//WindowPrompt ("Msg 2", header->title ,&okButton,&cancelButton);
		
		len = strlen(header->title);
		
		int offset = (len*5);
		
		if(offset > 240)
			offset = 240;
			
		GRRLIB_Printf(340 - offset, 400, tex_BMfont5, 0xFFFFFFFF, tsize, "%s", header->title);
	}
}

void LoadCurrentCover(int id)
{

	#ifndef TEST_MODE
	void *imgData;// = (void *)no_cover_png;

	char filepath[128];
	s32  ret;

	struct discHdr *header = &gameList[id];
	
//		substr
	
	//sprintf(filepath, USBLOADER_PATH "/disks/%c%c%c.png", header->id[0],header->id[1],header->id[2]);
	sprintf(filepath, USBLOADER_PATH "/disks/%c%c%c%c.png", header->id[0],header->id[1],header->id[2],header->id[3]);

	ret = Fat_ReadFile(filepath, &imgData);
	
	
	if (ret > 0) {
		current_cover_texture = GRRLIB_LoadTexture((const unsigned char*)imgData);
	}
	else
	{
		current_cover_texture = no_disc_texture;
	}
	
	#else
	current_cover_texture = no_disc_texture;
	#endif
	
}

void AddCover(GRRLIB_texImg tex)
{
	if(self.array_size < MAX_COVERS)
	{
		covers[self.array_size] = tex;
		self.array_size++;
	}
	/*
	self.array_size = self.array_size + 1;
	covers = (GRRLIB_texImg*)realloc(covers, (self.array_size * sizeof(GRRLIB_texImg)));

	if (covers == NULL)
	{
	   // fprintf(stdout, "ERROR: Couldn't realloc memory!");
	   return;
	}

    covers[self.array_size - 1] = tex;
	*/
}

void ClearCovers()
{
	#ifndef BUFFER_TEST
	int i;
	for(i = 0; i < self.array_size; i++)
	{
		free(covers[self.array_size].data);
	}
	
	self.array_size = 0;
	#else
	BUFFER_ClearCovers();
	#endif
}

void Init_Covers()
{
	//int i;
	
	ClearCovers();
	
	progress+=0.05;
	Paint_Progress(progress, NULL);
	
	//float max_progress = 1.7;
	
	//float per_game_prog = max_progress/self.gameCnt;
	
	#ifndef TEST_MODE
	
	#ifndef BUFFER_TEST
	int i;
	float max_progress = 1.7;
	float per_game_prog = max_progress/self.gameCnt;
	
	for(i = 0; i < self.gameCnt; i++)
	{
		void *imgData;// = (void *)no_cover_png;

		char filepath[128];
		s32  ret;

		struct discHdr *header = &gameList[i];
		
		if(self.array_size < MAX_COVERS)
		{
			sprintf(self.debugMsg, "Checking next cover...%s", header->id);
			Paint_Progress(1, self.debugMsg);
			Download_Cover(header);
			sprintf(filepath, USBLOADER_PATH "/covers/%s.png", header->id);

			ret = Fat_ReadFile(filepath, &imgData);
			
			
			if (ret > 0) {

				GRRLIB_texImg tmpTex = GRRLIB_LoadTexture((const unsigned char*)imgData);
				
				if ((tmpTex.w > COVER_WIDTH) || (tmpTex.h > COVER_HEIGHT))
				{
					AddCover(cover_texture);
				}
				else
				{
					AddCover(tmpTex);
				}
			}
			else
			{
				AddCover(cover_texture);
			}
		}
		progress+=per_game_prog;
		Paint_Progress(progress, self.debugMsg);
	}
	#endif
	
	#else
	
	int CoverCount = COVER_COUNT;
	
	for(i = 0; i < CoverCount; i++)
	{
		AddCover( GRRLIB_LoadTexture(no_cover_png) );
		progress+=per_game_prog;
		Paint_Progress(progress, NULL);
	}
	
	#endif
}

void draw_selected()
{
	
	if(self.selected && self.animate_flip < 1.0)
	{
		self.animate_flip += FLIP_SPEED;
		if(self.animate_flip > 1.0)
			self.animate_flip = 1.0;
			
		if(self.animate_flip > 0.3 && self.animate_flip < 0.7)
		{
			self.animate_flip = 0.7;
		}
			
	}
	else if(!self.selected)
	{
		self.animate_flip -= FLIP_SPEED;
		
		if(self.animate_flip > 0.3 && self.animate_flip < 0.7)
		{
			self.animate_flip = 0.3;
		}
		
		if(self.animate_flip < 0)
		{
			self.animate_flip = 0;
		}
	}
	
	
	  static const float SPACING = 2.8;
	  float dir = 1;
	  float loc, scale, angle;
	  
	  loc = SPACING * dir * (pow(1, -1) - 1);
	  scale = change_scale(self.animate_flip, 0, 1, 0, 360);
	  angle = -1 * dir * scale;
	
	  if(scale >= 180)
	  {
		//Use back art texture
		GRRLIB_DrawCoverImg(loc*1.2,back_texture,angle,1.4,0xFFFFFFFF);
		
		if(scale >= 360)
		{
			int i ;
			int len;
			animate_rotate++;
			if(animate_rotate == 360) animate_rotate = 0;
			
			GRRLIB_DrawImg(230,100, current_cover_texture, animate_rotate, 1, 1, 0xFFFFFFFF);
			
			Button_Paint(&loadButton);
			Button_Paint(&backButton);
			Button_Paint(&deleteButton);
			
			#ifndef TEST_MODE
			struct discHdr *header = NULL;
			header = &gameList[self.gameSelected];
			f32 size = 0.0;

			/* Get game size */
			WBFS_GameSize(header->id, &size);
			char name[64];
			
			for(i = 0; i < 64; i++)
				name[i] = toupper(header->title[i]);
				
			len = strlen(name);
			float tsize = .8;

			if(len > 20)
				tsize = .6;

			if(len > 28)
				tsize = .4;
				
			if(len > 40)
				tsize = .3;
			
			int offset = (len*10);
			
			if(offset > 240) offset = 240;
			
            GRRLIB_Printf(300 - offset, 10, text_font1, 0XFFFFFFFF, tsize, "%s", name);
            GRRLIB_Printf(210, 50, text_font1, 0XFFFFFFFF, .4, "(%c%c%c%c) (%.2fGB)", header->id[0], header->id[1], header->id[2], header->id[3], size);
			#else
            GRRLIB_Printf(90, 10, text_font1, 0XFFFFFFFF, .8, "%s", "JUSTINS GAME");
            GRRLIB_Printf(180, 50, text_font1, 0XFFFFFFFF, .5, "%s", "JUSTINS GAME");
			#endif
		}
	  }
	  else
	  {
	    #ifdef BUFFER_TEST
		DrawBufferedCover(self.gameSelected, loc, angle);
		#else
	    if(self.gameSelected < self.array_size)
		{
			GRRLIB_DrawCoverImg(loc*1.2,covers[self.gameSelected],angle,1.0,0xFFFFFFFF);
		}
		else
		{
			GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF);
		}
		#endif
	  }
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

	progress+=0.05;
	Paint_Progress(progress, NULL);
	
	/* Get header list */
	ret = WBFS_GetHeaders(buffer, cnt, sizeof(struct discHdr));
	if (ret < 0)
		goto err;

	progress+=0.05;
	Paint_Progress(progress, NULL);
	
	/* Sort entries */
	qsort(buffer, cnt, sizeof(struct discHdr), __Menu_EntryCmp);

	progress+=0.05;
	Paint_Progress(progress, NULL);
	
	/* Free memory */
	if (gameList)
		free(gameList);

	/* Set values */
	gameList = buffer;
	self.gameCnt  = cnt;
	COVER_COUNT = self.gameCnt;
	
	Init_Covers();

	progress+=0.05;
	Paint_Progress(progress, NULL);
	
	/* Reset variables */
	self.gameSelected = self.gameStart = 0;

	return 0;

err:
	/* Free memory */
	if (buffer)
		free(buffer);

	return ret;
}

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
	
	progress+=0.05;
	Paint_Progress(progress, NULL);
	

	/* Initialize DIP module */
	ret = Disc_Init();
	progress+=0.05;
	Paint_Progress(progress, NULL);
	
	if (ret < 0) {
		printf("[+] ERROR:\n");
		printf("    Could not initialize DIP module! (ret = %d)\n", ret);

		return false;
	}

	return true;
}

bool Init_Game_List(void)
{

	Paint_Progress(progress, NULL);
	
	/* Try to open device */
	if (WBFS_Open() >= 0) {
		/* Get game list */
		
		progress+=0.05;
		Paint_Progress(progress, NULL);
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
		
	self.shift = change_scale(xPos, min_loc, max_loc, -1*(COVER_COUNT/2.0), COVER_COUNT/2.0);
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

int WindowPrompt(char* title, char* txt, struct Button* choice_a, struct Button* choice_b)
{
	/*TODO Create Graphical Prompt*/
		
	bool doloop = true;
	
	if(choice_a == 0 && choice_b == 0)
	{
		doloop = false;
	}
	else
	{
		doloop = true;
		GRRLIB_FillScreen(0x000000FF);
		GRRLIB_Render();
	}
	
	
	do{

		WPAD_ScanPads();
		
		ir_t ir; // The struct for infrared
		
		WPAD_IR(WPAD_CHAN_0, &ir); // Let's get our infrared data
		wd = WPAD_Data(WPAD_CHAN_0);

		self.p_x = ir.sx-200;
		self.p_y = ir.sy-250;
		p_ang = ir.angle/2; // Set angle/2 to translate correctly

		Hover_Buttons();

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			quit();
		}
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
		{
			return false;
		}
		
		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
		{
			if(choice_a != 0)
			{
				if(Button_Select(choice_a, self.p_x, self.p_y))
				{
					return true;
				}
			}
			
			if(choice_b != 0)
			{
				if(Button_Select(choice_b, self.p_x, self.p_y))
				{
					return false;
				}
			}
		}
		
		Hover_Buttons();
		
		GRRLIB_FillScreen(0x000000FF);
		//GRRLIB_DrawImg(0, 0,    gradient_texture, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_DrawImg(120, 60, menu_bg_texture, 0, 1, 1, 0xFFFFFFFF);
		
		if(choice_a != 0)
		{
			Button_Paint(choice_a);
		}
		
		if(choice_b != 0)
		{
			Button_Paint(choice_b);
		}
		
		/*Draw Text*/
        GRRLIB_Printf(140, 70, tex_BMfont5,  0xFFFFFFFF, 1.5, "%s", title);
        GRRLIB_Printf(160, 110, tex_BMfont5, 0xFFFFFFFF, 1, "%s", txt);
		
		if(doloop)
			GRRLIB_DrawImg(self.p_x, self.p_y, pointer_texture, p_ang, 1, 1, 0xFFFFFFFF);
		
		GRRLIB_Render();
		
	}while(doloop);
	
	return false;
}
/****************************************************************************
 * ShowProgress
 *
 * Updates the variables used by the progress window for drawing a progress
 * bar. Also resumes the progress window thread if it is suspended.
 ***************************************************************************/
void
ShowProgress (s32 done, s32 total)
{

    static time_t start;
	static u32 expected;

    f32 percent; //, size;
	u32 d, h, m, s;

	//first time
	if (!done) {
		start    = time(0);
		expected = 300;
	}

	//Elapsed time
	d = time(0) - start;

	if (done != total) {
		//Expected time
		if (d)
			expected = (expected * 3 + d * total / done) / 4;

		//Remaining time
		d = (expected > d) ? (expected - d) : 0;
	}

	//Calculate time values
	h =  d / 3600;
	m = (d / 60) % 60;
	s =  d % 60;

	//Calculate percentage/size
	percent = (done * 100.0) / total;

	sprintf(prozent, "%s%0.2f%%", "Installing Game...", percent);

    sprintf(timet,"Time left: %d:%02d:%02d",h,m,s);

	/*Update and Draw Progress Window Here*/
	WindowPrompt(prozent, timet, 0, 0);
}


int ProgressWindow(char* title, char* msg)
{
	/*TODO Draw Window*/
	_title = title;
	_msg   = msg;
	
	int ret = wbfs_add_disc(hdd, __WBFS_ReadDVD, NULL, ShowProgress, ONLY_GAME_PARTITION, 0);
	
	progress = 0.0;
	
	return ret;

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

		self.p_x = ir.sx-200;
		self.p_y = ir.sy-250;
		p_ang = ir.angle/2; // Set angle/2 to translate correctly

		Hover_Buttons();

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			doloop = false;
		}
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
		{
			doloop = false;
		}
		
		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
		{
			if(Button_Select(&settingsButton, self.p_x, self.p_y))
			{
				doloop = false;
			}
			else if(Button_Select(&cheatonButton, self.p_x, self.p_y) || Button_Select(&cheatoffButton, self.p_x, self.p_y))
			{
				ocarinaChoice = (ocarinaChoice) ? 0 : 1;
			}
		}
		
		Hover_Buttons();
		
		GRRLIB_FillScreen(0x000000FF);
		//GRRLIB_DrawImg(0, 0,    gradient_texture, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_DrawImg(120, 60, menu_bg_texture, 0, 1, 1, 0xFFFFFFFF);
		
        GRRLIB_Printf(190, 63, tex_BMfont5, 0xFFFFFFFF, 1, "Coverflow Settings");
		
		Button_Paint(&settingsButton);
			
        GRRLIB_Printf(145, 103, tex_BMfont5, 0xFFFFFFFF, 1, "Ocarina");
		/*Draw Menu*/
		if (ocarinaChoice)
		{
			Button_Paint(&cheatonButton);
		}
		else Button_Paint(&cheatoffButton);
		
		GRRLIB_DrawImg(self.p_x, self.p_y, pointer_texture, p_ang, 1, 1, 0xFFFFFFFF);
		
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
			ret = ProgressWindow(gametxt, name);
			if (ret != 0) {
				WindowPrompt ("Install error!",0,&cancelButton,0);
				return false;
			} else {
				GetEntries();
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
			GetEntries();
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
	s32 ret;

	/* No game list */
	if (!self.gameCnt)
		return false;

	/* Selected game */
	header = &gameList[self.gameSelected];

    GRRLIB_Exit();
	
	#ifndef BUFFER_TEST
	int i = 0;
	for(i = 0; i < self.array_size; i++)
	{
		free(covers[i].data);
	}
	#endif
	
	free(cover_texture.data);
	free(back_texture.data);
	free(empty_texture.data);
	free(no_disc_texture.data);
	free(current_cover_texture.data);
	free(text_font1.data);

	BUFFER_ClearCovers();
	BUFFER_KillBuffer();
	Sleep(300);
	
	/* Set WBFS mode */
	Disc_SetWBFS(WBFS_DEVICE_USB,header->id);
		
	/* Open disc */
	ret = Disc_Open();
	if (ret < 0) {
		return false;
	}

	ret = Disc_WiiBoot();
    if (ret < 0) {
        SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
    }

	#endif
	
	return true;
}


int Net_Init(char *ip){
	
	s32 res;
    while ((res = net_init()) == -EAGAIN)
	{
		usleep(100 * 1000); //100ms
	}
	
    if (if_config(ip, NULL, NULL, true) < 0) {
		printf("      Error reading IP address, exiting");
		usleep(1000 * 1000 * 1); //1 sec
		return false;
	}
	return true;
}

void saveFile(char* imgPath, struct block file){
	//printf("\n\n    Size: %d byte", file.size);
			
	/* save png to sd card for future use*/
			
	FILE *f;
	f = fopen(imgPath, "wb");
	if (f)
	{
		fwrite(file.data,1,file.size,f);
		fclose (f);
	}
}

void Download_Cover(struct discHdr *header)
{
	char imgPath[100];

	if (!header)
		return;
		
	//the first time no image is found, attempt to init network
	/* Initialize Network <<<TO BE THREADED>>> */
	if(firstTimeDownload == true){
		char myIP[16];
		//printf("\n[+] Initializing Network.");
		sprintf(self.debugMsg, "Initializing Network");
		Paint_Progress(1, self.debugMsg);
		//GRRLIB_Printf(100, 100, text_font1, 0XFFFFFFFF, 1.0, "Initializing Network");
		if( Net_Init(myIP) ){
			//GRRLIB_Printf(100, 120, tex_BMfont5, 0XFFFFFFFF, 1.0, "Error Initializing Network");
			sprintf(self.debugMsg, "Error Initializing Network");
			Paint_Progress(1, self.debugMsg);
			inetOk = true;
		}
		else{
			sprintf(self.debugMsg, "Network Initialized");
			Paint_Progress(1, self.debugMsg);
		}

		firstTimeDownload = false;
	}
	
	/*
	GRRLIB_Render();
	usleep(50000);
	GRRLIB_Exit(); 
    free(tex_BMfont5.data);
	exit (0);
	*/
		
	if(inetOk) {
		//printf("\n    Network connection established.");
		/*try to download image */
			
		char url[100];
		struct block file;
	
		char region[4];
		switch(header->id[3]){
	
		case 'E':
			sprintf(region,"ntsc");
			break;

		case 'J':
			sprintf(region,"ntscj");
			break;

		case 'P':
			sprintf(region,"pal");
			break;
		}
		//printf("\n    Downloading cover...");

		snprintf(imgPath, sizeof(imgPath), "%s/covers/%s.png", CFG.images_path, header->id);
		
		sprintf(self.debugMsg, "Checking presence of %s", imgPath);
		Paint_Progress(1,self.debugMsg);
		
		FILE *fp;
		fp = fopen(imgPath, "rb");
		if (fp)
		{
			fclose (fp);
			sprintf(self.debugMsg, "%s present, not downloading", imgPath);
			Paint_Progress(1,self.debugMsg);
		}
		else{
			
			/*
			if (CFG.widescreen)
				sprintf(url, "http://www.theotherzone.com/wii/widescreen/%s/%s.png", region, header->id);
			else
				sprintf(url, "http://www.theotherzone.com/wii/%s/%s.png", region, header->id);
			*/
			
			sprintf(url, "http://www.theotherzone.com/wii/resize/%s/160/224/%s.png", region, header->id);
			sprintf(self.debugMsg, "Getting %s", url);
			Paint_Progress(1,self.debugMsg);
		
			file = downloadfile(url);
			
			if(file.data != NULL){
				saveFile(imgPath, file);
				free(file.data);
				sprintf(self.debugMsg, "done");
			    Paint_Progress(1,self.debugMsg);
				//else
					//donotdownload = true;
			}
			else {
				sprintf(self.debugMsg, "some error occurred");
				Paint_Progress(1,self.debugMsg);
			}
		}
		
		snprintf(imgPath, sizeof(imgPath), "%s/disks/%c%c%c%c.png", CFG.images_path,  header->id[0], header->id[1], header->id[2], header->id[3]);
		sprintf(self.debugMsg, "Checking presence of %s", imgPath);
		Paint_Progress(1,self.debugMsg);
		
		fp = fopen(imgPath, "rb");
		if (fp)
		{
			fclose (fp);
			sprintf(self.debugMsg, "%s present, not downloading", imgPath);
			Paint_Progress(1,self.debugMsg);
		}
		else{
			sprintf(url, "http://www.theotherzone.com/wii/diskart/160/160/%c%c%c%c.png", header->id[0], header->id[1], header->id[2], header->id[3]);
			sprintf(self.debugMsg, "Getting %s", url);
			Paint_Progress(1,self.debugMsg);
			
			file = downloadfile(url);
			
			if(file.data != NULL){
				saveFile(imgPath, file);
				free(file.data);
				sprintf(self.debugMsg, "done");
			    Paint_Progress(1,self.debugMsg);
				//else
					//donotdownload = true;
			}
			else {
				sprintf(self.debugMsg, "some error occurred");
				Paint_Progress(1,self.debugMsg);
			}
		//else
			//donotdownload = true;
		}
	}
	//refresh = true;				
} /* end download */

void LoadTextures()
{
	usb_error_texture = GRRLIB_LoadTexture(usb_error_png);
	pointer_texture   = GRRLIB_LoadTexture(generic_point_png);
	menu_bg_texture   = GRRLIB_LoadTexture(menu_bg_png);
	
	cover_texture = GRRLIB_LoadTexture(no_cover_png);
	back_texture = GRRLIB_LoadTexture(back_cover_png);
        no_disc_texture = GRRLIB_LoadTexture(no_disc_png);
	text_font1 = GRRLIB_LoadTexture(font1_png);
	
	slide_bar_texture = GRRLIB_LoadTexture(slide_bar_png);
	
    GRRLIB_InitTileSet(&helvetica, 22, 40, 0);
    GRRLIB_InitTileSet(&text_font1, 32, 36, 32);
	
    tex_BMfont5 = GRRLIB_LoadTexture(BMfont5_png);
    GRRLIB_InitTileSet(&tex_BMfont5, 8, 16, 0);
}

void initVars(){

	self.p_x = 0;
	self.p_y = 0;
	self.shift = 0;
	self.select_shift = 0;
	self.gameCnt = 0;
	self.gameSelected = 0;
	self.gameStart = 0;
	self.selected = false;
	self.animate_flip = 0.0;
	self.array_size = 0;
}

//---------------------------------------------------------------------------------
int main( int argc, char **argv ){
//---------------------------------------------------------------------------------
	#ifndef TEST_MODE
	//__Disc_SetLowMem();
	/* Load Custom IOS */
	int ret = IOS_ReloadIOS(249);
	/* Check if Custom IOS is loaded */
	if (ret < 0) {
		printf("[+] ERROR:\n");
		printf("    Custom IOS could not be loaded! (ret = %d)\n", ret);

		return 0;
	}
	#endif
		
	GRRLIB_Init();
    GRRLIB_FillScreen(0x000000FF);
    GRRLIB_Render();
    GRRLIB_FillScreen(0x000000FF);
    GRRLIB_Render();
	
	initVars();
	
    gradient_texture    = GRRLIB_LoadTexture(gradient_bg_png);
    loader_main_texture = GRRLIB_LoadTexture(loading_main_png);
    progress_texture    = GRRLIB_LoadTexture(progress_png);
	
	sprintf(self.debugMsg, "Loading textures");
	Paint_Progress(progress,self.debugMsg);
	
	LoadTextures();
	
	progress += .1;
	sprintf(self.debugMsg, "Init USB");
	Paint_Progress(progress,self.debugMsg);
	
	#ifndef TEST_MODE
	if(!init_usbfs())
		return 0;
		
	//LOAD CONFIG
	strcpy(CFG.images_path, USBLOADER_PATH);
	CFG.widescreen = 0;
	CFG.download = 1;
	//HARDCODED FOR NOW
	
	sprintf(self.debugMsg, "Initializing WBFS");
	Paint_Progress(progress,self.debugMsg);
	
	my_wbfsDev = WBFS_DEVICE_USB;

  INIT_RETRY:
	/* Initialize WBFS */
	ret = WBFS_Init(my_wbfsDev);
	
	if(ret < 0)
	{
		while(1)
		{
			WPAD_ScanPads();
		
			GRRLIB_DrawImg(0, 0, usb_error_texture, 0, 1, 1, 0xFFFFFFFF);
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
		
			GRRLIB_DrawImg(0, 0, usb_error_texture, 0, 1, 1, 0xFFFFFFFF);
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
	Init_Buttons();
	
	#ifdef BUFFER_TEST
	BUFFER_InitBuffer(BUFFER_THREAD_COUNT);
	UpdateBufferedImages();
	
	int wait = 300; //ms
	float prog = 2.1/300.0;
	
	sprintf(self.debugMsg, "Initializing Threaded Image Buffer...");
	while(wait > 0)
	{
		wait--;
		progress += prog;
		Paint_Progress(progress, self.debugMsg);
		Sleep(1);
	}
	
	#endif
	
	sprintf(self.debugMsg, "Freeing unused textures");
	Paint_Progress(progress,self.debugMsg);
	
	free(gradient_texture.data);
	free(loader_main_texture.data);
	free(progress_texture.data);
	free(usb_error_texture.data);
	
	Sleep(300);
	
	self.selected = false;
	
	bool select_ready = false;
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	
	#ifdef TEST_MODE
	PAD_Init();
	#endif
	
	bool dragging = false;
	
	while(1) {

		WPAD_ScanPads();
		
		#ifdef TEST_MODE
		PAD_ScanPads();
		#endif
		
		ir_t ir; // The struct for infrared
		
		WPAD_IR(WPAD_CHAN_0, &ir); // Let's get our infrared data
		wd = WPAD_Data(WPAD_CHAN_0);

		self.p_x = ir.sx-200;
		self.p_y = ir.sy-250;
		p_ang = ir.angle/2; // Set angle/2 to translate correctly

		Hover_Buttons();
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			BUFFER_ClearCovers();
			BUFFER_KillBuffer();
			Sleep(300);
			quit();
		}
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B ||
			PAD_ButtonsDown(0) & PAD_BUTTON_B)
		{
			if(self.selected && self.animate_flip >= 1.0)
			{
				self.selected = false;
			}
		}
		
		if(dragging)
		{
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_A)
			{
				DragSlider(self.p_x);
			}
			else
			{
				dragging = false;
			}
		}
				
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A ||
			PAD_ButtonsDown(0) & PAD_BUTTON_A)
		{
			//First Check Buttons
			if(Button_Select(&addButton, self.p_x, self.p_y))
			{
				AddGame();
			}
			if(Button_Select(&settingsButton, self.p_x, self.p_y))
			{
				Settings_Menu();
			}
			else if(Button_Select(&slideButton, self.p_x, self.p_y))
			{
				dragging = true;
			}
			else
			{
				if(self.gameCnt)
				{
					if(!self.selected && self.animate_flip <= 0.0)
					{
						if(self.p_x < 400 && self.p_x > 200 &&
							self.p_y > 60 && self.p_y < 380)
						{
							if(select_ready && self.select_shift == 0.0)
							{
								self.selected = true;
								LoadCurrentCover(self.gameSelected);
							}
						}
						else if(self.p_x < 200 &&
							self.p_y > 60 && self.p_y < 380)
						{
							self.select_shift = (-4)*((200-self.p_x)/200.0);
						}
						else if(self.p_x > 400 &&
							self.p_y > 60 && self.p_y < 380)
						{
							self.select_shift = 5*(self.p_x-345.0)/280.0;
						}
					}
					
					if(self.selected && self.animate_flip == 1.0)
					{
						if(Button_Select(&loadButton, self.p_x, self.p_y))
						{
							#ifdef BUFFER_TEST
							//BUFFER_ClearCovers();
							//BUFFER_KillBuffer();
							#endif
							
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
						}
						else if(Button_Select(&deleteButton, self.p_x, self.p_y))
						{
							Menu_Delete();
							self.selected = false;
						}
						else if(Button_Select(&backButton, self.p_x, self.p_y))
						{
							self.selected = false;
						}
					}
				}
			}
		}

		if(!self.selected && self.animate_flip == 0)
		{
			if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT  ||
				PAD_ButtonsDown(0) & PAD_BUTTON_LEFT)
			
			{	
				select_ready = false;
					
				if(!((int)self.shift-1 <= (-1)*(COVER_COUNT/2.0)))
					self.shift -= SCROLL_SPEED;
			}
			else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT ||
				PAD_ButtonsDown(0) & PAD_BUTTON_RIGHT)
			{
				select_ready = false;
					
				if(!((int)self.shift+.5 >= (COVER_COUNT/2.0)))
					self.shift += SCROLL_SPEED;
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
							self.shift -= mult*SCROLL_SPEED;
					}
					else
					{
						self.select_shift += mult*SCROLL_SPEED;
						if(!((int)self.shift+.5 >= (COVER_COUNT/2.0)))
							self.shift += mult*SCROLL_SPEED;
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
				
					//CRASH!
					/*Draw Game Title*/
					draw_game_title(self.gameSelected);
				}
					
			}
		}
		
		#ifdef BUFFER_TEST
		UpdateBufferedImages();
		#endif
		
		draw_covers();


		if(self.selected || self.animate_flip != 0)
		{
			draw_selected();
		}
		else
		{
			DrawSlider();
			Button_Paint(&addButton);
			Button_Paint(&settingsButton);
		}

		GRRLIB_DrawImg(self.p_x, self.p_y, pointer_texture, p_ang, 1, 1, 0xFFFFFFFF);
        GRRLIB_Render();

		Sleep(1);
	}
	
	//Preview
	
    GRRLIB_Exit(); 
	
	SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	
	return 0;
} 
