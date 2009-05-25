#include "gfx.h"
#include "settings.h"
#include "localization.h"

extern s_self self;
extern s_pointer pointer;
extern s_settings settings;
extern s_gameSettings gameSetting;
extern s_title* titleList;

extern int COVER_COUNT;
// Language selection config
char glanguages[11][22] =
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
char gvidmodes[6][22] =
{{ "  Game Default" },
{ "   Automatic", },
{ "  Force PAL50", },
{ "  Force PAL60", },
{ "  Force NTSC", },
{ "Console Default"}};
//hook types for ocarina
char ghooks[3][9] =
{{"   VI"},
{" Wii Pad"},
{" GC Pad"}};

static char timet[256];

void LoadTextures()
{
	pointer_texture        = GRRLIB_LoadTexture(generic_point_png);
	pointer_shadow_texture = GRRLIB_LoadTexture(pointer_shadow_png);
	turn_point_texture     = GRRLIB_LoadTexture(turning_point_png);
	menu_bg_texture		   = GRRLIB_LoadTexture(menu_bg_png);
	cover_texture		   = GRRLIB_LoadTexture(no_cover_png);
	back_texture		   = GRRLIB_LoadTexture(back_cover_png);
	no_disc_texture		   = GRRLIB_LoadTexture(no_disc_png);
	load_bg_texture		   = GRRLIB_LoadTexture(bg_options_screen_no_transparency_png);
	font_texture           = GRRLIB_LoadTexture(BMfont5_png);
	font_title             = GRRLIB_LoadTexture(font_w14_h20_png);
	font_title_small       = GRRLIB_LoadTexture(font_w10_h14_png);
	slidebar_texture       = GRRLIB_LoadTexture(slidebar_png);
	slidebar_white_texture = GRRLIB_CreateEmptyTexture(slidebar_texture.w, slidebar_texture.h);
	GRRLIB_BMFX_Invert(slidebar_texture, slidebar_white_texture); //invert the slider black to white
	ambientlight_texture   = GRRLIB_LoadTexture(ambientlight_png);
	ambientlight_white_texture = GRRLIB_CreateEmptyTexture(ambientlight_texture.w, ambientlight_texture.h);
	GRRLIB_BMFX_Invert(ambientlight_texture, ambientlight_white_texture); //invert the fade from black to white
	battery_bar            = GRRLIB_LoadTexture(battery_bar_png);
    battery                = GRRLIB_LoadTexture(battery_png);
    battery_dead           = GRRLIB_LoadTexture(battery_dead_png);
	ttf_button_black       = GRRLIB_LoadTexture(ttf_button_black_png);
	ttf_button_white       = GRRLIB_LoadTexture(ttf_button_white_png);

	GRRLIB_InitTileSet(&font_texture, 8, 16, 0);
	GRRLIB_InitTileSet(&font_title, 14, 20, 32);
	GRRLIB_InitTileSet(&font_title_small, 10, 14, 32);
}

void DrawBufferedCover(int i, float loc, float angle, float falloff)
{
	if(i < MAX_BUFFERED_COVERS || i >= 0)
	{
		if(BUFFER_IsCoverReady(i))
		{
			pthread_mutex_lock(&buffer_mutex[i]);
			if(_texture_data[i].data)
			{
				GRRLIB_DrawCoverImg(loc*1.2,_texture_data[i],angle,1.0,0xFFFFFFFF, falloff, settings.theme);
			}
			else
			{
				GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
			}
			pthread_mutex_unlock(&buffer_mutex[i]);
		}
		else
		{
			GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
		}	
	}
	else
	{
		GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
	}	

}

void Paint_Progress(float v, char* msg)
{
	int i;
	int count = (int)(v*10);
	
	if(count > 26)
		count = 26;

	GRRLIB_2D_Init();

	GRRLIB_DrawImg(0, 0, loader_main_texture, 0, 1, 1, 0xFFFFFFFF);

	for(i = 0; i < count; i++)
	{
		GRRLIB_DrawImg(165+12*i, 231, progress_texture, 0, 1, 1, 0xFFFFFFFF);
	}
	
#ifdef DEBUG
	if(msg != NULL)
		GRRLIB_Printf(140, 255, font_title_small,  0x444444FF, 1, "%s", msg);
#endif
    
	GRRLIB_Render();
}

void Paint_Progress_Generic(int v, int max, char* msg)
{
	float percent = (float)v/(float)max;
	int count = percent*26;
        if(count > 26)
                count = 26;
	int i;

	GRRLIB_2D_Init();
	
	GRRLIB_DrawImg(0, 0, loader_main_texture, 0, 1, 1, 0xFFFFFFFF);
	
	for(i = 0; i < count; i++)
	{
		GRRLIB_DrawImg(165+12*i, 231, progress_texture, 0, 1, 1, 0xFFFFFFFF);
	}
	
	#ifdef DEBUG
	if(msg != NULL)
		GRRLIB_Printf(140, 255, font_title_small,  0x444444FF, 1, "%s", msg);
    #endif
    
	GRRLIB_Render();
}

