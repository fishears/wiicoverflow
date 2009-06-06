#include "gfx.h"
#include "settings.h"
#include "localization.h"
#include "buffer.h"
#include "CFreeTypeGX.h"

extern s_self self;
extern s_pointer pointer;
extern s_settings settings;
extern s_gameSettings gameSetting;
extern s_title* titleList;
extern s_coverFlip coverFlip[];
extern int COVER_COUNT;

void ResetBuffer()
{
	BUFFER_KillBuffer();
	Sleep(300);
	BUFFER_InitBuffer(BUFFER_THREAD_COUNT);
	
	if(settings.covers3d)
		BUFFER_3D_COVERS();
	else
		BUFFER_2D_COVERS();
	
	BUFFER_KillBuffer();
	InitializeBuffer(self.gameList,self.gameCnt,BUFFER_WINDOW,COVER_COUNT/2.0 +self.shift);
	BUFFER_InitBuffer();
	UpdateBufferedImages();
}

void LoadFonts()
{
	ttf14pt = CFreeTypeGX_new();
    ttf16pt = CFreeTypeGX_new();
	ttf18pt = CFreeTypeGX_new();
	ttf20pt = CFreeTypeGX_new();
	ttf24pt = CFreeTypeGX_new();
    CFreeTypeGX_LoadFont(ttf14pt, font_ttf, font_ttf_size, 14, true);
	CFreeTypeGX_LoadFont(ttf16pt, font_ttf, font_ttf_size, 16, true);
	CFreeTypeGX_LoadFont(ttf18pt, font_ttf, font_ttf_size, 18, true);
	CFreeTypeGX_LoadFont(ttf20pt, font_ttf, font_ttf_size, 20, true);
	CFreeTypeGX_LoadFont(ttf24pt, font_ttf, font_ttf_size, 24, true);
}


void LoadTextures()
{
	pointer_texture				= GRRLIB_LoadTexturePNG(pointer_basic_png);
	pointer_shadow_texture		= GRRLIB_LoadTexturePNG(pointer_shadow_png);
	turn_point_texture			= GRRLIB_LoadTexturePNG(pointer_turning_png); // can't find free
	cover_texture				= GRRLIB_LoadTexturePNG(no_cover_png);
	cover_texture_3d			= GRRLIB_LoadTexturePNG(full_cover_png);
	no_disc_texture				= GRRLIB_LoadTexturePNG(no_disc_png);
	slidebar_texture			= GRRLIB_LoadTexturePNG(slidebar_png);  // can't find free
	slidebar_white_texture		= GRRLIB_CreateEmptyTexture(slidebar_texture.w, slidebar_texture.h);
	GRRLIB_BMFX_Invert(slidebar_texture, slidebar_white_texture); //invert the slider black to white
	ambientlight_texture		= GRRLIB_LoadTexturePNG(ambientlight_png);  // can't find free
	ambientlight_white_texture	= GRRLIB_CreateEmptyTexture(ambientlight_texture.w, ambientlight_texture.h);
	GRRLIB_BMFX_Invert(ambientlight_texture, ambientlight_white_texture); //invert the fade from black to white
	battery_bar					= GRRLIB_LoadTexturePNG(battery_bar_png);  // can't find free
    battery_bar_red				= GRRLIB_LoadTexturePNG(battery_bar_red_png);  // can't find free
    battery						= GRRLIB_LoadTexturePNG(battery_png);  // can't find free
    battery_dead				= GRRLIB_LoadTexturePNG(battery_dead_png);  // can't find free
	menu_button_texture			= GRRLIB_LoadTexturePNG(menu_button_png);
	menu_button_over_texture	= GRRLIB_LoadTexturePNG(menu_button_over_png);	
	menu_header_texture			= GRRLIB_LoadTexturePNG(menu_header_png);
	menu_header_vflip_texture	= GRRLIB_CreateEmptyTexture(menu_header_texture.w, menu_header_texture.h);
	GRRLIB_BMFX_FlipV(menu_header_texture, menu_header_vflip_texture);
	menu_logo_texture			= GRRLIB_LoadTexturePNG(menu_logo_png);
	dialog_box_titlebar_texture	= GRRLIB_LoadTexturePNG(dialog_box_titlebar_png);
	dialog_box_titlebar_long_texture	= GRRLIB_LoadTexturePNG(dialog_box_titlebar_long_png);
	dialog_box_icon_texture		= GRRLIB_LoadTexturePNG(dialog_box_icon_png);
	menu_graphics_wireframe_texture	= GRRLIB_LoadTexturePNG(menu_graphics_wireframe_png);
	menu_graphics_box1_texture	= GRRLIB_LoadTexturePNG(menu_graphics_box1_png);
	menu_graphics_box2_texture	= GRRLIB_LoadTexturePNG(menu_graphics_box2_png);
	//flag_jp_texture = GRRLIB_LoadTexturePNG(flag_jp_png);
	flag_br_texture = GRRLIB_LoadTexturePNG(flag_br_png);
	//flag_cn_texture = GRRLIB_LoadTexturePNG(flag_cn_png);
	flag_da_texture = GRRLIB_LoadTexturePNG(flag_da_png);
	flag_de_texture = GRRLIB_LoadTexturePNG(flag_de_png);
	flag_fr_texture = GRRLIB_LoadTexturePNG(flag_fr_png);
	flag_it_texture = GRRLIB_LoadTexturePNG(flag_it_png);
	//flag_tw_texture = GRRLIB_LoadTexturePNG(flag_tw_png);
	//flag_pt_texture = GRRLIB_LoadTexturePNG(flag_pt_png);
	//flag_ru_texture = GRRLIB_LoadTexturePNG(flag_ru_png);
	flag_uk_texture = GRRLIB_LoadTexturePNG(flag_uk_png);
	flag_us_texture = GRRLIB_LoadTexturePNG(flag_us_png);
	flag_es_texture = GRRLIB_LoadTexturePNG(flag_es_png);
	flag_ct_texture = GRRLIB_LoadTexturePNG(flag_ct_png);
	flag_fi_texture = GRRLIB_LoadTexturePNG(flag_fi_png);
	flag_MyLang_texture = GRRLIB_LoadTexturePNG(flag_MyLang_png);
	flag_dk_texture = GRRLIB_LoadTexturePNG(flag_dk_png);
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
				if(settings.covers3d)
				{
					GRRLIB_DrawCoverImg(loc*1.2,cover_texture_3d,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
				}
				else
				{
					GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
				}
			}
			pthread_mutex_unlock(&buffer_mutex[i]);
		}
		else
		{
			if(settings.covers3d)
			{
				GRRLIB_DrawCoverImg(loc*1.2,cover_texture_3d,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
			}
			else
			{
				GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
			}
		}	
	}
	else
	{
		if(settings.covers3d)
		{
			GRRLIB_DrawCoverImg(loc*1.2,cover_texture_3d,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
		}
		else
		{
			GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
		}
	}	

}