void Init_Buttons()
{
#ifndef 	LOCALBUTTON_OFF
    char labelBuf[15]; //add for Localization
#endif	
	addButton				= Button_Init(add_button_png, add_button_hover_png, 580, 417);
    slideButton				= Button_Init(slide_png, slide_hover_png, 260, 426);
    okButton				= Button_Init(ok_png, ok_hover_png, 220, 290);
	cancelButton			= Button_Init(cancel_png, cancel_hover_png, 360, 290);
	loadButton				= Button_Init(load_png, load_hover_png, 220, 300);
	deleteButton			= Button_Init(delete_png, delete_hover_png, 220, 400);
    resetButton				= Button_Init(reset_png, reset_hover_png, 350, 330);
    backButton				= Button_Init(back_png, back_hover_png, 340, 300);
    gbackButton				= Button_Init(back_png, back_hover_png, 340, 300);
    gsettingsButton			= Button_Init(settings_png, settings_hover_png, 30, 420);
    cheatonButton			= Button_Init(toggle_on_png, toggle_on_png, 215,85);
    cheatoffButton			= Button_Init(toggle_off_png, toggle_off_png, 215,85);
    langupButton			= Button_Init(plus_button_png, plus_button_hover_png,456,123);
    langdownButton			= Button_Init(minus_button_png, minus_button_hover_png, 300,123);
    vidupButton				= Button_Init(plus_button_png, plus_button_hover_png, 456,150);
    viddownButton			= Button_Init(minus_button_png, minus_button_hover_png, 300,150);
    vidtvonButton			= Button_Init(toggle_on_png, toggle_on_png, 350, 175);
    vidtvoffButton			= Button_Init(toggle_off_png, toggle_off_png, 350, 175);
    hookupButton			= Button_Init(plus_button_png, plus_button_hover_png, 456,87);
    hookdownButton			= Button_Init(minus_button_png, minus_button_hover_png, 360,87);
    gcheatonButton          = Button_Init(toggle_on_png, toggle_on_png, 239,185); //
    gcheatoffButton         = Button_Init(toggle_off_png, toggle_off_png, 239,185); //
    glangupButton           = Button_Init(plus_button_png, plus_button_hover_png,480,223);
    glangdownButton         = Button_Init(minus_button_png, minus_button_hover_png, 324,223);
    gvidupButton            = Button_Init(plus_button_png, plus_button_hover_png, 480,250);
    gviddownButton          = Button_Init(minus_button_png, minus_button_hover_png, 324,250);
    gvidtvonButton          = Button_Init(toggle_on_png, toggle_on_png, 374, 275);
    gvidtvoffButton         = Button_Init(toggle_off_png, toggle_off_png, 374, 275);
    ghookupButton           = Button_Init(plus_button_png, plus_button_hover_png, 480,187);
    ghookdownButton         = Button_Init(minus_button_png, minus_button_hover_png, 384,187);
    coverTextOnButton       = Button_Init(toggle_on_png, toggle_on_png, 350, 330);
    coverTextOffButton      = Button_Init(toggle_off_png, toggle_off_png, 350, 330);
    graphicsButton          = Button_Init(ok_png, ok_hover_png, 350, 215);
    yesButton               = Button_Init(yes_png, yes_hover_png, 220, 290);
    noButton                = Button_Init(no_png, no_hover_png, 340, 290);
    settingsButton		    = Button_Init(settings_png, settings_hover_png, 30, 420);
    settingsButton	    	= Button_Init(settings_png, settings_hover_png, 30, 420);
    titlesButton		    = Button_Init(titles_png, titles_hover_png, 400, 250);
	coversButton			= Button_Init(covers_png, covers_hover_png, 310, 250);
    zoomupButton            = Button_Init(plus_button_png, plus_button_hover_png,456,95);
    zoomdownButton          = Button_Init(minus_button_png, minus_button_hover_png, 300,95);
    spacingupButton         = Button_Init(plus_button_png, plus_button_hover_png,456,133);
    spacingdownButton       = Button_Init(minus_button_png, minus_button_hover_png, 300,133);
    angleupButton           = Button_Init(plus_button_png, plus_button_hover_png,456,171);
    angledownButton         = Button_Init(minus_button_png, minus_button_hover_png, 300,171);
    falloffupButton         = Button_Init(plus_button_png, plus_button_hover_png,456,209);
    falloffdownButton       = Button_Init(minus_button_png, minus_button_hover_png, 300,209);
    windowupButton          = Button_Init(plus_button_png, plus_button_hover_png,456, 247);
    windowdownButton        = Button_Init(minus_button_png, minus_button_hover_png, 300,247);
    themeWhiteButton        = Button_Init(theme_white_png, theme_white_png, 350, 295);
    themeBlackButton        = Button_Init(theme_black_png, theme_black_png, 350, 295);
    quickstartOnButton      = Button_Init(toggle_on_png, toggle_on_png, 350, 335);
    quickstartOffButton     = Button_Init(toggle_off_png, toggle_off_png, 350, 335);
    rumbleOnButton          = Button_Init(toggle_on_png, toggle_on_png, 350, 375);
    rumbleOffButton         = Button_Init(toggle_off_png, toggle_off_png, 350, 375);
    musicOnButton           = Button_Init(toggle_on_png, toggle_on_png, 350, 415);
    musicOffButton          = Button_Init(toggle_off_png, toggle_off_png, 350, 415);
    bookmarkOnButton        = Button_Init(star_on_png, star_on_png, 515, 140);
    bookmarkOffButton       = Button_Init(star_off_png, star_on_png, 515, 140);
    homeMenuTopButton       = Button_Init(homeTop_png, homeTop_hover_png, 0, 0);
    homeMenuBottomButton    = Button_Init(homeBottom_png, homeBottom_hover_png, 0, 368);
    wiiMenuButton           = Button_Init(wiiMenu_png, wiiMenu_hover_png, 34, 180);
    loaderButton            = Button_Init(loader_png, loader_hover_png, 174, 180);
    wiimoteButton           = Button_Init(wiimote_png, wiimote_png, 54, 400);

#ifndef 	LOCALBUTTON_OFF
	//add for Localization
	sprintf(addButton.label, localStr("B01","")); 
    sprintf(deleteButton.label, localStr("B02","")); 
    sprintf(cancelButton.label, localStr("B03",""));
	sprintf(yesButton.label, localStr("B04","")); 
    sprintf(noButton.label, localStr("B05","")); 
	sprintf(labelBuf, localStr("B06",""));
    sprintf(okButton.label, labelBuf); 
	sprintf(graphicsButton.label, labelBuf);
    sprintf(resetButton.label, localStr("B07","")); 
    //sprintf(downloadButton.label, localStr("B08","")); 
	sprintf(coversButton.label, localStr("B08","")); 
	sprintf(titlesButton.label, localStr("B08","")); 
    sprintf(loadButton.label, localStr("B09","")); 
    sprintf(labelBuf, localStr("B10",""));
	sprintf(backButton.label, labelBuf); 
	sprintf(gbackButton.label, labelBuf); 

	//sprintf(labelBuf, "An"); //for testing
	sprintf(labelBuf, localStr("B11",""));
	sprintf(cheatonButton.label, labelBuf); 
	sprintf(vidtvonButton.label, labelBuf); 
	sprintf(gcheatonButton.label, labelBuf); 
	sprintf(gvidtvonButton.label, labelBuf); 
	sprintf(coverTextOnButton.label, labelBuf); 
	sprintf(quickstartOnButton.label, labelBuf); 
	sprintf(rumbleOnButton.label, labelBuf); 
	sprintf(musicOnButton.label, labelBuf); 

	//sprintf(labelBuf, "      Aus");  //for testing
	sprintf(labelBuf, localStr("B12",""));
	sprintf(cheatoffButton.label, labelBuf); 
	sprintf(vidtvoffButton.label, labelBuf); 
	sprintf(gcheatoffButton.label, labelBuf); 
	sprintf(gvidtvoffButton.label, labelBuf); 
	sprintf(coverTextOffButton.label, labelBuf); 
	sprintf(quickstartOffButton.label, labelBuf); 
	sprintf(rumbleOffButton.label, labelBuf); 
	sprintf(musicOffButton.label, labelBuf); 
	sprintf(themeWhiteButton.label, localStr("B13",""));
	sprintf(themeBlackButton.label, localStr("B14",""));
	sprintf(homeMenuTopButton.label, localStr("B15",""));
	sprintf(wiiMenuButton.label, localStr("B16",""));
	sprintf(loaderButton.label, localStr("B17",""));
	
#endif
}

void DrawSlider(int theme_id)
{
	int min_loc = 20;
	int max_loc = 296;
	int x = change_scale(self.shift, self.min_cover, self.max_cover, min_loc, max_loc);
	
	if (self.gameCnt < 2)
		x = 155;
	
	slideButton.x = 446 - x;
	slideButton.y = 426;
	
	switch (theme_id)
	{
		case 0: // black theme
			GRRLIB_DrawImg(120, 410, slidebar_texture, 0, 1, 1, 0xFFFFFFFF);
			Button_Theme_Paint(&slideButton, settings.theme);
			break;
		case 1: // white theme
			GRRLIB_DrawImg(120, 410, slidebar_white_texture, 0, 1, 1, 0xFFFFFFFF);
			Button_Theme_Paint(&slideButton, settings.theme);
			break;
		default:
			GRRLIB_DrawImg(120, 410, slidebar_texture, 0, 1, 1, 0xFFFFFFFF);
			Button_Theme_Paint(&slideButton, settings.theme);
			break;
	}
}

void GRRLIB_Cover(float pos, int texture_id)
{
	if(texture_id >= self.gameCnt)
		return;

	if(pos == 0)
		self.gameSelected = texture_id;
	
	if((self.selected || self.animate_flip > 0) && pos == 0)
		return;

	//static const float SPACING = 2.8;
	float dir = 1;
	float loc, scale, angle;
	float falloff;
	
	if (pos < 0) {
		dir *= -1;
		pos *= -1;
	}

	loc = settings.coverSpacing * dir * (pow(pos + 1, -1) - 1);
	
	falloff = (pos)*2*settings.coverFallOff;
	
	scale = pow(pos + 1, -2);
	angle = -1 * dir * change_scale(scale, 0, 1, settings.coverAngle, 0);

	DrawBufferedCover(texture_id, loc, angle, falloff);
}


void draw_covers()
{
	int i;
	
	for(i = (-1*(COVER_COUNT/2.0)); i < 0; i++)
	{
		//Some logic to avoid drawing everything
		if(abs(self.shift+i) < settings.drawWindow)
			GRRLIB_Cover(i+self.shift, i+(COVER_COUNT/2.0));
	}
	
	for(i = ((COVER_COUNT/2.0)); i >= 0; i--)
	{
		// Some logic to avoid drawing everything
		if(abs(self.shift+i) < settings.drawWindow)
			GRRLIB_Cover(i+self.shift, i+(COVER_COUNT/2.0));
	}
	
	if (settings.theme) //white
	{
		GRRLIB_DrawImg(-50, 0, ambientlight_white_texture, 0, 1, 1, 0xEEEEEEFF);   // draw the left side fade
		GRRLIB_DrawImg(450, 0, ambientlight_white_texture, 180, 1, 1, 0xEEEEEEFF); // flip the graphic horizontally for the right side
	}
	else //default black
	{
		GRRLIB_DrawImg(-40, 0, ambientlight_texture, 0, 1, 1, 0xFFFFFFFF);   // draw the left side fade
		GRRLIB_DrawImg(440, 0, ambientlight_texture, 180, 1, 1, 0xFFFFFFFF); // flip the graphic horizontally for the right side
	}
}



void draw_game_title(int index, float textSize)
{
	if(index != -1)
	{
		int len = 0;
		
		struct discHdr *header = NULL;
		char gameName[36];
		char title[MAX_TITLE_LEN];
		
		header = &self.gameList[index];
		
		if(self.usingTitlesTxt){
			sprintf(title, "%s", header->title);
			getTitle(titleList, (char*)header->id, title);
		}
		
		if (self.gameCnt < 1)
		{
			sprintf(gameName, "%s", "Please add a game...");
			len = strlen(gameName);
		}
		else
		{
			if(self.usingTitlesTxt)
				len = strlen(title);
			else
				len = strlen(header->title);
			// chomp the title to fit
			// TODO: Lengthen this for TTF
#ifndef TTF_TEST
			if(len <= 35) //the length of the max title is 35 fixed width chars
			{
				if(self.usingTitlesTxt)
					sprintf(gameName, "%s", (title));
				else
					sprintf(gameName, "%s", (header->title));
			}
			else
			{
				if(self.usingTitlesTxt)
					strncpy(gameName, title, 32);
				else
					strncpy(gameName, header->title, 32);
				gameName[32] = '\0';
				strncat(gameName, "...", 3);
				len = 35;
			}
#else
			if(len <= 45) //the length of the max title is 45 22pt TTF chars
			{
				if(self.usingTitlesTxt)
					sprintf(gameName, "%s", (title));
				else
					sprintf(gameName, "%s", (header->title));
			}
			else
			{
				if(self.usingTitlesTxt)
					strncpy(gameName, title, 42);
				else
					strncpy(gameName, header->title, 42);
				gameName[42] = '\0';
				strncat(gameName, "...", 3);
				len = 45;
			}
			
#endif
		}
		

#ifndef TTF_TEST
		len = strlen(gameName);
		
		float offset = (len*7.9); // calc a font scaled offset from title length
		
		if((int)offset > 260)
			offset = 260.0; // dont draw on top of the setting button

		GRRLIB_Printf(340 - (int)offset, 400, font_title, 0xFFFFFFFF, textSize, "%s", gameName);
#else
		if (settings.theme) // black text on white matte
		{
			CFreeTypeGX_DrawText(ttf22pt, 320, 410, CFreeTypeGX_charToWideChar(ttf22pt, gameName), (GXColor){0xff, 0xff, 0xff, 0xff}, 0x0002);
			CFreeTypeGX_DrawText(ttf22pt, 320, 410, CFreeTypeGX_charToWideChar(ttf22pt, gameName), (GXColor){0x00, 0x00, 0x00, 0xcc}, 0x0002);
		}
		else //white text on black matte
		{
			CFreeTypeGX_DrawText(ttf22pt, 320, 410, CFreeTypeGX_charToWideChar(ttf22pt, gameName), (GXColor){0x00, 0x00, 0x00, 0xff}, 0x0002);
			CFreeTypeGX_DrawText(ttf22pt, 320, 410, CFreeTypeGX_charToWideChar(ttf22pt, gameName), (GXColor){0xff, 0xff, 0xff, 0xdd}, 0x0002);
		}
#endif		
	}
}