void Paint_Progress(float v, char* msg)
{
	int i;
	int count = (int)(v*10);
	
	if(count > 26)
		count = 26;

	GRRLIB_2D_Init();

	CFreeTypeGX_DrawText(ttf18pt, 320, 220, TX.welcomeMsg, (GXColor){0xee, 0xee, 0xee, 0xff}, FTGX_JUSTIFY_CENTER);
	GRRLIB_DrawImg(162, 230, progress_bar_texture, 0, 1, 1, 0xFFFFFFFF);
	GRRLIB_DrawImgReflection(162, 230 + progress_bar_texture.h + 5, progress_bar_texture, 0, 1, 1, 1.0);

	for(i = 0; i < count; i++)
	{
		GRRLIB_DrawImg(165+12*i, 232, progress_step_texture, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_DrawImgReflection(165+12*i, 232 + progress_step_texture.h + 9, progress_step_texture, 0, 1, 1, 1.0);
	}
	
#ifdef DEBUG
	if(msg != NULL)
		CFreeTypeGX_DrawText(ttf16pt, 320, 320, msg, (GXColor){0x66, 0x66, 0x66, 0xff}, FTGX_JUSTIFY_CENTER);
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

	//GRRLIB_2D_Init();
	
	GRRLIB_DrawImg(162, 230, progress_bar_texture, 0, 1, 1, 0xFFFFFFFF);
	GRRLIB_DrawImgReflection(162, 230 + progress_bar_texture.h + 5, progress_bar_texture, 0, 1, 1, 1.0);

	
	for(i = 0; i < count; i++)
	{
		GRRLIB_DrawImg(165+12*i, 232, progress_step_texture, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_DrawImgReflection(165+12*i, 232 + progress_step_texture.h + 9, progress_step_texture, 0, 1, 1, 1.0);
	}
	
	#ifdef DEBUG
	if(msg != NULL)
		CFreeTypeGX_DrawText(ttf16pt, 320, 320,  msg, (GXColor){0x66, 0x66, 0x66, 0xff}, FTGX_JUSTIFY_CENTER);
    #endif
    
	GRRLIB_Render();
}

void Init_Buttons()
{
	/////////////////////////////////
	// Main screen
	slideButton				= Button_Init(slide_png, slide_hover_png, 260, 426);
	settingsButton	    	= Button_Init(button_round_gear_png, button_round_gear_over_png, 30, 427);
	addButton				= Button_Init(button_round_add_png,  button_round_add_over_png, 580, 427);
 	// Dialog Box Buttons
	okButton				= Button_TTF_Init(button_bar_h28w104_black_png, button_bar_h28w104_white_png, 300, 290, TX.okB);
	cancelButton			= Button_TTF_Init(button_bar_h28w104_black_png, button_bar_h28w104_white_png, 414, 290, TX.cancelB);
	yesButton               = Button_TTF_Init(button_bar_h28w104_black_png, button_bar_h28w104_white_png, 300, 290, TX.yesB);
    noButton                = Button_TTF_Init(button_bar_h28w104_black_png, button_bar_h28w104_white_png, 414, 290, TX.noB);
	// Game Launch Panel Buttons
	loadButton				= Button_TTF_Init(button_bar_h28w104_black_png, button_bar_h28w104_white_png, 353, 335, TX.loadB);
    backButton				= Button_TTF_Init(button_bar_h28w104_black_png, button_bar_h28w104_white_png, 468, 335, TX.backB);
	lockButton			    = Button_Init(button_round_lock_png, button_round_lock_over_png, 543, 120);
	unlockButton			= Button_Init(button_round_unlock_png, button_round_unlock_over_png, 543, 120);
	deleteButton			= Button_Init(button_round_delete_png, button_round_delete_over_png, 505, 120);
    gsettingsButton			= Button_Init(button_round_gear_png, button_round_gear_over_png, 543, 120);
	bookmarkOnButton        = Button_Init(star_on_png, star_on_png, 515, 140);  
    bookmarkOffButton       = Button_Init(star_off_png, star_on_png, 515, 140);
	// 'Home' button menu buttons
    homeMenuTopButton       = Button_Init(menu_home_top_png,     menu_home_top_over_png, 0, 0);
    homeMenuBottomButton    = Button_Init(menu_home_bottom_png,  menu_home_bottom_over_png, 0, 376);
    wiiMenuButton           = Button_TTF_Init(menu_home_button_png,  menu_home_button_over_png, 46, 187, TX.wiimenuB);
    loaderButton            = Button_TTF_Init(menu_home_button_png,  menu_home_button_over_png, 186, 187, TX.loaderB);
    wiimoteButton           = Button_Init(menu_home_wiimote_png, menu_home_wiimote_png, 54, 400);
	// Settings Panels Header Buttons
    menuSettingsButton		= Button_TTF_Init(menu_button_png, menu_button_over_png, 160, 20, TX.cflowSettings); 
	menuGraphicsButton		= Button_TTF_Init(menu_button_png, menu_button_over_png, 320, 20, TX.graphics);
    menuLanguagesButton		= Button_TTF_Init(menu_button_png, menu_button_over_png, 480, 20, TX.languagesB); 
	menuLogoButton			= Button_Init(menu_logo_png, menu_logo_png, 30, 20);
	// General Settings Panel
    musicOnButton           = Button_Init(button_bar_h28w104_toggle_right_png, button_bar_h28w104_toggle_right_png, 310, 97);
    musicOffButton          = Button_Init(button_bar_h28w104_toggle_left_png, button_bar_h28w104_toggle_left_png, 310, 97);
    rumbleOnButton          = Button_Init(button_bar_h28w104_toggle_right_png, button_bar_h28w104_toggle_right_png, 310, 131);
    rumbleOffButton         = Button_Init(button_bar_h28w104_toggle_left_png, button_bar_h28w104_toggle_left_png, 310, 131);
    quickstartOnButton      = Button_Init(button_bar_h28w104_toggle_right_png, button_bar_h28w104_toggle_right_png, 310, 165);
    quickstartOffButton     = Button_Init(button_bar_h28w104_toggle_left_png, button_bar_h28w104_toggle_left_png, 310, 165);
    themeBlackButton        = Button_TTF_Init(button_bar_h28w104_black_png, button_bar_h28w104_white_png, 310, 199, TX.blackB);
    themeWhiteButton        = Button_TTF_Init(button_bar_h28w104_black_png, button_bar_h28w104_white_png, 422, 199, TX.whiteB);
	coversButton			= Button_TTF_Init(button_bar_h28w104_black_png, button_bar_h28w104_white_png, 310, 265, TX.coversB);
	titlesButton		    = Button_TTF_Init(button_bar_h28w104_black_png, button_bar_h28w104_white_png, 422, 265, TX.titlesB);
    viddownButton			= Button_Init(button_minus_png, button_minus_over_png, 310,334);
	vidupButton				= Button_Init(button_plus_png, button_plus_over_png, 338,334);
    vidtvonButton			= Button_Init(button_bar_h28w104_toggle_right_png, button_bar_h28w104_toggle_right_png, 310, 299);
    vidtvoffButton			= Button_Init(button_bar_h28w104_toggle_left_png, button_bar_h28w104_toggle_left_png, 310, 299);
    cheatonButton			= Button_Init(button_bar_h28w104_toggle_right_png, button_bar_h28w104_toggle_right_png, 310,367);
    cheatoffButton			= Button_Init(button_bar_h28w104_toggle_left_png, button_bar_h28w104_toggle_left_png, 310,367);
    hookdownButton			= Button_Init(button_minus_png, button_minus_over_png, 422,367);
    hookupButton			= Button_Init(button_plus_png, button_plus_over_png, 450,367);
	// Graphic Settings Panel
    falloffdownButton       = Button_Init(button_minus_png, button_minus_over_png, 99,123);
    falloffupButton         = Button_Init(button_plus_png, button_plus_over_png,127,123);
    windowdownButton        = Button_Init(button_minus_png, button_minus_over_png, 195,110);
    windowupButton          = Button_Init(button_plus_png, button_plus_over_png, 223, 110);
    zoomdownButton          = Button_Init(button_minus_png, button_minus_over_png, 292,110);
    zoomupButton            = Button_Init(button_plus_png, button_plus_over_png, 320,110);
    spacingdownButton       = Button_Init(button_minus_png, button_minus_over_png, 386,110);
    spacingupButton         = Button_Init(button_plus_png, button_plus_over_png, 414,110);
    angledownButton         = Button_Init(button_minus_png, button_minus_over_png, 484,123);
    angleupButton           = Button_Init(button_plus_png, button_plus_over_png, 512,123);
    hidescrollOnButton		= Button_Init(button_bar_h28w104_toggle_right_png, button_bar_h28w104_toggle_right_png, 101,369);
    hidescrollOffButton		= Button_Init(button_bar_h28w104_toggle_left_png, button_bar_h28w104_toggle_left_png, 101, 369);
    covers3dOnButton		= Button_Init(button_bar_h28w104_toggle_right_png, button_bar_h28w104_toggle_right_png, 268, 369);
    covers3dOffButton		= Button_Init(button_bar_h28w104_toggle_left_png, button_bar_h28w104_toggle_left_png, 268, 369);
    coverTextOnButton       = Button_Init(button_bar_h28w104_toggle_right_png, button_bar_h28w104_toggle_right_png, 435, 369);
    coverTextOffButton      = Button_Init(button_bar_h28w104_toggle_left_png, button_bar_h28w104_toggle_left_png, 435, 369);
	// Language Settings Panel
	langupButton			= Button_Init(button_plus_png, button_plus_over_png,321,90);
    langdownButton			= Button_Init(button_minus_png, button_minus_over_png, 293,90);
	flagUSButton			= Button_Flag_Init(flag_us_png, 166, 150, "United States"); // TODO: Need to create TX for all of these
	flagITButton			= Button_Flag_Init(flag_it_png, 296, 150, "Italy");
	flagDEButton			= Button_Flag_Init(flag_de_png, 426, 150, "Germany");
	flagUKButton			= Button_Flag_Init(flag_uk_png, 166, 220, "United Kingdom");
	flagBRButton			= Button_Flag_Init(flag_br_png, 296, 220, "Brazil");
	flagFRButton			= Button_Flag_Init(flag_fr_png, 426, 220, "France");
	flagDAButton			= Button_Flag_Init(flag_da_png, 166, 290, "Netherlands");
	//flagJPButton			= Button_Flag_Init(flag_jp_png, 296, 290, "Japan"); 
	flagFIButton			= Button_Flag_Init(flag_fi_png, 296, 290, "Finnland"); 
	//flagPTButton			= Button_Flag_Init(flag_pt_png, 426, 290, "Portugal");
	flagDKButton			= Button_Flag_Init(flag_dk_png, 426, 290, "Denmark");
	//flagTWButton			= Button_Flag_Init(flag_tw_png, 166, 360, "Taiwan");
	//flagRUButton			= Button_Flag_Init(flag_ru_png, 296, 360, "Russia");
	//flagCNButton			= Button_Flag_Init(flag_cn_png, 426, 360, "China");
	flagESButton			= Button_Flag_Init(flag_es_png, 166, 360, "Spain");		
	flagCTButton			= Button_Flag_Init(flag_ct_png, 296, 360, "Catalan");
	flagMyLangButton 		= Button_Flag_Init(flag_MyLang_png, 426, 360, "MyLanguage");
			
	// Game Settings Screen Buttons
    gvidtvonButton          = Button_Init(button_bar_h28w104_toggle_right_png, button_bar_h28w104_toggle_right_png, 360, 164);
    gvidtvoffButton         = Button_Init(button_bar_h28w104_toggle_left_png, button_bar_h28w104_toggle_left_png, 360, 164);
	gcheatonButton			= Button_Init(button_bar_h28w104_toggle_right_png, button_bar_h28w104_toggle_right_png, 360, 198);
    gcheatoffButton			= Button_Init(button_bar_h28w104_toggle_left_png, button_bar_h28w104_toggle_left_png, 360, 198);
    ghookdownButton         = Button_Init(button_minus_png, button_minus_over_png, 360, 232);
    ghookupButton           = Button_Init(button_plus_png, button_plus_over_png, 388, 232);
    glangdownButton         = Button_Init(button_minus_png, button_minus_over_png, 360,266);
	glangupButton           = Button_Init(button_plus_png, button_plus_over_png,388,266);
    gviddownButton          = Button_Init(button_minus_png, button_minus_over_png, 360, 300);
	gvidupButton            = Button_Init(button_plus_png, button_plus_over_png, 388, 300);
	gbackButton				= Button_TTF_Init(button_bar_h28w104_black_png, button_bar_h28w104_white_png, 468, 335, TX.backB);
} // End Init_Buttons();

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
		
	if(settings.covers3d)
	{
		if(coverFlip[texture_id].flip)
		{
			coverFlip[texture_id].angle+=3;
			if(coverFlip[texture_id].angle >=180)
				coverFlip[texture_id].angle = 180;
				
			angle += coverFlip[texture_id].angle;
		}
		else if(!coverFlip[texture_id].flip)
		{
			if(coverFlip[texture_id].angle > 0)
			{
				coverFlip[texture_id].angle-=3;
			}
			else
				coverFlip[texture_id].angle = 0;
				
			angle += coverFlip[texture_id].angle;
			
		}
	}
	
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
		
		if (self.gameCnt < 1) // No games yet
		{
			sprintf(gameName, "%s", TX.addAGame);
			len = strlen(gameName);
		}
		else
		{
			if(self.usingTitlesTxt)
				len = strlen(title);
			else
				len = strlen(header->title);

			// chomp the title to fit
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
		}
		
		if (settings.theme)
			CFreeTypeGX_DrawText(ttf20pt, 320, 410,  gameName, (GXColor){0x11, 0x11, 0x11, 0xff}, FTGX_JUSTIFY_CENTER);
		else
			CFreeTypeGX_DrawText(ttf20pt, 320, 410,  gameName, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_CENTER);
	}
}