int draw_selected_two(bool load, bool hover)
{
        /*Animate fliping cover*/
	if(self.selected && self.animate_flip < 1.0)
	{
		self.animate_rotate = 0.0;
		self.animate_load  = 0.0;
		self.animate_load_speed = ANIMATE_SPEED;
		self.animate_direction = 1;
		
		self.animate_flip += FLIP_SPEED;
		if(self.animate_flip > 1.0)
			self.animate_flip = 1.0;
			
		if(self.animate_flip > 0.3 && self.animate_flip < 0.7)
			self.animate_flip = 0.7;
	}
	else if(!self.selected)
	{
		self.animate_flip -= FLIP_SPEED;
		
		if(self.animate_flip > 0.3 && self.animate_flip < 0.7)
			self.animate_flip = 0.3;
		
		if(self.animate_flip < 0)
			self.animate_flip = 0;
	}
	
	float dir = 1;
	float loc, scale, angle;

	loc = settings.coverSpacing * dir * (pow(1, -1) - 1);
	scale = change_scale(self.animate_flip, 0, 1, 0, 270);
	angle = -1 * dir * scale;

	if(load)
	{
		self.animate_rotate+=5;
		if(self.animate_rotate == 360) self.animate_rotate = 0;
		
		self.animate_load_speed -= 4;
		
		if(self.animate_count < 0)
		{
			self.animate_slide_x+=8;
		}
		else
		{
			self.animate_count--;
		}
			
		if(self.animate_slide_x > 530)
		{
			return 1;
		}
		
		if(self.animate_load < 15)
			self.animate_load+=1;
	}
	else if(hover)
	{
		
		if(self.animate_load < 15 )
		{
			self.animate_load+=1;
		}	
	}
	else
	{
		if(self.animate_load > 0)
			self.animate_load--;
	}
	
		
	if(scale >= 250)//-16
	{
		GRRLIB_DrawImg(64, 110, load_bg_texture, 0, 1, 1, 0xFFFFFFFF);
		
		loadButton.x = 245;
		loadButton.y = 316;
		backButton.x = 335;
		backButton.y = 316;
		deleteButton.x = 425;
		deleteButton.y = 316;
		gsettingsButton.x = 520;
		gsettingsButton.y = 312;
		
		Button_Paint(&loadButton);
		Button_Paint(&backButton);
		//Button_Toggle_Paint(&bookmarkOffButton, &bookmarkOnButton, self.dummy);
		
		if(!settings.parentalLock)
			Button_Hover(&deleteButton, pointer.p_x, pointer.p_y);
			
		Button_Hover(&backButton, pointer.p_x, pointer.p_y);
		
		if(!settings.parentalLock)
		{
			Button_Paint(&gsettingsButton);
			Button_Hover(&gsettingsButton, pointer.p_x, pointer.p_y);
		}
		
		if(!settings.parentalLock)
			Button_Paint(&deleteButton);
		
#ifndef TEST_MODE
		struct discHdr *header = NULL;
		header = &self.gameList[self.gameSelected];
		char gameName[MAX_TITLE_LEN]; 


		if(self.usingTitlesTxt){
			sprintf(gameName, "%s", header->title);
			getTitle(titleList, (char*)header->id, gameName);
		}
		else
			sprintf(gameName, "%s", (header->title));
		// chomp the title to fit
		if(strlen(gameName) >= 20)
		{
			//strncpy(gameName, header->title, 17);
			gameName[17] = '\0';
			strncat(gameName, "...", 3);
		}
		/*
		if(strlen(header->title) < 20)
		{
			sprintf(gameName, "%s", (header->title));
		}
		else
		{
			strncpy(gameName, header->title, 17);
			gameName[17] = '\0';
			strncat(gameName, "...", 3);
		}
		*/

		// Display Title, Last Played, and Size
		GRRLIB_Printf(245, 174, font_title, 0xFFFFFFFF, 1, "%s", gameName);
		if((strcmp(gameSetting.lastPlayed, "-1"))==0)
			GRRLIB_Printf(255, 220, font_title_small, 0xFFFFFFFF, 1, localStr("M032", "Never played before"));
		else
			GRRLIB_Printf(255, 220, font_title_small, 0xFFFFFFFF, 1, localStr("M033", "Played %s"),gameSetting.lastPlayed);
		GRRLIB_Printf(255, 240, font_title_small, 0xFFFFFFFF, 1, localStr("M034", "Size: %.2f GB"), self.gsize);
#else
		GRRLIB_Printf(270, 174, font_title, 0xFFFFFFFF, 1, "%s", "Best game");
		GRRLIB_Printf(280, 210, font_title_small, 0xFFFFFFFF, 1, "%s", " Game ID: KBGSUX");
		GRRLIB_Printf(280, 230, font_title_small, 0xFFFFFFFF, 1, "Size:    %.2fGB", self.gsize);
#endif
		
		// DISC IMAGE
		if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
			GRRLIB_DrawImg(86+self.animate_slide_x+self.animate_load,170, current_cover_texture, self.animate_rotate, AR_16_9, AR_16_9, 0xFFFFFFFF);
		else
			GRRLIB_DrawImg(76+self.animate_slide_x+self.animate_load,170, current_cover_texture, self.animate_rotate, 1, 1, 0xFFFFFFFF);
		
		if(self.gameSelected < MAX_BUFFERED_COVERS || self.gameSelected >= 0)
		{
			if(BUFFER_IsCoverReady(self.gameSelected))
			{
				pthread_mutex_lock(&buffer_mutex[self.gameSelected]);
				if(_texture_data[self.gameSelected].data)
				{
					if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
						GRRLIB_DrawImg(60, 131, _texture_data[self.gameSelected], 0, AR_16_9, 1, 0xFFFFFFFF);
					else
						GRRLIB_DrawImg(60, 131, _texture_data[self.gameSelected], 0, 1, 1, 0xFFFFFFFF);
				}
				else
				{
					if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
						GRRLIB_DrawImg(60, 131, cover_texture, 0, AR_16_9, 1, 0xFFFFFFFF);
					else
						GRRLIB_DrawImg(60, 131, cover_texture, 0, 1, 1, 0xFFFFFFFF);
				}
					
				pthread_mutex_unlock(&buffer_mutex[self.gameSelected]);
			}
			else
			{	if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
					GRRLIB_DrawImg(60, 131, cover_texture, 0, 1, AR_16_9, 0xFFFFFFFF);
				else
					GRRLIB_DrawImg(60, 131, cover_texture, 0, 1, 1, 0xFFFFFFFF);
			}	
		}
		else
		{	
			if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
				GRRLIB_DrawImg(60, 131, cover_texture, 0, AR_16_9, 1, 0xFFFFFFFF);
			else
				GRRLIB_DrawImg(60, 131, cover_texture, 0, 1, 1, 0xFFFFFFFF);
		}	
		
  }
  else
  {
	DrawBufferedCover(self.gameSelected, loc, angle,  0 );
  }
  
  return 0;
}

void draw_selected()
{
	//WindowPrompt("zucca!", "topa", 0, &okButton);
	
	if(self.selected && self.animate_flip < 1.0)
	{
		self.animate_flip += FLIP_SPEED;
		if(self.animate_flip > 1.0)
			self.animate_flip = 1.0;
			
		if(self.animate_flip > 0.3 && self.animate_flip < 0.7)
			self.animate_flip = 0.7;
	}
	else if(!self.selected)
	{
		self.animate_flip -= FLIP_SPEED;
		
		if(self.animate_flip > 0.3 && self.animate_flip < 0.7)
			self.animate_flip = 0.3;
		
		if(self.animate_flip < 0)
			self.animate_flip = 0;
	}
	
	float dir = 1;
	float loc, scale, angle;
	
	loc = settings.coverSpacing * dir * (pow(1, -1) - 1);
	scale = change_scale(self.animate_flip, 0, 1, 0, 360);
	angle = -1 * dir * scale;
	
	if(scale >= 180)
	{
		//Use back art texture
		GRRLIB_DrawCoverImg(loc*1.2,back_texture,angle,1.4,0xFFFFFFFF, 0, settings.theme);
	
		if(scale >= 360)
		{
			int i ;
			int len;
			
			self.animate_rotate++;
			
			if(self.animate_rotate == 360) self.animate_rotate = 0;
			
			GRRLIB_DrawImg(230,100, current_cover_texture, self.animate_rotate, 1, 1, 0xFFFFFFFF);
			
			Button_Paint(&loadButton);
			Button_Paint(&backButton);
			Button_Paint(&deleteButton);
			
#ifndef TEST_MODE
			struct discHdr *header = NULL;
			header = &self.gameList[self.gameSelected];
			f32 size = 0.0;

			/* Get game size */
			WBFS_GameSize(header->id, &size);
			char name[MAX_TITLE_LEN];
			
			if(self.usingTitlesTxt){
				sprintf(name, "%s", header->title);
				getTitle(titleList, (char*)header->id, name);
			}
			else{
				WindowPrompt("zucca!", "eheh", 0, &okButton);
				for(i = 0; i < MAX_TITLE_LEN; i++)
					name[i] = toupper(header->title[i]);
			}
			
			len = strlen(name);
			
			float tsize = .8;

			if(len > 20)
				tsize = .6;

			if(len > 28)
				tsize = .4;
				
			if(len > 40)
				tsize = .3;
			
			int offset = (len*10);
			
			if(offset > 240)
				offset = 240;
			
			GRRLIB_Printf(300 - offset, 10, font_texture, settings.fontColor, tsize, "%s", name);
			GRRLIB_Printf(210, 50, font_texture, settings.fontColor, .4, "(%c%c%c%c) (%.2fGB)", header->id[0], header->id[1], header->id[2], header->id[3], size);
#else
			GRRLIB_Printf(90, 10, font_texture, settings.fontColor, .8, "%s", "JUSTINS GAME");
			GRRLIB_Printf(180, 50, font_texture, settings.fontColor, .5, "%s", "JUSTINS GAME");
#endif
		}
  }
  else
  {
	DrawBufferedCover(self.gameSelected, loc, angle, 0);
	
  }
}

float change_scale_without_containing(float val, float in_min, float in_max, float out_min, float out_max)
{
  float percent = 0;
  
  if (in_min == in_max)
    return 0;
  
  percent = (val - in_min) / (in_max - in_min);
  
  return (out_min + percent * (out_max - out_min));
}

float change_scale(float val, float in_min, float in_max, float out_min, float out_max)
{
	if(val > in_max)
		val = in_max;
  
	if(val < in_min)
		val = in_min;
  
	return change_scale_without_containing(val, in_min, in_max, out_min, out_max);
}