int DrawLoadGameDialog(bool load, bool hover)
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
	
	if(settings.covers3d)
	{
		scale = change_scale(self.animate_flip, 0, 1, 0, 360);
		angle = -1 * dir * scale;
		
		angle += coverFlip[self.gameSelected].angle;
	}
	else
	{
		scale = change_scale(self.animate_flip, 0, 1, 0, 270);
		angle = -1 * dir * scale ;
	}
	
	if(load)
	{
		self.animate_rotate+=5;
		if(self.animate_rotate == 360)
			self.animate_rotate = 0;
		
		self.animate_load_speed -= 4;
		
		if(self.animate_count < 0)
			self.animate_slide_x+=8;
		else
			self.animate_count--;
	
		if(self.animate_slide_x > 530)
			return 1;
		
		if(self.animate_load < 15)
			self.animate_load+=1;
	}
	else if(hover)
	{
		
		if(self.animate_load < 15 )
			self.animate_load+=1;
	}
	else
	{
		if(self.animate_load > 0)
			self.animate_load--;
	}
	
	int check = 0;
	
	if(settings.covers3d)
	{
		if(scale >= 180)//-16
			check = 1;
	}
	else
	{
		if(scale >= 250)//-16
			check = 1;
	}
	
	if(check)
	{
		// Draw the background panel
		GRRLIB_Rectangle(40, 106, 560, 276, 0xffffffdd, true);
		GRRLIB_Rectangle(42, 108, 556, 272, 0x737373FF, true);
		GRRLIB_Rectangle(268, 168, 304, 44, 0xffffffdd, true);
		GRRLIB_Rectangle(270, 170, 300, 40, 0x000000FF, true);
		// Draw the buttons
		
		
		Button_TTF_Paint(&backButton);
		//Button_Toggle_Paint(&bookmarkOffButton, &bookmarkOnButton, self.dummy);
		if(!settings.parentalLock)
		{
			Button_Paint(&deleteButton);
			Button_Paint(&gsettingsButton);
			
			Button_Hover(&gsettingsButton, pointer.p_x, pointer.p_y);
			Button_Hover(&deleteButton, pointer.p_x, pointer.p_y);
			
		}
		
		Button_TTF_Paint(&loadButton);

		
		Button_Hover(&backButton, pointer.p_x, pointer.p_y);

		// Get the title info
		struct discHdr *header = NULL;
		header = &self.gameList[self.gameSelected];
		char gameName[MAX_TITLE_LEN]; 
		
		if(self.usingTitlesTxt)
		{
			// Load a custom title
			sprintf(gameName, "%s", header->title);
			getTitle(titleList, (char*)header->id, gameName);
		}
		else
			sprintf(gameName, "%s", (header->title));

		// Chomp the title to fit
		if(strlen(gameName) >= 22)
		{
			//strncpy(gameName, header->title, 17);
			gameName[19] = '\0';
			strncat(gameName, "...", 3);
		}
		
		// Display Title, Last Played, and Size
        char tTemp[50];
		sprintf(tTemp,"%s",gameName);
		CFreeTypeGX_DrawText(ttf20pt, 420, 200, tTemp, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_CENTER);

		if ((strcmp(gameSetting.lastPlayed, "-1"))==0)
			CFreeTypeGX_DrawText(ttf16pt, 270, 253, TX.neverPlayed, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
		else
		{
			sprintf(tTemp, TX.played, gameSetting.lastPlayed);
			CFreeTypeGX_DrawText(ttf16pt, 270, 253, tTemp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
		}
	
		sprintf(tTemp, TX.size, self.gsize);
		CFreeTypeGX_DrawText(ttf16pt, 270, 278, tTemp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
		
		// Draw the cover image
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
					{
						if(settings.covers3d)
							GRRLIB_DrawFlatCoverImg(60, 131, _texture_data[self.gameSelected], 0, AR_16_9, 1, 0xFFFFFFFF);
						else
							GRRLIB_DrawImg(60, 131, _texture_data[self.gameSelected], 0, AR_16_9, 1, 0xFFFFFFFF);
					}
					else
					{
						if(settings.covers3d)
							GRRLIB_DrawFlatCoverImg(60, 131, _texture_data[self.gameSelected], 0, 1, 1, 0xFFFFFFFF);
						else
							GRRLIB_DrawImg(60, 131, _texture_data[self.gameSelected], 0, 1, 1, 0xFFFFFFFF);
					}
				}
				else
				{
					if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
					{
						if(settings.covers3d)
							GRRLIB_DrawFlatCoverImg(60, 131, cover_texture_3d, 0, AR_16_9, 1, 0xFFFFFFFF);
						else
							GRRLIB_DrawImg(60, 131, cover_texture, 0, AR_16_9, 1, 0xFFFFFFFF);
					}
					else
					{
						if(settings.covers3d)
							GRRLIB_DrawFlatCoverImg(60, 131, cover_texture_3d, 0, 1, 1, 0xFFFFFFFF);
						else
							GRRLIB_DrawImg(60, 131, cover_texture, 0, 1, 1, 0xFFFFFFFF);
					}
				}
				pthread_mutex_unlock(&buffer_mutex[self.gameSelected]);
			}
			else
			{	
				if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
				{
					if(settings.covers3d)
						GRRLIB_DrawFlatCoverImg(60, 131, cover_texture_3d, 0, AR_16_9, 1, 0xFFFFFFFF);
					else
						GRRLIB_DrawImg(60, 131, cover_texture, 0, AR_16_9, 1, 0xFFFFFFFF);
				}
				else
				{
					if(settings.covers3d)
						GRRLIB_DrawFlatCoverImg(60, 131, cover_texture_3d, 0, 1, 1, 0xFFFFFFFF);
					else
						GRRLIB_DrawImg(60, 131, cover_texture, 0, 1, 1, 0xFFFFFFFF);
				}
			}	
		}
		else
		{	
			if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
			{
				if(settings.covers3d)
					GRRLIB_DrawFlatCoverImg(60, 131, cover_texture_3d, 0, AR_16_9, 1, 0xFFFFFFFF);
				else
					GRRLIB_DrawImg(60, 131, cover_texture, 0, AR_16_9, 1, 0xFFFFFFFF);
			}
			else
			{
				if(settings.covers3d)
					GRRLIB_DrawFlatCoverImg(60, 131, cover_texture_3d, 0, 1, 1, 0xFFFFFFFF);
				else
					GRRLIB_DrawImg(60, 131, cover_texture, 0, 1, 1, 0xFFFFFFFF);
			}
		}	
		
	}
	else
	{
		DrawBufferedCover(self.gameSelected, loc, angle,  0 );
	}
	
	return 0;
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
	bool returnVal = false;
	bool doloop = true;
	char* pch;
	
	if(choice_a == 0 && choice_b == 0)
		doloop = false;
	
	/////////////////////////////////////
	// Draw the intro - lower the dialog
	int i = 1;
	int fade = 0x00;
	float moving_y;
	
	for(i = 0; i <= 20; i++)
	{
		// Draw the covers in the back
		draw_covers();
		GRRLIB_2D_Init();
		// Draw the screen fade
		GRRLIB_FillScreen(0x00000000|fade);
		fade+=8;
		// Draw the background panel
		moving_y = change_scale(i, 0, 20, -202, 148);
		GRRLIB_Rectangle(108, moving_y, 424, 184, 0xffffffdd, true);
		moving_y = change_scale(i, 0, 20, -200, 150);
		GRRLIB_Rectangle(110, moving_y, 420, 180, 0x737373FF, true);
		moving_y = change_scale(i, 0, 20, -210, 140);
		GRRLIB_DrawImg(90, moving_y, dialog_box_titlebar_long_texture, 0, 1, 1, 0xFFFFFFFF);
		// Draw buttons
		moving_y = change_scale(i, 0, 20, -60, 290);
		if(choice_a != 0 && choice_b != 0)
		{
			choice_a->y = moving_y;
			choice_b->y = moving_y;
			Button_TTF_Paint(choice_a);
			Button_TTF_Paint(choice_b);
		}
		else
		{
			if(choice_a != 0)
			{
				choice_a->y = moving_y;
				Button_TTF_Paint(choice_a);
			}
			if(choice_b != 0)
			{
				choice_b->y = moving_y;
				Button_TTF_Paint(choice_b);
			}
		}
		// Draw Icon and reflection
		moving_y = change_scale(i, 0, 20, -155, 195);
		GRRLIB_DrawImg(130, moving_y, dialog_box_icon_texture, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_DrawImgReflection(130, moving_y + dialog_box_icon_texture.h + 4, dialog_box_icon_texture, 0, 1, 1, 0.7);
		// Draw Dialog Box Title Text
		moving_y = change_scale(i, 0, 20, -197, 153);
		CFreeTypeGX_DrawText(ttf16pt, 215, moving_y, title, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		// Draw Dialog Box Body Text
		moving_y = change_scale(i, 0, 20, -160, 190);
		int lsp = moving_y;
		if(txt != NULL)
		{
			char* msg = malloc(strlen(txt)*sizeof(char));
			sprintf(msg, txt);
			
			pch = strtok(msg, "\n");
			while (pch != NULL)
			{
				CFreeTypeGX_DrawText(ttf16pt, 220, lsp, pch, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				pch  = strtok(NULL, "\n");
				lsp += 16;
			}
			free(msg);
        }
		// Draw the pointer
		WPAD_ScanPads();
		GetWiimoteData();
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		GRRLIB_Render();
	}
	
	
	do{
		///////////////////////
		// Handle Button events
		WPAD_ScanPads();
		PAD_ScanPads();
		GetWiimoteData();
		
		/////////////////////////////////////////////////////
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
		// Check for Button Events
		if((WPAD_ButtonsDown(0) & WPAD_BUTTON_B) || (PAD_ButtonsDown(0) & PAD_BUTTON_B))
		{
			doloop = false;
		}
		
		if((WPAD_ButtonsDown(0) & WPAD_BUTTON_1) || (PAD_ButtonsDown(0) & PAD_BUTTON_X))
			GRRLIB_ScrShot(USBLOADER_PATH "/sshot.png");

		if((WPAD_ButtonsDown(0) & WPAD_BUTTON_A) || (PAD_ButtonsDown(0) & PAD_BUTTON_A))
		{
			if(choice_a != 0)
			{
				if(Button_Select(choice_a, pointer.p_x, pointer.p_y))
				{
					doloop = false;
					returnVal = true;
					break;
				}
			}
			if(choice_b != 0)
			{
				if(Button_Select(choice_b, pointer.p_x, pointer.p_y))
				{
					doloop = false;
					break;
				}
				
			}
		}

		////////////////////////
		// Draw the dialog
		draw_covers();
		GRRLIB_2D_Init();
		// Draw the screen fade
		GRRLIB_FillScreen(0x00000000|fade);
		// Draw the background panel
		GRRLIB_Rectangle(108, 148, 424, 184, 0xffffffdd, true);
		GRRLIB_Rectangle(110, 150, 420, 180, 0x737373FF, true);
		GRRLIB_DrawImg(90, 140, dialog_box_titlebar_long_texture, 0, 1, 1, 0xFFFFFFFF);
		// Draw buttons
		if(choice_a != 0 && choice_b != 0)
		{
			Button_TTF_Paint(choice_a);
			Button_TTF_Paint(choice_b);
		}
		else
		{
			if(choice_a != 0)
				Button_TTF_Paint(choice_a);
			if(choice_b != 0)
				Button_TTF_Paint(choice_b);
		}
		// Draw Icon and reflection
		GRRLIB_DrawImg(130, 195, dialog_box_icon_texture, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_DrawImgReflection(130, 195 + dialog_box_icon_texture.h + 4, dialog_box_icon_texture, 0, 1, 1, 0.7);
		// Draw Dialog Box Title Text
		CFreeTypeGX_DrawText(ttf16pt, 215, 153, title, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		// Draw Dialog Box Body Text
		int y = 190;
		int sp = 0;
		if(txt != NULL)
		{
			char* msg = malloc(strlen(txt)*sizeof(char));
			sprintf(msg, txt);
			
			pch = strtok(msg, "\n");
			while (pch != NULL)
			{
				CFreeTypeGX_DrawText(ttf16pt, 220, y+sp, pch, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				pch = strtok(NULL, "\n");
				sp+=16;
			}
			free(msg);
        }
		

		////////////////////////////////
		// Draw the default pointer hand
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		
		GRRLIB_Render();
		
	}while(doloop);
	
	/////////////////////////////////////
	// Draw the outro - raise the menu
	for(i = 0; i <= 20; i++)
	{
		// Draw the covers in the back
		draw_covers();
		GRRLIB_2D_Init();
		// Draw the screen fade
		GRRLIB_FillScreen(0x00000000|fade);
		fade-=8;
		// Draw the background panel
		moving_y = change_scale(i, 0, 20, 148, -202);
		GRRLIB_Rectangle(108, moving_y, 424, 184, 0xffffffdd, true);
		moving_y = change_scale(i, 0, 20, 150, -200);
		GRRLIB_Rectangle(110, moving_y, 420, 180, 0x737373FF, true);
		moving_y = change_scale(i, 0, 20, 140, -210);
		GRRLIB_DrawImg(90, moving_y, dialog_box_titlebar_long_texture, 0, 1, 1, 0xFFFFFFFF);
		// Draw buttons
		moving_y = change_scale(i, 0, 20, 290, -60);
		if(choice_a != 0 && choice_b != 0)
		{
			choice_a->y = moving_y;
			choice_b->y = moving_y;
			Button_TTF_Paint(choice_a);
			Button_TTF_Paint(choice_b);
		}
		else
		{
			if(choice_a != 0)
			{
				choice_a->y = moving_y;
				Button_TTF_Paint(choice_a);
			}
			if(choice_b != 0)
			{
				choice_b->y = moving_y;
				Button_TTF_Paint(choice_b);
			}
		}
		// Draw Icon and reflection
		moving_y = change_scale(i, 0, 20, 195, -155);
		GRRLIB_DrawImg(130, moving_y, dialog_box_icon_texture, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_DrawImgReflection(130, moving_y + dialog_box_icon_texture.h + 4, dialog_box_icon_texture, 0, 1, 1, 0.7);
		// Draw Dialog Box Title Text
		moving_y = change_scale(i, 0, 20, 153, -197);
		CFreeTypeGX_DrawText(ttf16pt, 215, moving_y, title, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		// Draw Dialog Box Body Text
		moving_y = change_scale(i, 0, 20, 190, -160);
		int lsp = moving_y;
		if(txt != NULL)
		{
			char* msg = malloc(strlen(txt)*sizeof(char));
			sprintf(msg, txt);
			
			pch = strtok(msg, "\n");
			while (pch != NULL)
			{
				CFreeTypeGX_DrawText(ttf16pt, 220, lsp, pch, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				pch  = strtok(NULL, "\n");
				lsp += 16;
			}
			free(msg);
        }
		// Draw the pointer
		WPAD_ScanPads();
		GetWiimoteData();
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		GRRLIB_Render();
	}
	
	self.rumbleAmt = 0;
	return returnVal;
}

int WindowPromptInstall(char* id,char* title, char* txt, struct Button* choice_a, struct Button* choice_b)
{
	bool returnVal = false;
	bool doloop = true;
	char* pch;
        GRRLIB_texImg myTex;
	unsigned char buffer[160 * 224 * 4 * 10];
	WPAD_Rumble(0,0); //sometimes rumble remain active
	if(networkInit(self.ipAddress))
	{
		Download_Cover(id, 1, self.gameCnt);
		//CoversDownloaded();

		char filepath[255];
		sprintf(filepath, USBLOADER_PATH "/covers/%s.png", id);
		int ret = Fat_ReadFileToBuffer(filepath, (void*)buffer, 160 * 224 * 4);
		if(ret > 0)
		{
			myTex = GRRLIB_LoadTexturePNG(buffer);
		}
		else
		{
			myTex = GRRLIB_LoadTexture(no_cover_png);
		}
	}
	else // no network
	{
		myTex = GRRLIB_LoadTexture(no_cover_png);
	}
	if(choice_a == 0 && choice_b == 0)
		doloop = false;

	/////////////////////////////////////
	// Draw the intro - lower the dialog
	int i = 1;
	int fade = 0x00;
	float moving_y;

	for(i = 0; i <= 20; i++)
	{
		// Draw the covers in the back
		draw_covers();
		GRRLIB_2D_Init();
		// Draw the screen fade
		GRRLIB_FillScreen(0x00000000|fade);
		fade+=8;
		// Draw the background panel
		moving_y = change_scale(i, 0, 20, -202, 148);
		GRRLIB_Rectangle(108, moving_y, 424, 184, 0xffffffdd, true);
		moving_y = change_scale(i, 0, 20, -200, 150);
		GRRLIB_Rectangle(110, moving_y, 420, 180, 0x737373FF, true);
		moving_y = change_scale(i, 0, 20, -210, 140);
		GRRLIB_DrawImg(90, moving_y, dialog_box_titlebar_long_texture, 0, 1, 1, 0xFFFFFFFF);
                // Draw the cover image
		if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
			GRRLIB_DrawImg(85, moving_y, myTex, 0, (AR_16_9)*0.7, 0.7, 0xFFFFFFFF);
		else
			GRRLIB_DrawImg(85, moving_y, myTex, 0, 0.7, 0.7, 0xFFFFFFFF);
		// Draw buttons
		moving_y = change_scale(i, 0, 20, -60, 290);
		if(choice_a != 0 && choice_b != 0)
		{
			choice_a->y = moving_y;
			choice_b->y = moving_y;
			Button_TTF_Paint(choice_a);
			Button_TTF_Paint(choice_b);
		}
		else
		{
			if(choice_a != 0)
			{
				choice_a->y = moving_y;
				Button_TTF_Paint(choice_a);
			}
			if(choice_b != 0)
			{
				choice_b->y = moving_y;
				Button_TTF_Paint(choice_b);
			}
		}
		// Draw Dialog Box Title Text
		moving_y = change_scale(i, 0, 20, -197, 153);
		CFreeTypeGX_DrawText(ttf16pt, 215, moving_y, title, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		// Draw Dialog Box Body Text
		moving_y = change_scale(i, 0, 20, -160, 190);
		int lsp = moving_y;
		if(txt != NULL)
		{
			char* msg = malloc(strlen(txt)*sizeof(char));
			sprintf(msg, txt);

			pch = strtok(msg, "\n");
			while (pch != NULL)
			{
				CFreeTypeGX_DrawText(ttf16pt, 220, lsp, pch, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				pch  = strtok(NULL, "\n");
				lsp += 16;
			}
			free(msg);
        }
		// Draw the pointer
		WPAD_ScanPads();
		GetWiimoteData();
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		GRRLIB_Render();
	}


	do{
		///////////////////////
		// Handle Button events
		WPAD_ScanPads();
		PAD_ScanPads();
		GetWiimoteData();

		/////////////////////////////////////////////////////
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
		// Check for Button Events
		if((WPAD_ButtonsDown(0) & WPAD_BUTTON_B) || (PAD_ButtonsDown(0) & PAD_BUTTON_B))
		{
			doloop = false;
		}

		if((WPAD_ButtonsDown(0) & WPAD_BUTTON_1) || (PAD_ButtonsDown(0) & PAD_BUTTON_X))
			GRRLIB_ScrShot(USBLOADER_PATH "/sshot.png");

		if((WPAD_ButtonsDown(0) & WPAD_BUTTON_A) || (PAD_ButtonsDown(0) & PAD_BUTTON_A))
		{
			if(choice_a != 0)
			{
				if(Button_Select(choice_a, pointer.p_x, pointer.p_y))
				{
					doloop = false;
					returnVal = true;
					break;
				}
			}
			if(choice_b != 0)
			{
				if(Button_Select(choice_b, pointer.p_x, pointer.p_y))
				{
					doloop = false;
					break;
				}

			}
		}

		////////////////////////
		// Draw the dialog
		draw_covers();
		GRRLIB_2D_Init();
		// Draw the screen fade
		GRRLIB_FillScreen(0x00000000|fade);
		// Draw the background panel
		GRRLIB_Rectangle(108, 148, 424, 184, 0xffffffdd, true);
		GRRLIB_Rectangle(110, 150, 420, 180, 0x737373FF, true);
		GRRLIB_DrawImg(90, 140, dialog_box_titlebar_long_texture, 0, 1, 1, 0xFFFFFFFF);
                // Draw the cover image
		if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
			GRRLIB_DrawImg(85, 133, myTex, 0, (AR_16_9)*0.7, 0.7, 0xFFFFFFFF);
		else
			GRRLIB_DrawImg(85, 133, myTex, 0, 0.7, 0.7, 0xFFFFFFFF);
		// Draw buttons
		if(choice_a != 0 && choice_b != 0)
		{
			Button_TTF_Paint(choice_a);
			Button_TTF_Paint(choice_b);
		}
		else
		{
			if(choice_a != 0)
				Button_TTF_Paint(choice_a);
			if(choice_b != 0)
				Button_TTF_Paint(choice_b);
		}
		// Draw Dialog Box Title Text
		CFreeTypeGX_DrawText(ttf16pt, 215, 153, title, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		// Draw Dialog Box Body Text
		int y = 190;
		int sp = 0;
		if(txt != NULL)
		{
			char* msg = malloc(strlen(txt)*sizeof(char));
			sprintf(msg, txt);

			pch = strtok(msg, "\n");
			while (pch != NULL)
			{
				CFreeTypeGX_DrawText(ttf16pt, 220, y+sp, pch, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				pch = strtok(NULL, "\n");
				sp+=16;
			}
			free(msg);
        }


		////////////////////////////////
		// Draw the default pointer hand
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);

		GRRLIB_Render();

	}while(doloop);

	/////////////////////////////////////
	// Draw the outro - raise the menu
	for(i = 0; i <= 20; i++)
	{
		// Draw the covers in the back
		draw_covers();
		GRRLIB_2D_Init();
		// Draw the screen fade
		GRRLIB_FillScreen(0x00000000|fade);
		fade-=8;
		// Draw the background panel
		moving_y = change_scale(i, 0, 20, 148, -202);
		GRRLIB_Rectangle(108, moving_y, 424, 184, 0xffffffdd, true);
		moving_y = change_scale(i, 0, 20, 150, -200);
		GRRLIB_Rectangle(110, moving_y, 420, 180, 0x737373FF, true);
		moving_y = change_scale(i, 0, 20, 140, -210);
		GRRLIB_DrawImg(90, moving_y, dialog_box_titlebar_long_texture, 0, 1, 1, 0xFFFFFFFF);
                // Draw the cover image
		if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
			GRRLIB_DrawImg(85, moving_y-10, myTex, 0, (AR_16_9)*0.7, 0.7, 0xFFFFFFFF);
		else
			GRRLIB_DrawImg(85, moving_y-10, myTex, 0, 0.7, 0.7, 0xFFFFFFFF);
		// Draw buttons
		moving_y = change_scale(i, 0, 20, 290, -60);
		if(choice_a != 0 && choice_b != 0)
		{
			choice_a->y = moving_y;
			choice_b->y = moving_y;
			Button_TTF_Paint(choice_a);
			Button_TTF_Paint(choice_b);
		}
		else
		{
			if(choice_a != 0)
			{
				choice_a->y = moving_y;
				Button_TTF_Paint(choice_a);
			}
			if(choice_b != 0)
			{
				choice_b->y = moving_y;
				Button_TTF_Paint(choice_b);
			}
		}
		// Draw Dialog Box Title Text
		moving_y = change_scale(i, 0, 20, 153, -197);
		CFreeTypeGX_DrawText(ttf16pt, 215, moving_y, title, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		// Draw Dialog Box Body Text
		moving_y = change_scale(i, 0, 20, 190, -160);
		int lsp = moving_y;
		if(txt != NULL)
		{
			char* msg = malloc(strlen(txt)*sizeof(char));
			sprintf(msg, txt);

			pch = strtok(msg, "\n");
			while (pch != NULL)
			{
				CFreeTypeGX_DrawText(ttf16pt, 220, lsp, pch, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				pch  = strtok(NULL, "\n");
				lsp += 16;
			}
			free(msg);
        }
		// Draw the pointer
		WPAD_ScanPads();
		GetWiimoteData();
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		GRRLIB_Render();
	}

	self.rumbleAmt = 0;
        free(myTex.data);
	return returnVal;
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

	char timet[1024];
    
	int percent; //, size;
	int d, h, m, s;

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
	m = (int)(d / 60) % 60;
	s =  (int)d % 60;

	//Calculate percentage/size
	percent = (int)(done * 100.0) / total;

	//sprintf(prozent, "%s%0.2f%%", "Installing Game...", percent);

	//sprintf(timet,TX.installing, percent, h,m,s);
    //sprintf(timet, "Installing %d (%d %d %d)", percent, h,m,s);//OK
	sprintf(timet, "Installing %d%% (%d:%02d:%02d)", percent, h,m,s);

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
	//int ret = wbfs_add_disc(hdd, __WBFS_ReadDVD, NULL, NULL, ONLY_GAME_PARTITION, 0);
	
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

void freeResources(){

	//textures
	free(pointer_texture.data);
	free(pointer_shadow_texture.data);
	free(turn_point_texture.data);
	free(cover_texture.data);
	free(cover_texture_3d.data);
	free(no_disc_texture.data);
	free(slidebar_texture.data);
	free(slidebar_white_texture.data);
	free(ambientlight_texture.data);
	free(ambientlight_white_texture.data);
	free(battery_bar.data);
    free(battery_bar_red.data);
    free(battery.data);
    free(battery_dead.data);
	free(menu_button_texture.data);
	free(menu_button_over_texture.data);
	free(menu_header_texture.data);
	free(menu_header_vflip_texture.data);
	free(menu_logo_texture.data);
	free(dialog_box_titlebar_texture.data);
	free(dialog_box_titlebar_long_texture.data);
	free(dialog_box_icon_texture.data);
	free(menu_graphics_wireframe_texture.data);
	free(menu_graphics_box1_texture.data);
	free(menu_graphics_box2_texture.data);
	//free(flag_jp_texture.data);
	free(flag_br_texture.data);
	//free(flag_cn_texture.data);
	free(flag_da_texture.data);
	free(flag_de_texture.data);
	free(flag_fr_texture.data);
	free(flag_it_texture.data);
	//free(flag_tw_texture.data);
	//free(flag_pt_texture.data);
	free(flag_dk_texture.data);
	//free(flag_ru_texture.data);
	free(flag_uk_texture.data);
	free(flag_us_texture.data);
	free(flag_es_texture.data);
	free(flag_ct_texture.data);
	free(flag_fi_texture.data);
	free(flag_MyLang_texture.data);
	if (coverLoaded)free(current_cover_texture.data); // is this always available?
	
	//buttons
	FreeButtonResources(&slideButton);
	FreeButtonResources(&settingsButton);
	FreeButtonResources(&addButton);
 	// Dialog Box Buttons
	FreeButtonResources(&okButton);
	FreeButtonResources(&cancelButton);
	FreeButtonResources(&yesButton);
    FreeButtonResources(&noButton);
	// Game Launch Panel Buttons
	FreeButtonResources(&loadButton);
    FreeButtonResources(&backButton);
	FreeButtonResources(&lockButton);
	FreeButtonResources(&unlockButton);
	FreeButtonResources(&deleteButton);
    FreeButtonResources(&gsettingsButton);
	FreeButtonResources(&bookmarkOnButton);
    FreeButtonResources(&bookmarkOffButton);
	// 'Home' button menu buttons
    FreeButtonResources(&homeMenuTopButton);
    FreeButtonResources(&homeMenuBottomButton);
    FreeButtonResources(&wiiMenuButton);
    FreeButtonResources(&loaderButton);
    FreeButtonResources(&wiimoteButton);
	// Settings Panels Header Buttons
    FreeButtonResources(&menuSettingsButton);
	FreeButtonResources(&menuGraphicsButton);
    FreeButtonResources(&menuLanguagesButton);
	FreeButtonResources(&menuLogoButton);
	// General Settings Panel
    FreeButtonResources(&musicOnButton);
    FreeButtonResources(&musicOffButton);
    FreeButtonResources(&rumbleOnButton);
    FreeButtonResources(&rumbleOffButton);
    FreeButtonResources(&quickstartOnButton);
    FreeButtonResources(&quickstartOffButton);
    FreeButtonResources(&themeBlackButton);
    FreeButtonResources(&themeWhiteButton);
	FreeButtonResources(&coversButton);
	FreeButtonResources(&titlesButton);
    FreeButtonResources(&viddownButton);
	FreeButtonResources(&vidupButton);
    FreeButtonResources(&vidtvonButton);
    FreeButtonResources(&vidtvoffButton);
    FreeButtonResources(&cheatonButton);
    FreeButtonResources(&cheatoffButton);
    FreeButtonResources(&hookdownButton);
    FreeButtonResources(&hookupButton);
	// Graphic Settings Panel
    FreeButtonResources(&falloffdownButton);
    FreeButtonResources(&falloffupButton);
    FreeButtonResources(&windowdownButton);
    FreeButtonResources(&windowupButton);
    FreeButtonResources(&zoomdownButton);
    FreeButtonResources(&zoomupButton);
    FreeButtonResources(&spacingdownButton);
    FreeButtonResources(&spacingupButton);
    FreeButtonResources(&angledownButton);
    FreeButtonResources(&angleupButton);
    FreeButtonResources(&hidescrollOnButton);
    FreeButtonResources(&hidescrollOffButton);
    FreeButtonResources(&covers3dOnButton);
    FreeButtonResources(&covers3dOffButton);
    FreeButtonResources(&coverTextOnButton);
    FreeButtonResources(&coverTextOffButton);
	// Language Settings Panel
	FreeButtonResources(&langupButton);
    FreeButtonResources(&langdownButton);
	FreeButtonResources(&flagUSButton);
	FreeButtonResources(&flagITButton);
	FreeButtonResources(&flagDEButton);
	FreeButtonResources(&flagUKButton);
	FreeButtonResources(&flagBRButton);
	FreeButtonResources(&flagFRButton);
	FreeButtonResources(&flagDAButton);
	//FreeButtonResources(&flagJPButton);
	FreeButtonResources(&flagFIButton);
	//FreeButtonResources(&flagPTButton);
	FreeButtonResources(&flagDKButton);
	//FreeButtonResources(&flagTWButton);
	//FreeButtonResources(&flagRUButton);
	//FreeButtonResources(&flagCNButton);
	FreeButtonResources(&flagESButton);
	FreeButtonResources(&flagCTButton);
	FreeButtonResources(&flagMyLangButton);
	// Game Settings Screen Buttons
    FreeButtonResources(&gvidtvonButton);
    FreeButtonResources(&gvidtvoffButton);
	FreeButtonResources(&gcheatonButton);
    FreeButtonResources(&gcheatoffButton);
    FreeButtonResources(&ghookdownButton);
    FreeButtonResources(&ghookupButton);
    FreeButtonResources(&glangdownButton);
	FreeButtonResources(&glangupButton);
    FreeButtonResources(&gviddownButton);
	FreeButtonResources(&gvidupButton);
	FreeButtonResources(&gbackButton);

/* //  CFreeTypeGX_delete function make the games don't boot, removed temporary (it wass addedd in r533)

	CFreeTypeGX_delete(ttf14pt);
    CFreeTypeGX_delete(ttf16pt);
	CFreeTypeGX_delete(ttf18pt);
	CFreeTypeGX_delete(ttf20pt);
	CFreeTypeGX_delete(ttf24pt);
*/
	
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