int WindowPrompt(char* title, char* txt, struct Button* choice_a, struct Button* choice_b)
{
	bool doloop = true;
	char* pch;
	
	if(choice_a == 0 && choice_b == 0)
	{
		doloop = false;
	}
	else
	{
		doloop = true;
	}
		
	do{
		WPAD_ScanPads();
		PAD_ScanPads();
		GetWiimoteData();

		//if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		//	exit(0);
		
		if((WPAD_ButtonsDown(0) & WPAD_BUTTON_B)|| (PAD_ButtonsDown(0) & PAD_BUTTON_B))
			return false;
		
		if((WPAD_ButtonsDown(0) & WPAD_BUTTON_A) || (PAD_ButtonsDown(0) & PAD_BUTTON_A))
		{
			if(choice_a != 0)
			{
				if(Button_Select(choice_a, pointer.p_x, pointer.p_y))
					return true;
			}
			if(choice_b != 0)
			{
				if(Button_Select(choice_b, pointer.p_x, pointer.p_y))
					return false;
			}
		}

		// Draw the dialog panel
		GRRLIB_DrawImg(50, 80, load_bg_texture, 0, 1, 1.25, 0xFFFFFFFF);

		// Draw buttons
		if(choice_a != 0 && choice_b != 0){
			choice_a->x = 320-5-80;
			choice_b->x = 320+5;
#ifndef TTF_TEST
			Button_Paint(choice_a); 
			Button_Paint(choice_b);
#else
			GRRLIB_DrawImg(choice_a->x, 290, ttf_button_black, 0, 1, 1, 0xFFFFFFFF);
			CFreeTypeGX_DrawText(ttf16pt, (choice_a->x)+40, 310, CFreeTypeGX_charToWideChar(ttf16pt, "OK"), (GXColor){0xff, 0xff, 0xff, 0xff}, 0x0002);
			GRRLIB_DrawImg(choice_b->x, 290, ttf_button_black, 0, 1, 1, 0xFFFFFFFF);
			CFreeTypeGX_DrawText(ttf16pt, (choice_b->x)+40, 310, CFreeTypeGX_charToWideChar(ttf16pt, "CANCEL"), (GXColor){0xff, 0xff, 0xff, 0xff}, 0x0002);
#endif
		}
		
		else{
			if(choice_a != 0){
				choice_a->x = 320-40;
#ifndef TTF_TEST
				Button_Paint(choice_a); 
#else
				GRRLIB_DrawImg(choice_a->x, 290, ttf_button_black, 0, 1, 1, 0xFFFFFFFF);
				CFreeTypeGX_DrawText(ttf16pt, (choice_a->x)+40, 310, CFreeTypeGX_charToWideChar(ttf16pt, "OK"), (GXColor){0xff, 0xff, 0xff, 0xff}, 0x0002);
#endif
			}
			
			if(choice_b != 0){
				choice_b->x = 320-40;
#ifndef TTF_TEST
				Button_Paint(choice_b);
#else
				GRRLIB_DrawImg(choice_b->x, 290, ttf_button_black, 0, 1, 1, 0xFFFFFFFF);
				CFreeTypeGX_DrawText(ttf16pt, (choice_b->x)+40, 310, CFreeTypeGX_charToWideChar(ttf16pt, "CANCEL"), (GXColor){0xff, 0xff, 0xff, 0xff}, 0x0002);
#endif
			}
		}
		
		int y = 140;
		int sp = 0;
		
		// Draw text
#ifndef TTF_TEST
		GRRLIB_Printf(100, 105, font_title, 0xFFFFFFFF, 1, "%s", title);
#else
		CFreeTypeGX_DrawText(ttf22pt, 100, 105, CFreeTypeGX_charToWideChar(ttf22pt, title), (GXColor){0x00, 0x00, 0x00, 0xff}, 0x0001);
		CFreeTypeGX_DrawText(ttf22pt, 100, 105, CFreeTypeGX_charToWideChar(ttf22pt, title), (GXColor){0xff, 0xff, 0xff, 0xdd}, 0x0001);
#endif
		if(txt != NULL)
		{
			char* msg = malloc(strlen(txt)*sizeof(char));
			sprintf(msg, txt);
			
			pch = strtok(msg, "\n");
			while (pch != NULL)
			{
#ifndef TTF_TEST
				GRRLIB_Printf(138, y+sp, font_title_small, settings.fontColor, 1, "%s", pch);
#else
				CFreeTypeGX_DrawText(ttf16pt, 135, y+sp, CFreeTypeGX_charToWideChar(ttf16pt, pch), (GXColor){0x00, 0x00, 0x00, 0xff}, 0x0001);
				CFreeTypeGX_DrawText(ttf16pt, 135, y+sp, CFreeTypeGX_charToWideChar(ttf16pt, pch), (GXColor){0xff, 0xff, 0xff, 0xdd}, 0x0001);
#endif
				pch = strtok(NULL, "\n");
				sp+=16;
			}
			free(msg);
        }
		// Check for button-pointer intersections, and rumble
		if (Button_Hover(choice_a, pointer.p_x, pointer.p_y) ||
			Button_Hover(choice_b, pointer.p_x, pointer.p_y))
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
		// Draw the default pointer hand
		if(doloop)
			DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		
		GRRLIB_Render();
		
	}while(doloop);
	self.rumbleAmt = 0;
	return false;
}

/****************************************************************************
 * ShowProgress
 *
 * Updates the variables used by the progress window for drawing a progress
 * bar. Also resumes the progress window thread if it is suspended.
 ***************************************************************************/

void ShowProgress (s32 done, s32 total)
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

	//sprintf(prozent, "%s%0.2f%%", "Installing Game...", percent);

    sprintf(timet,localStr("M035", "Installing... (%0.2f%%) Time left: %d:%02d:%02d"),percent, h,m,s);

	/*Update and Draw Progress Window Here*/
	//WindowPrompt(prozent, timet, 0, 0);
	
	Paint_Progress_Generic(done, total, timet); 
}


int ProgressWindow(wbfs_t *hdd, char* title, char* msg)
{
	//Freeze Buffer thread during install
	pthread_mutex_lock(&lock_thread_mutex);
	
	/*TODO Draw Window*/
	_title = title;
	_msg   = msg;
	
	int ret = wbfs_add_disc(hdd, __WBFS_ReadDVD, NULL, ShowProgress, ONLY_GAME_PARTITION, 0);
	
	self.progress = 0.0;
	
	//Resume Buffer Thread
	pthread_mutex_unlock(&lock_thread_mutex);
	Sleep(1);
	
	return ret;

}

void GetWiimoteData()
{
	WPAD_IR(WPAD_CHAN_0, &self.ir); // Let's get our infrared data
	WPAD_Orientation(WPAD_CHAN_0, &self.orient);
	
	pointer.p_x = self.ir.sx-160; // This is to adjust to overscan
	pointer.p_y = self.ir.sy-220; // Same
	pointer.p_ang = self.ir.angle/2; // Set angle/2 to translate correctly
	pointer.p_type = 0;
}


void DrawCursor(int type, f32 xpos, f32 ypos, float degrees, float scaleX, f32 scaleY, u32 color )
{
	// draw the pointer shadow
	GRRLIB_DrawImg(pointer.p_x+4, pointer.p_y+4, pointer_shadow_texture, pointer.p_ang, 1, 1, color);
	switch (type)
	{
		case 0: // default cursor
			GRRLIB_DrawImg(pointer.p_x, pointer.p_y, pointer_texture, pointer.p_ang, 1, 1, color);
			break;
		case 1: // turning hand cursor
			GRRLIB_DrawImg(pointer.p_x, pointer.p_y, turn_point_texture, pointer.p_ang, 1, 1, color);
			break;
		default:
			GRRLIB_DrawImg(pointer.p_x, pointer.p_y, pointer_texture, pointer.p_ang, 1, 1, color);
			break;
	}
	
}
void game_settings_menu()
{
    //get/set per-game settings
    struct discHdr *header = NULL;
    header = &self.gameList[self.gameSelected];
    char titleID[7];
    char gameName[MAX_TITLE_LEN];

    // chomp the title to fit
	if(self.usingTitlesTxt){
		sprintf(gameName, "%s", header->title);
		getTitle(titleList, (char*)header->id, gameName);
	}
	else
		sprintf(gameName, "%s", (header->title));
	
    if(strlen(header->title) >= 20)
    {
		//strncpy(gameName, header->title, 17);
		gameName[17] = '\0';
		strncat(gameName, "...", 3);
    }
	
    sprintf(titleID, "%s", header->id);
    if(!getGameSettings(titleID, &gameSetting));
    {
        setGameSettings(titleID, &gameSetting,-1);
        getGameSettings(titleID, &gameSetting);
    }
    // ocarina will be -1 if we never been into game settings before
    if(gameSetting.ocarina == -1)
    {
        gameSetting.ocarina = 0;
        gameSetting.hooktype = 0;
        gameSetting.language = 0;
        gameSetting.video = 0;
        gameSetting.vipatch = 0;
    }

    bool doloop = true;
    do{
		WPAD_ScanPads();

		GetWiimoteData();

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
                {
                    setGameSettings(titleID, &gameSetting,-1);
                    return;
                }
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
                {
                    setGameSettings(titleID, &gameSetting,-1);
                    return;
                }
		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
		{
			if(Button_Select(&gbackButton, pointer.p_x, pointer.p_y))
			{
				setGameSettings(titleID, &gameSetting,-1);
				return;
			}
			else if (Button_Select(&gcheatonButton, pointer.p_x, pointer.p_y) || Button_Select(&gcheatoffButton, pointer.p_x, pointer.p_y))
			{
				gameSetting.ocarina = (gameSetting.ocarina) ? 0 : 1; // Clicked the Ocarina button, toggle state
			}
			else if (Button_Select(&gvidtvonButton, pointer.p_x, pointer.p_y) || Button_Select(&gvidtvoffButton, pointer.p_x, pointer.p_y))
			{
				gameSetting.vipatch = (gameSetting.vipatch) ? 0 : 1; // Clicked the VIPATCH button, toggle state
			}
			else if (Button_Select(&glangdownButton, pointer.p_x, pointer.p_y))
			{ // Clicked on the language buttons
				if (gameSetting.language > 0)
				{
					gameSetting.language --;
				}
				else
				{
					gameSetting.language = (CFG_LANG_COUNT - 1);
				}
			}
			else if (Button_Select(&glangupButton, pointer.p_x, pointer.p_y))
			{
				if (gameSetting.language < (CFG_LANG_COUNT - 1))
				{
					gameSetting.language ++;
				}
				else
				{
					gameSetting.language = 0;
				}
			}
			else if (Button_Select(&ghookdownButton, pointer.p_x, pointer.p_y))
			{ // Clicked on the hooktype buttons
				if (gameSetting.hooktype > 0)
				{
					gameSetting.hooktype --;
				}
				else
				{
					gameSetting.hooktype = (CFG_HOOK_COUNT - 1);
				}
			}
			else if (Button_Select(&ghookupButton, pointer.p_x, pointer.p_y))
			{
				if (gameSetting.hooktype < (CFG_HOOK_COUNT - 1))
				{
					gameSetting.hooktype ++;
				}
				else
				{
					gameSetting.hooktype = 0;
				}
			}
			else if (Button_Select(&gviddownButton, pointer.p_x,pointer.p_y))
			{
				// Clicked on the video down button
				if (gameSetting.video > 0)
				{
					gameSetting.video --;
				}
				else
				{
					gameSetting.video = (CFG_VIDEO_COUNT -1);
				}
			}
			else if (Button_Select(&gvidupButton, pointer.p_x,pointer.p_y))
			{
				// Clicked on the video up button
				if (gameSetting.video <(CFG_VIDEO_COUNT -1))
				{
					gameSetting.video ++;
				}
				else
				{
					gameSetting.video = 0;
				}
			}
		}

		GRRLIB_FillScreen(0x000000FF);
		GRRLIB_DrawImg(64, 110, load_bg_texture, 0, 1, 1, 0xFFFFFFFF);

		gbackButton.x = 474;
		gbackButton.y = 320;

		Button_Paint(&gbackButton);
		Button_Paint(&glangupButton);
		Button_Paint(&glangdownButton);
		Button_Paint(&gvidupButton);
		Button_Paint(&gviddownButton);
		Button_Paint(&ghookupButton);
		Button_Paint(&ghookdownButton);
		Button_Toggle_Paint(&gcheatoffButton, &gcheatonButton, gameSetting.ocarina);
		Button_Toggle_Paint(&gvidtvoffButton, &gvidtvonButton, gameSetting.vipatch);

		Button_Hover(&gbackButton, pointer.p_x, pointer.p_y);
		Button_Hover(&glangupButton, pointer.p_x, pointer.p_y);
		Button_Hover(&glangdownButton, pointer.p_x, pointer.p_y);
		Button_Hover(&gvidupButton, pointer.p_x, pointer.p_y);
		Button_Hover(&gviddownButton, pointer.p_x, pointer.p_y);
		Button_Hover(&gcheatoffButton, pointer.p_x, pointer.p_y);
		Button_Hover(&gcheatonButton, pointer.p_x, pointer.p_y);
		Button_Hover(&gvidtvoffButton, pointer.p_x, pointer.p_y);
		Button_Hover(&gvidtvonButton, pointer.p_x, pointer.p_y);
		Button_Hover(&ghookupButton, pointer.p_x, pointer.p_y);
		Button_Hover(&ghookdownButton, pointer.p_x, pointer.p_y);

        //BUTTON TEXT
		GRRLIB_Printf(89, 145,  font_title, settings.fontColor, 1, localStr("M036", "%s settings"), gameName);
		GRRLIB_Printf(169, 193,  font_texture, settings.fontColor, 1, localStr("M135", "Ocarina:"));
		GRRLIB_Printf(334, 193,  font_texture, settings.fontColor, 1, localStr("M037", "Hook:"));
		GRRLIB_Printf(409, 193,  font_texture, 0xFFFFFFFF, 1, "%s",ghooks[gameSetting.hooktype]);
		GRRLIB_Printf(169, 228, font_texture, settings.fontColor, 1, localStr("M038", "Language:"));
		GRRLIB_Printf(354, 228, font_texture, 0xFFFFFFFF, 1, "%s",glanguages[gameSetting.language]);
		GRRLIB_Printf(169, 257, font_texture, settings.fontColor, 1, localStr("M039", "Video mode:"));
		GRRLIB_Printf(354, 255, font_texture, 0xFFFFFFFF, 1, "%s",gvidmodes[gameSetting.video]);
		GRRLIB_Printf(169, 289, font_texture, settings.fontColor, 1, localStr("M040", "VIDTV patch:"));

		// Draw the default pointer hand
		if(doloop)
			DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);

		// Check for button-pointer intersections, and rumble
		if ((Button_Hover(&gbackButton, pointer.p_x, pointer.p_y) ||
			   Button_Hover(&glangupButton, pointer.p_x, pointer.p_y) ||
			   Button_Hover(&glangdownButton, pointer.p_x, pointer.p_y) ||
			   Button_Hover(&gvidupButton, pointer.p_x, pointer.p_y) ||
			   Button_Hover(&gviddownButton, pointer.p_x, pointer.p_y) ||
			   Button_Hover(&gcheatoffButton, pointer.p_x, pointer.p_y) ||
			   Button_Hover(&gcheatonButton, pointer.p_x, pointer.p_y) ||
			   Button_Hover(&gvidtvoffButton, pointer.p_x, pointer.p_y) ||
			   Button_Hover(&gvidtvonButton, pointer.p_x, pointer.p_y) ||
			   Button_Hover(&ghookupButton, pointer.p_x, pointer.p_y) ||
			   Button_Hover(&ghookdownButton, pointer.p_x, pointer.p_y)))
		{
			// Should we be rumbling?
			if (--self.rumbleAmt > 0)
			{
				if(settings.rumble)
					WPAD_Rumble(0,1); // Turn on Wiimote rumble
			}
			else 
				WPAD_Rumble(0,0); // Kill the rumble
		}
		else
		{ // If no button is being hovered, kill the rumble
			WPAD_Rumble(0,0);
			self.rumbleAmt = 5;
		}
		
		GRRLIB_Render();

	}while(doloop);

    return;
}

void freeResources(){

	free(pointer_texture.data);
	free(pointer_shadow_texture.data);
	free(cover_texture.data);
	free(back_texture.data);
	free(empty_texture.data);
	free(no_disc_texture.data);
	free(current_cover_texture.data);
	free(font_texture.data);
	free(font_title.data);
	free(font_title_small.data);
	free(progress_texture.data);
	
	GRRLIB_FillScreen(0x000000FF);
	GRRLIB_Render();
	
	GRRLIB_FillScreen(0x000000FF);
	GRRLIB_Render();
	
	GRRLIB_FillScreen(0x000000FF);
	GRRLIB_Render();
	
	GRRLIB_FillScreen(0x000000FF);
	GRRLIB_Render();
	
	GRRLIB_Exit();
	
}

bool CoverHoverCenter()
{
	float fx_min, fx_max, fy_min, fy_max;    // these will hold our calculated edge coords at the appropriate zoom level
	float cur_zoom     = settings.coverZoom; // our current zoom level
	float zoom_in      = 0.69;  // max zoom in value
	float zoom_out     = -8.0;  // max zoom out value
	float xmin_zoomin  = 172.0; // the follow can be fine tuned... or probably calculated from the texture size... TODO
	float xmin_zoomout = 231.0;
	float xmax_zoomin  = 386.0;
	float xmax_zoomout = 329.0;
	float ymin_zoomin  = 40.0;
	float ymin_zoomout = 127.0;
	float ymax_zoomin  = 360.0;
	float ymax_zoomout = 270.0;
	
	fx_min = change_scale(cur_zoom, zoom_out, zoom_in, xmin_zoomout, xmin_zoomin);
	fx_max = change_scale(cur_zoom, zoom_out, zoom_in, xmax_zoomout, xmax_zoomin);
	fy_min = change_scale(cur_zoom, zoom_out, zoom_in, ymin_zoomout, ymin_zoomin);
	fy_max = change_scale(cur_zoom, zoom_out, zoom_in, ymax_zoomout, ymax_zoomin);
	
	#ifdef TEST_MODE
	return true;
	#endif
	
	if (pointer.p_x > fx_min && pointer.p_x < fx_max && pointer.p_y > fy_min && pointer.p_y < fy_max)
		return true;
	else
		return false;
}

bool CoverHoverLeft()
{
	float fx_l_edge, fy_min, fy_max;    // these will hold our calculated edge coords at the appropriate zoom level
	float cur_zoom     = settings.coverZoom; // our current zoom level
	float zoom_out     = -8.0;  // max zoom in value
	float zoom_in      = 0.69;  // max zoom out value
	float xedge_zoomin  = 130.0; // the follow can be fine tuned... or probably calculated from the texture size... TODO
	float xedge_zoomout = 206.0;
	float ymin_zoomin  = 40.0;
	float ymin_zoomout = 127.0;
	float ymax_zoomin  = 360.0;
	float ymax_zoomout = 270.0;
	
	fx_l_edge = change_scale(cur_zoom, zoom_out, zoom_in, xedge_zoomout, xedge_zoomin);
	fy_min    = change_scale(cur_zoom, zoom_out, zoom_in, ymin_zoomout, ymin_zoomin);
	fy_max    = change_scale(cur_zoom, zoom_out, zoom_in, ymax_zoomout, ymax_zoomin);
	
	if (pointer.p_x < fx_l_edge && pointer.p_y > fy_min && pointer.p_y < fy_max)
		return true;
	else
		return false;
}

bool CoverHoverRight()
{
	
	float fx_r_edge, fy_min, fy_max;    // these will hold our calculated edge coords at the appropriate zoom level
	float cur_zoom     = settings.coverZoom; // our current zoom level
	float zoom_out     = -8.0;  // max zoom in value
	float zoom_in      = 0.69;  // max zoom out value
	float xedge_zoomin  = 430.0; // the follow can be fine tuned... or probably calculated from the texture size... TODO
	float xedge_zoomout = 350.0;
	float ymin_zoomin  = 40.0;
	float ymin_zoomout = 127.0;
	float ymax_zoomin  = 360.0;
	float ymax_zoomout = 270.0;
	
	fx_r_edge = change_scale(cur_zoom, zoom_out, zoom_in, xedge_zoomout, xedge_zoomin);
	fy_min    = change_scale(cur_zoom, zoom_out, zoom_in, ymin_zoomout, ymin_zoomin);
	fy_max    = change_scale(cur_zoom, zoom_out, zoom_in, ymax_zoomout, ymax_zoomin);
	
	if (pointer.p_x > fx_r_edge && pointer.p_y > fy_min && pointer.p_y < fy_max)
		return true;
	else
		return false;
}


