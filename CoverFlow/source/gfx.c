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
	InitializeBuffer(self.gameList,self.gameCnt,BUFFER_WINDOW,COVER_COUNT/2.0 +self.shift,settings.covers3d);
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
	GRRLIB_LoadTexturePNG(&pointer_texture, pointer_basic_png);
	GRRLIB_LoadTexturePNG(&pointer_shadow_texture,pointer_shadow_png);
	GRRLIB_LoadTexturePNG(&turn_point_texture,pointer_turning_png); // can't find free
	BufferImageToSlot(&cover_texture,no_cover_png,7);
	BufferImageToSlot(&cover_texture_3d,full_cover_png,5); // read notes for MEM2_EXTENT before removing
	GRRLIB_LoadTexturePNG(&no_disc_texture,no_disc_png);
	GRRLIB_LoadTexturePNG(&slidebar_texture,slidebar_png);  // can't find free
	GRRLIB_CreateEmptyTexture(&slidebar_white_texture,slidebar_texture.w, slidebar_texture.h);
	GRRLIB_BMFX_Invert(slidebar_texture, slidebar_white_texture); //invert the slider black to white
	BufferImageToSlot(&ambientlight_texture,ambientlight_png,6);   // read notes for MEM2_EXTENT before removing
	GRRLIB_CreateEmptyTexture(&ambientlight_white_texture,ambientlight_texture.w, ambientlight_texture.h);
	GRRLIB_BMFX_Invert(ambientlight_texture, ambientlight_white_texture); //invert the fade from black to white
	GRRLIB_LoadTexturePNG(&battery_bar,battery_bar_png);  // can't find free
    GRRLIB_LoadTexturePNG(&battery_bar_red,battery_bar_red_png);  // can't find free
    GRRLIB_LoadTexturePNG(&battery,battery_png);  // can't find free
    GRRLIB_LoadTexturePNG(&battery_dead,battery_dead_png);  // can't find free
	GRRLIB_LoadTexturePNG(&menu_button_texture,menu_button_png);
	GRRLIB_LoadTexturePNG(&menu_button_over_texture,menu_button_over_png);	
	GRRLIB_LoadTexturePNG(&menu_header_texture,menu_header_png);
	GRRLIB_CreateEmptyTexture(&menu_header_vflip_texture,menu_header_texture.w, menu_header_texture.h);
	GRRLIB_BMFX_FlipV(menu_header_texture, menu_header_vflip_texture);
	GRRLIB_LoadTexturePNG(&menu_logo_texture,menu_logo_png);
	GRRLIB_LoadTexturePNG(&menu_loading_texture,menu_loading_png);
	GRRLIB_LoadTexturePNG(&dialog_box_titlebar_texture,dialog_box_titlebar_png);
	GRRLIB_LoadTexturePNG(&dialog_box_titlebar_long_texture,dialog_box_titlebar_long_png);
	GRRLIB_LoadTexturePNG(&dialog_box_icon_texture,dialog_box_icon_png);
	GRRLIB_LoadTexturePNG(&menu_graphics_wireframe_texture,menu_graphics_wireframe_png);
	GRRLIB_LoadTexturePNG(&menu_graphics_box1_texture,menu_graphics_box1_png);
	GRRLIB_LoadTexturePNG(&menu_graphics_box2_texture,menu_graphics_box2_png);
	GRRLIB_LoadTexturePNG(&flag_br_texture,flag_br_png);
	GRRLIB_LoadTexturePNG(&flag_da_texture,flag_da_png);
	GRRLIB_LoadTexturePNG(&flag_de_texture,flag_de_png);
	GRRLIB_LoadTexturePNG(&flag_fr_texture,flag_fr_png);
	GRRLIB_LoadTexturePNG(&flag_it_texture,flag_it_png);
	GRRLIB_LoadTexturePNG(&flag_tr_texture,flag_tr_png);
	GRRLIB_LoadTexturePNG(&flag_us_texture,flag_us_png);
	GRRLIB_LoadTexturePNG(&flag_es_texture,flag_es_png);
	GRRLIB_LoadTexturePNG(&flag_ct_texture,flag_ct_png);
	GRRLIB_LoadTexturePNG(&flag_fi_texture,flag_fi_png);
	GRRLIB_LoadTexturePNG(&flag_MyLang_texture,flag_MyLang_png);
	GRRLIB_LoadTexturePNG(&flag_dk_texture,flag_dk_png);
	GRRLIB_LoadTexturePNG(&flag_hu_texture,flag_hu_png);
	GRRLIB_LoadTexturePNG(&hdspace_texture,hdspace_png);
	GRRLIB_LoadTexturePNG(&button_round_info_texture,button_round_info_png);
	GRRLIB_LoadTexturePNG(&button_round_info_over_texture,button_round_info_over_png);	
}

void DrawBufferedCover(int i, float loc, float zpos, float angle, float falloff)
{
	self.isInit2D = false;

	if(i < MAX_BUFFERED_COVERS || i >= 0)
	{
		if(BUFFER_IsCoverReady(i))
		{
			pthread_mutex_lock(&buffer_mutex[i]);
			if(_texture_data[i].data)
			{
				GRRLIB_DrawCoverImg(loc*1.2, zpos,_texture_data[i],angle,1.0,0xFFFFFFFF, falloff, settings.theme);
			}
			else
			{
				if(settings.covers3d)
				{
					GRRLIB_DrawCoverImg(loc*1.2,zpos,cover_texture_3d,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
				}
				else
				{
					GRRLIB_DrawCoverImg(loc*1.2,zpos,cover_texture,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
				}
			}
			pthread_mutex_unlock(&buffer_mutex[i]);
		}
		else
		{
			if(settings.covers3d)
			{
				GRRLIB_DrawCoverImg(loc*1.2,zpos,cover_texture_3d,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
			}
			else
			{
				GRRLIB_DrawCoverImg(loc*1.2,zpos,cover_texture,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
			}
		}	
	}
	else
	{
		if(settings.covers3d)
		{
			GRRLIB_DrawCoverImg(loc*1.2,zpos,cover_texture_3d,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
		}
		else
		{
			GRRLIB_DrawCoverImg(loc*1.2,zpos,cover_texture,angle,1.0,0xFFFFFFFF, falloff, settings.theme);
		}
	}	

}

void Paint_Progress(float v, char* msg)
{
	int i;
	int count = (int)(v*10);
	
	if(count > 26)
		count = 26;
	
	if (!self.isInit2D){
		GRRLIB_2D_Init();
		self.isInit2D = true;
		}
	CFreeTypeGX_DrawText(ttf18pt, 320, 220, TX.welcomeMsg, (GXColor){0xee, 0xee, 0xee, 0xff}, FTGX_JUSTIFY_CENTER);
	for(i = 0; i < count; i++)
	{
		GRRLIB_DrawImg(165+12*i, 232, progress_step_texture, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_DrawImgReflection(165+12*i, 232 + progress_step_texture.h + 9, progress_step_texture, 0, 1, 1, 1.0);
	}
	GRRLIB_DrawImg(162, 230, progress_bar_texture, 0, 1, 1, 0xFFFFFFFF);
	GRRLIB_DrawImgReflection(162, 230 + progress_bar_texture.h + 5, progress_bar_texture, 0, 1, 1, 1.0);
	
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
	
	if (!self.isInit2D){
		GRRLIB_2D_Init();
		self.isInit2D = true;
		}

	int i;
	for(i = 0; i < count; i++)
	{
		GRRLIB_DrawImg(165+12*i, 232, progress_step_texture, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_DrawImgReflection(165+12*i, 232 + progress_step_texture.h + 9, progress_step_texture, 0, 1, 1, 1.0);
	}
	GRRLIB_DrawImg(162, 230, progress_bar_texture, 0, 1, 1, 0xFFFFFFFF);
	GRRLIB_DrawImgReflection(162, 230 + progress_bar_texture.h + 5, progress_bar_texture, 0, 1, 1, 1.0);
	
#ifdef DEBUG
	if(msg != NULL)
		CFreeTypeGX_DrawText(ttf16pt, 320, 320,  msg, (GXColor){0x66, 0x66, 0x66, 0xff}, FTGX_JUSTIFY_CENTER);
#endif
    
	GRRLIB_Render();
}

void Paint_Progress_FadeInStart()
{
	if (!self.isInit2D){
		GRRLIB_2D_Init();
		self.isInit2D = true;
		}

	int tFade;
	for(tFade=0xFF;tFade>=0x00;tFade-=8)
	{
		CFreeTypeGX_DrawText(ttf18pt, 320, 220, TX.welcomeMsg, (GXColor){0xee, 0xee, 0xee, 0xff}, FTGX_JUSTIFY_CENTER);
		GRRLIB_DrawImg(162, 230, progress_bar_texture, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_DrawImgReflection(162, 230 + progress_bar_texture.h + 5, progress_bar_texture, 0, 1, 1, 1.0);
		GRRLIB_FillScreen(0x00000000|tFade);
		GRRLIB_Render();
	}
	CFreeTypeGX_DrawText(ttf18pt, 320, 220, TX.welcomeMsg, (GXColor){0xee, 0xee, 0xee, 0xff}, FTGX_JUSTIFY_CENTER);
	GRRLIB_DrawImg(162, 230, progress_bar_texture, 0, 1, 1, 0xFFFFFFFF);
	GRRLIB_DrawImgReflection(162, 230 + progress_bar_texture.h + 5, progress_bar_texture, 0, 1, 1, 1.0);
	GRRLIB_Render();
}

void Paint_Progress_FadeToBG()
{
	int tFade;
	for(tFade=0x00;tFade<=255;tFade+=8)
	{
		if (!self.isInit2D){
			GRRLIB_2D_Init();
			self.isInit2D = true;
			}
		CFreeTypeGX_DrawText(ttf18pt, 320, 220, TX.welcomeMsg, (GXColor){0xee, 0xee, 0xee, 0xff}, FTGX_JUSTIFY_CENTER);
		GRRLIB_DrawImg(162, 230, progress_bar_texture, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_DrawImgReflection(162, 230 + progress_bar_texture.h + 5, progress_bar_texture, 0, 1, 1, 1.0);
		int tBar;
		for(tBar = 0; tBar < 26; tBar++)
		{
			GRRLIB_DrawImg(165+12*tBar, 232, progress_step_texture, 0, 1, 1, 0xFFFFFFFF);
			GRRLIB_DrawImgReflection(165+12*tBar, 232 + progress_step_texture.h + 9, progress_step_texture, 0, 1, 1, 1.0);
		}
		if (settings.theme)
			GRRLIB_FillScreen(0xCFCFCF00|tFade);
		else
			GRRLIB_FillScreen(0x00000000|tFade);
		GRRLIB_Render();
	}
	if (settings.theme)
		GRRLIB_FillScreen(0xFFFFFFFF);
	else
		GRRLIB_FillScreen(0x000000FF);
	GRRLIB_Render();
}


void Init_Buttons()
{
	/////////////////////////////////
	// Main screen
	Button_Init(&slideButton,slide_png, slide_hover_png, 260, 426);
	Button_Init(&settingsButton, button_round_gear_png, button_round_gear_over_png, 30, 427);
	Button_Init(&addButton, button_round_add_png,  button_round_add_over_png, 580, 427);
    Button_Init(&infoButton, button_round_info_png, button_round_info_over_png, 580, 44);
 	// Dialog Box Buttons
	Button_TTF_Init(&okButton, button_bar_h28w104_black_png, button_bar_h28w104_white_png, 300, 290, TX.okB);
	Duplicate_Button_TTF(&cancelButton, okButton, 414, 290, TX.cancelB);
	Duplicate_Button_TTF(&yesButton, okButton, 300, 290, TX.yesB);
    Duplicate_Button_TTF(&noButton, okButton, 414, 290, TX.noB);
	// Game Launch Panel Buttons
	Duplicate_Button_TTF(&loadButton, okButton, 353, 335, TX.loadB);
    Duplicate_Button_TTF(&backButton, okButton, 468, 335, TX.backB);
	Button_Init(&deleteButton, button_round_delete_png, button_round_delete_over_png, 505, 120);
    Duplicate_Button(&gsettingsButton, settingsButton, 543, 120);
	Button_Init(&bookmarkOnButton, star_on_png, star_on_png, 515, 140);  
    Button_Init(&bookmarkOffButton, star_off_png, star_on_png, 515, 140);
	// 'Home' button menu buttons
    Button_Init(&homeMenuTopButton, menu_home_top_png,     menu_home_top_over_png, 0, 0);
    Button_Init(&homeMenuBottomButton, menu_home_bottom_png,  menu_home_bottom_over_png, 0, 376);
    Button_TTF_Init(&wiiMenuButton, menu_home_button_png,  menu_home_button_over_png, 46, 187, TX.wiimenuB);
    Duplicate_Button_TTF(&loaderButton, wiiMenuButton, 186, 187, TX.loaderB);
    Button_Init(&wiimoteButton, menu_home_wiimote_png, menu_home_wiimote_png, 54, 400);
	// Settings Panels Header Buttons
    Button_TTF_Init(&menuSettingsButton, menu_button_png, menu_button_over_png, 160, 20, TX.cflowSettings); 
	Duplicate_Button_TTF(&menuGraphicsButton, menuSettingsButton, 320, 20, TX.graphics);
    Duplicate_Button_TTF(&menuLanguagesButton, menuSettingsButton, 480, 20, TX.languagesB); 
	Button_Init(&menuLogoButton, menu_logo_png, menu_logo_png, 30, 20);
	// General Settings Panel
    Button_Init(&musicOnButton, button_bar_h28w104_toggle_right_png, button_bar_h28w104_toggle_right_png, 310, 97);
    Button_Init(&musicOffButton, button_bar_h28w104_toggle_left_png, button_bar_h28w104_toggle_left_png, 310, 97);
    Duplicate_Button(&rumbleOnButton, musicOnButton, 310, 131);
    Duplicate_Button(&rumbleOffButton, musicOffButton, 310, 131);
    Duplicate_Button(&quickstartOnButton, musicOnButton, 310, 165);
    Duplicate_Button(&quickstartOffButton, musicOffButton, 310, 165);
    Duplicate_Button_TTF(&themeBlackButton, okButton, 310, 199, TX.blackB);
    Duplicate_Button_TTF(&themeWhiteButton, okButton, 422, 199, TX.whiteB);
	Duplicate_Button_TTF(&coversButton, okButton, 310, 265, TX.coversB);
	Duplicate_Button_TTF(&titlesButton, okButton, 422, 265, TX.titlesB);
    Button_Init(&viddownButton, button_minus_png, button_minus_over_png, 310,334);
	Button_Init(&vidupButton, button_plus_png, button_plus_over_png, 338,334);
    Duplicate_Button(&vidtvonButton, musicOnButton, 310, 299);
    Duplicate_Button(&vidtvoffButton, musicOffButton, 310, 299);
    Duplicate_Button(&cheatonButton, musicOnButton, 198,367);
    Duplicate_Button(&cheatoffButton, musicOffButton, 198,367);
    Duplicate_Button(&hookdownButton, viddownButton, 422,367);
    Duplicate_Button(&hookupButton, vidupButton, 450,367);
	Button_Init(&menuLoadingButton, menu_loading_png, menu_loading_png, 310, 414);
    Duplicate_Button_TTF(&cheatDownButton, okButton,310,367,TX.downloadB);
	// Graphic Settings Panel
    Duplicate_Button(&falloffdownButton, viddownButton, 99,123);
    Duplicate_Button(&falloffupButton, vidupButton,127,123);
    Duplicate_Button(&windowdownButton, viddownButton, 195,110);
    Duplicate_Button(&windowupButton, vidupButton, 223, 110);
    Duplicate_Button(&zoomdownButton, viddownButton, 292,110);
    Duplicate_Button(&zoomupButton, vidupButton, 320,110);
    Duplicate_Button(&spacingdownButton, viddownButton, 386,110);
    Duplicate_Button(&spacingupButton, vidupButton, 414,110);
    Duplicate_Button(&angledownButton, viddownButton, 484,123);
    Duplicate_Button(&angleupButton, vidupButton, 512,123);
	
	Duplicate_Button(&falldistdownButton, viddownButton,  99,240);
	Duplicate_Button(&falldistupButton, vidupButton, 127,240);
	Duplicate_Button(&coverzoomdownButton, viddownButton, 292,240);
	Duplicate_Button(&coverzoomupButton, vidupButton, 320,240);
	
    Duplicate_Button(&hidescrollOnButton, musicOnButton, 101,369);
    Duplicate_Button(&hidescrollOffButton, musicOffButton, 101, 369);
    Duplicate_Button(&covers3dOnButton, musicOnButton, 268, 369);
    Duplicate_Button(&covers3dOffButton, musicOffButton, 268, 369);
    Duplicate_Button(&covers3dFatButton, musicOnButton, 268, 423);
    Duplicate_Button(&covers3dThinButton, musicOffButton, 268, 423);
    Duplicate_Button(&coverTextOnButton, musicOnButton, 435, 369);
    Duplicate_Button(&coverTextOffButton, musicOffButton, 435, 369);
	Duplicate_Button_TTF(&resetButton, okButton, 435, 423, TX.resetB);
	Duplicate_Button_TTF(&restoreButton, okButton, 101, 423, TX.restoreB);

// Language Settings Panel
	Duplicate_Button(&langupButton, vidupButton,321,90);
    Duplicate_Button(&langdownButton, viddownButton, 293,90);
	Button_Flag_Init(&flagUSButton, flag_us_png, 108, 150, TX.English); 
	Button_Flag_Init(&flagDEButton, flag_de_png, 233, 150, TX.German);
	Button_Flag_Init(&flagITButton, flag_it_png, 358, 150, TX.Italian); 
	Button_Flag_Init(&flagFRButton, flag_fr_png, 483, 150, TX.French);
	
	Button_Flag_Init(&flagDAButton, flag_da_png, 108, 220, TX.Dutch);
	Button_Flag_Init(&flagESButton, flag_es_png, 233, 220, TX.Spanish); 
	Button_Flag_Init(&flagDKButton, flag_dk_png, 358, 220, TX.Danish); 
	Button_Flag_Init(&flagFIButton, flag_fi_png, 483, 220, TX.Finnish);
	
	Button_Flag_Init(&flagBRButton, flag_br_png, 108, 290, TX.PortugueseBR); 
	Button_Flag_Init(&flagCTButton, flag_ct_png, 233, 290, TX.Catalan); 
	Button_Flag_Init(&flagTRButton, flag_tr_png, 358, 290, TX.Turkish); 
	Button_Flag_Init(&flagHUButton, flag_hu_png, 483, 290, TX.Hungarian); 
	
	Button_Flag_Init(&flagMyLangButton, flag_MyLang_png, 483, 360, TX.MyLanguage);

		
	// Game Settings Screen Buttons
	Button_Init(&lockButton, button_round_lock_png, button_round_lock_over_png, 543, 120 + YOS_GSDB);
	Button_Init(&unlockButton, button_round_unlock_png, button_round_unlock_over_png, 543, 120 + YOS_GSDB);
    Duplicate_Button(&gvidtvonButton, musicOnButton,  360, 164 + YOS_GSDB);
    Duplicate_Button(&gvidtvoffButton, musicOffButton, 360, 164 + YOS_GSDB);
	Duplicate_Button(&gcheatonButton, musicOnButton, 360, 198 + YOS_GSDB);
    Duplicate_Button(&gcheatoffButton, musicOffButton, 360, 198 + YOS_GSDB);
    Duplicate_Button(&ghookdownButton, viddownButton, 360, 232 + YOS_GSDB);
    Duplicate_Button(&ghookupButton, vidupButton, 388, 232 + YOS_GSDB);
    Duplicate_Button(&glangdownButton, viddownButton, 360,266 + YOS_GSDB);
	Duplicate_Button(&glangupButton, vidupButton,388,266 + YOS_GSDB);
    Duplicate_Button(&gviddownButton, viddownButton, 360, 300 + YOS_GSDB);
	Duplicate_Button(&gvidupButton, vidupButton, 388, 300 + YOS_GSDB);
    Duplicate_Button(&gfixdownButton, viddownButton, 360, 334 + YOS_GSDB);
    Duplicate_Button(&gfixupButton, vidupButton, 388, 334 + YOS_GSDB);
	Duplicate_Button_TTF(&gbackButton, okButton, 266, 368 + YOS_GSDB, TX.backB);
  #ifdef CHEAT_MANAGER
    Duplicate_Button_TTF(&manageCheatsButton, okButton, 468, 198 + YOS_GSDB, TX.manageB);

	//cheat manager buttons
    Button_Init(&cheatEnabled[0], button_cheat_on_png, button_cheat_off_png, 44, 80);
    Button_Init(&cheatDisabled[0], button_cheat_off_png, button_cheat_on_png, 44, 80);
    Button_Init(&cheatEditButton[0], button_cheat_edit_png, button_cheat_edit_over_png, 68, 80);
    Duplicate_Button(&pageUpButton, vidupButton,478,82);
    Duplicate_Button(&pageDownButton, viddownButton,450,82);
    Duplicate_Button_TTF(&cheatDoneButton, okButton,84,82,TX.done);
    Duplicate_Button(&selectAllButton, cheatEnabled[0],84,120);
    Duplicate_Button(&deselectAllButton, cheatDisabled[0],84,120);
    Button_Init(&charUpButton[0],button_char_up_png,button_char_up_over_png,100,100);
    Button_Init(&charDownButton[0],button_char_down_png,button_char_down_over_png,100,100);
  #endif
} // End Init_Buttons();

void DrawSlider(int yPos, int theme_id)
{
	int min_loc = 20;
	int max_loc = 296;
	int x = change_scale(self.shift, self.min_cover, self.max_cover, min_loc, max_loc);
	
	if (self.gameCnt < 2)
		x = 155;
	
	slideButton.x = 446 - x;
	slideButton.y = yPos + 16;
	
	switch (theme_id)
	{
		case 0: // black theme
			GRRLIB_DrawImg(120, yPos, slidebar_texture, 0, 1, 1, 0xFFFFFFFF);
			Button_Theme_Paint(&slideButton, settings.theme);
			break;
		case 1: // white theme
			GRRLIB_DrawImg(120, yPos, slidebar_white_texture, 0, 1, 1, 0xFFFFFFFF);
			Button_Theme_Paint(&slideButton, settings.theme);
			break;
		default:
			GRRLIB_DrawImg(120, yPos, slidebar_texture, 0, 1, 1, 0xFFFFFFFF);
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

	if(abs(pos) < settings.falldist)
	{
		loc = settings.coverSpacing * dir * (pow(pos + 1, -1) - 1);
	}
	else
	{
		loc = (settings.coverSpacing * dir * (pow((settings.falldist) + 1, -1) - 1))+(pos-settings.falldist)*((settings.coverSpacing * dir * (pow((settings.falldist) + 1, -1) - 1)) - (settings.coverSpacing * dir * (pow((settings.falldist-1) + 1, -1) - 1)));
	}
	
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
	float zpos = 0.0;

	bool slowTwist = true;

	if (self.twisting)
	{
		if ( abs(self.orient.roll) > 20 )
			slowTwist = false;
	}

	if(abs(pos) < 1)
	{
		zpos = settings.mainCoverZoom-((pos)*settings.mainCoverZoom);
	}
	
	//if (!self.scrolling && slowTwist)
	//{
	//	if (abs(angle) < 45)
	//		zpos = easeOutCubic( abs(angle), 1.9, -1.9, 45);
	//	else if ((abs(angle) >135) && (abs(angle) <= 180))
	//		zpos = easeOutCubic( (180 - abs(angle)), -1.9, 1.9, 45);
	//	else if ((abs(angle) >180) && (abs(angle) <= 225))
	//		zpos = easeOutCubic( (225 - abs(angle)), 0, -1.9, 45);
	//}

	// Draw the cover
	DrawBufferedCover(texture_id, loc, zpos,  angle, falloff);
}

void DrawCoverFlyInStart()
{
	float zEnd = settings.coverZoom;
	float yEnd = 0.0;
	float yEndTitle = 410.0;
	float yEndSlider = 410.0; 
	float yEndButtons = 427.0;
	float yEndButtonInfo =44.0;
	int   q;
	int tFade;
	float moving_y;
	for (q=0;q<=192;q++)
	{
		// Calc the fly in using Penner easing equation
		settings.coverZoom = easeOutQuint(q, zEnd-10, 10.0, 192);
		settings.coverCamY = easeOutQuint(q, yEnd+3, -3.0, 192);
		// Draw the covers
		draw_covers();
		// Flip to 2D and draw the bottom items
		GRRLIB_2D_Init();
		// Float in the buttons
		if(!settings.parentalLock)
		{
			moving_y = easeOutQuint(q, yEndButtons + 70, -70.0, 192);
			addButton.y = moving_y;
			settingsButton.y = moving_y;
			Button_Theme_Paint(&addButton, settings.theme);
			Button_Theme_Paint(&settingsButton, settings.theme);
		}
		// Float in InfoButton
		moving_y = easeOutQuint(q, yEndButtonInfo - 70, +70.0, 192);
		infoButton.y = moving_y;
		Button_Theme_Paint(&infoButton, settings.theme);
		// Float in the game title
		if(settings.coverText)
		{
			moving_y = easeOutQuint(q, yEndTitle + 70, -70.0, 192);
			draw_game_title(moving_y, self.gameSelected, 1.0);
		}
		// Float in the slider
		if(settings.hideScroll)
		{
			moving_y = easeOutQuint(q, yEndSlider + 70, -70.0, 192);
			DrawSlider(moving_y, settings.theme);
		}
		// Draw the fade mask
		tFade = (255 - (q * 2));
		if(tFade < 0) tFade = 0;
		if (settings.theme)
			GRRLIB_FillScreen(0xCFCFCF00|tFade);
		else
			GRRLIB_FillScreen(0x00000000|tFade);
		WPAD_ScanPads();
		GetWiimoteData();
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		GRRLIB_Render();
		// Update the images while flying in
		UpdateBufferedImages();
	}
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

void draw_game_title(int yPos, int index, float textSize)
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
			CFreeTypeGX_DrawText(ttf20pt, 320, yPos,  gameName, (GXColor){0x11, 0x11, 0x11, 0xff}, FTGX_JUSTIFY_CENTER);
		else
			CFreeTypeGX_DrawText(ttf20pt, 320, yPos,  gameName, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_CENTER);
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
			char str[30];
			getSysdate(gameSetting.lastPlayed, str);
			//sprintf(tTemp, TX.played, gameSetting.lastPlayed);
			sprintf(tTemp, TX.played, str);
			CFreeTypeGX_DrawText(ttf16pt, 270, 253, tTemp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
		}
	
		sprintf(tTemp, TX.size, self.gsize);
		CFreeTypeGX_DrawText(ttf16pt, 270, 278, tTemp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
		
		sprintf(tTemp, "GameID:  %s", (char*)header->id);
		CFreeTypeGX_DrawText(ttf16pt, 270, 303, tTemp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
		
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
		// TODO: This still needs some work on the transition
		float zpos = 0.0;
		zpos = change_scale( abs(angle), 0, 90, 1.9, 0);
		DrawBufferedCover(self.gameSelected, loc, zpos, angle, 0);
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
	if(choice_a != 0)
		choice_a->hovering = false;
	if(choice_b != 0)
		choice_b->hovering = false;
	/////////////////////////////////////
	// Draw the intro - lower the dialog
	int i = 1;
	int fade = 0x00;
	int slidetime = 30;
	float moving_y;
	
	for(i = 0; i <= slidetime; i++)
	{
		// Draw the covers in the back
		draw_covers();
		GRRLIB_2D_Init();
		// Draw the screen fade
		GRRLIB_FillScreen(0x00000000|fade);
		fade+=5;
		// Draw the background panel
		moving_y = easeOutQuad(i, 148 - 350, +350.0, slidetime);
		GRRLIB_Rectangle(108, moving_y, 424, 184, 0xffffffdd, true);
		moving_y = easeOutQuad(i, 150 - 350, +350.0, slidetime);
		GRRLIB_Rectangle(110, moving_y, 420, 180, 0x737373FF, true);
		moving_y = easeOutQuad(i, 140 - 350, +350.0, slidetime);
		GRRLIB_DrawImg(90, moving_y, dialog_box_titlebar_long_texture, 0, 1, 1, 0xFFFFFFFF);
		// Draw buttons
		moving_y = easeOutQuad(i, 290 - 350, +350.0, slidetime);
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
		moving_y = easeOutQuad(i, 195 - 350, +350.0, slidetime);
		GRRLIB_DrawImg(130, moving_y, dialog_box_icon_texture, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_DrawImgReflection(130, moving_y + dialog_box_icon_texture.h + 4, dialog_box_icon_texture, 0, 1, 1, 0.7);
		// Draw Dialog Box Title Text
		moving_y = easeOutQuad(i, 153 - 350, +350.0, slidetime);
		CFreeTypeGX_DrawText(ttf16pt, 215, moving_y, title, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		// Draw Dialog Box Body Text
		moving_y = easeOutQuad(i, 190 - 350, +350.0, slidetime);
		int lsp = moving_y;
		if(txt != NULL)
		{
			char* msg = malloc(strlen(txt)*sizeof(char));
			sprintf(msg, txt);
			
			pch = strtok(msg, "|\n");
			while (pch != NULL)
			{
				CFreeTypeGX_DrawText(ttf16pt, 220, lsp, pch, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				pch  = strtok(NULL, "|\n");
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
			
			pch = strtok(msg, "|\n");
			while (pch != NULL)
			{
				CFreeTypeGX_DrawText(ttf16pt, 220, y+sp, pch, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				pch = strtok(NULL, "|\n");
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
	for(i = 0; i <= slidetime; i++)
	{
		// Draw the covers in the back
		draw_covers();
		GRRLIB_2D_Init();
		// Draw the screen fade
		GRRLIB_FillScreen(0x00000000|fade);
		fade-=5;
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
			
			pch = strtok(msg, "|\n");
			while (pch != NULL)
			{
				CFreeTypeGX_DrawText(ttf16pt, 220, lsp, pch, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				pch  = strtok(NULL, "|\n");
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
	WPAD_Rumble(0,0);
	return returnVal;
}

int WindowPromptInstall(char* id,char* title, char* txt, struct Button* choice_a, struct Button* choice_b, int option)
{
	bool returnVal = false;
	bool doloop = true;
	char* pch;
    GRRLIB_texImg myTex;
	unsigned char buffer[160 * 224 * 4 * 10];
	char filepath[255];
	int ret;
	
	sprintf(filepath, USBLOADER_PATH "/covers/%s.png", id);
	
	
	WPAD_Rumble(0,0); //sometimes rumble remain active
	
	if(option == 0){
		if(networkInit(self.ipAddress))
		{
			Download_Cover(id, 1, self.gameCnt);
			//CoversDownloaded();
			ret = Fat_ReadFileToBuffer(filepath, (void*)buffer, 160 * 224 * 4);
			if(ret > 0)
			{
				GRRLIB_LoadTexturePNG(&myTex,buffer);
			}
			else
			{
				//myTex = cover_texture;//GRRLIB_LoadTexture(no_cover_png);
				GRRLIB_DuplicateTexture(&myTex, cover_texture, cover_texture.w, cover_texture.h);
			}
		}
		else // no network
		{
			//myTex = cover_texture;//GRRLIB_LoadTexture(no_cover_png);
			GRRLIB_DuplicateTexture(&myTex, cover_texture, cover_texture.w, cover_texture.h);
		}
	}
	else
	{
		ret = Fat_ReadFileToBuffer(filepath, (void*)buffer, 160 * 224 * 4);
		if(ret > 0)
		{
			GRRLIB_LoadTexturePNG(&myTex,buffer);
		}
		else
		{
			//myTex = cover_texture;//GRRLIB_LoadTexture(no_cover_png);
			GRRLIB_DuplicateTexture(&myTex, cover_texture, cover_texture.w, cover_texture.h);
		}
	}
	
	if(choice_a == 0 && choice_b == 0)
		doloop = false;
	if(choice_a != 0)
		choice_a->hovering = false;
	if(choice_b != 0)
		choice_b->hovering = false;
	
	/////////////////////////////////////
	// Draw the intro - lower the dialog
	int i = 1;
	int fade = 0x00;
	int slidetime = 30;
	float moving_y;

	for(i = 0; i <= slidetime; i++)
	{
		// Draw the covers in the back
		draw_covers();
		GRRLIB_2D_Init();
		// Draw the screen fade
		GRRLIB_FillScreen(0x00000000|fade);
		fade+=5;
		// Draw the background panel
		moving_y = easeOutQuad(i, 148 - 350, +350.0, slidetime);
		GRRLIB_Rectangle(108, moving_y, 424, 184, 0xffffffdd, true);
		moving_y = easeOutQuad(i, 150 - 350, +350.0, slidetime);
		GRRLIB_Rectangle(110, moving_y, 420, 180, 0x737373FF, true);
		moving_y = easeOutQuad(i, 140 - 350, +350.0, slidetime);
		GRRLIB_DrawImg(90, moving_y, dialog_box_titlebar_long_texture, 0, 1, 1, 0xFFFFFFFF);
		// Draw the cover image
		moving_y = easeOutQuad(i, 133 - 350, +350.0, slidetime);
		if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
			GRRLIB_DrawImg(85, moving_y, myTex, 0, (AR_16_9)*0.7, 0.7, 0xFFFFFFFF);
		else
			GRRLIB_DrawImg(85, moving_y, myTex, 0, 0.7, 0.7, 0xFFFFFFFF);
		// Draw buttons
		moving_y = easeOutQuad(i, 290 - 350, +350.0, slidetime);
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
		moving_y = easeOutQuad(i, 153 - 350, +350.0, slidetime);
		CFreeTypeGX_DrawText(ttf16pt, 215, moving_y, title, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		// Draw Dialog Box Body Text
		moving_y = easeOutQuad(i, 190 - 350, +350.0, slidetime);
		int lsp = moving_y;
		if(txt != NULL)
		{
			char* msg = malloc(strlen(txt)*sizeof(char));
			sprintf(msg, txt);

			pch = strtok(msg, "|\n");
			while (pch != NULL)
			{
				CFreeTypeGX_DrawText(ttf16pt, 220, lsp, pch, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				pch  = strtok(NULL, "|\n");
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

			pch = strtok(msg, "|\n");
			while (pch != NULL)
			{
				CFreeTypeGX_DrawText(ttf16pt, 220, y+sp, pch, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				pch = strtok(NULL, "|\n");
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
	for(i = 0; i <= slidetime; i++)
	{
		// Draw the covers in the back
		draw_covers();
		GRRLIB_2D_Init();
		// Draw the screen fade
		GRRLIB_FillScreen(0x00000000|fade);
		fade-=5;
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

			pch = strtok(msg, "|\n");
			while (pch != NULL)
			{
				CFreeTypeGX_DrawText(ttf16pt, 220, lsp, pch, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				pch  = strtok(NULL, "|\n");
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

	WPAD_Rumble(0,0);
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
	
	/*TODO Draw Window*/
	_title = title;
	_msg   = msg;
	
	int ret = wbfs_add_disc(hdd, __WBFS_ReadDVD, NULL, ShowProgress, ONLY_GAME_PARTITION, 0);
	//int ret = wbfs_add_disc(hdd, __WBFS_ReadDVD, NULL, NULL, ONLY_GAME_PARTITION, 0);
	
	self.progress = 0.0;
	
	//Resume Buffer Thread
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
	//free(cover_texture.data);
	//free(cover_texture_3d.data);
	free(no_disc_texture.data);
	free(slidebar_texture.data);
	free(slidebar_white_texture.data);
	//free(ambientlight_texture.data);
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
	free(menu_loading_texture.data);
	free(dialog_box_titlebar_texture.data);
	free(dialog_box_titlebar_long_texture.data);
	free(dialog_box_icon_texture.data);
	free(menu_graphics_wireframe_texture.data);
	free(menu_graphics_box1_texture.data);
	free(menu_graphics_box2_texture.data);
	free(flag_br_texture.data);
	free(flag_da_texture.data);
	free(flag_de_texture.data);
	free(flag_fr_texture.data);
	free(flag_it_texture.data);
	free(flag_dk_texture.data);
	free(flag_tr_texture.data);
	free(flag_us_texture.data);
	free(flag_es_texture.data);
	free(flag_ct_texture.data);
	free(flag_fi_texture.data);
	free(flag_hu_texture.data);
	free(flag_MyLang_texture.data);
	//if (coverLoaded)free(current_cover_texture.data); // now in mem2
	free(hdspace_texture.data);
	
	//buttons
	FreeButtonResources(&slideButton);
	FreeButtonResources(&settingsButton);
	FreeButtonResources(&addButton);
	FreeButtonResources(&infoButton);
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
    //FreeButtonResources(&rumbleOnButton);
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
	FreeButtonResources(&menuLoadingButton);
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
	FreeButtonResources(&falldistupButton);
	FreeButtonResources(&falldistdownButton);
	FreeButtonResources(&coverzoomupButton);
	FreeButtonResources(&coverzoomdownButton);
	
    FreeButtonResources(&hidescrollOnButton);
    FreeButtonResources(&hidescrollOffButton);
    FreeButtonResources(&covers3dThinButton);
    FreeButtonResources(&covers3dFatButton);
    FreeButtonResources(&covers3dOnButton);
    FreeButtonResources(&covers3dOffButton);
    FreeButtonResources(&coverTextOnButton);
    FreeButtonResources(&coverTextOffButton);
	FreeButtonResources(&resetButton);
	FreeButtonResources(&restoreButton);
	// Language Settings Panel
	FreeButtonResources(&langupButton);
    FreeButtonResources(&langdownButton);
	FreeButtonResources(&flagUSButton);
	FreeButtonResources(&flagITButton);
	FreeButtonResources(&flagDEButton);
	FreeButtonResources(&flagTRButton);
	FreeButtonResources(&flagBRButton);
	FreeButtonResources(&flagFRButton);
	FreeButtonResources(&flagDAButton);
	FreeButtonResources(&flagFIButton);
	FreeButtonResources(&flagDKButton);
	FreeButtonResources(&flagESButton);
	FreeButtonResources(&flagCTButton);
	FreeButtonResources(&flagHUButton);
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
	FreeButtonResources(&gfixdownButton);
	FreeButtonResources(&gfixupButton);
	FreeButtonResources(&charUpButton[0]);
	FreeButtonResources(&charDownButton[0]);
		
 #ifdef CHEAT_MANAGER
        int n;
        for(n=0;n<LINES_PER_PAGE;n++) //create the buttons
        {
            FreeButtonResources(&cheatEnabled[n]);
            FreeButtonResources(&cheatDisabled[n]);
            FreeButtonResources(&cheatEditButton[n]);
        }
	
	FreeButtonResources(&manageCheatsButton);
	FreeButtonResources(&pageUpButton);
    FreeButtonResources(&pageDownButton);
    FreeButtonResources(&cheatDoneButton);
    FreeButtonResources(&selectAllButton);
    FreeButtonResources(&deselectAllButton);
    FreeButtonResources(&cheatDownButton);
 #endif

 //  CFreeTypeGX_delete function make the games don't boot, removed temporary (it wass addedd in r533)

	CFreeTypeGX_delete(ttf14pt);
    CFreeTypeGX_delete(ttf16pt);
	CFreeTypeGX_delete(ttf18pt);
	CFreeTypeGX_delete(ttf20pt);
	CFreeTypeGX_delete(ttf24pt);
	
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



void Label_Buttons()
{
	strcpy( okButton.ttf_label, TX.okB);
	strcpy( cancelButton.ttf_label, TX.cancelB);	
	strcpy( yesButton.ttf_label, TX.yesB);
	strcpy( noButton.ttf_label, TX.noB);	
	strcopy( resetButton.ttf_label, TX.resetB, 15);
	strcopy( restoreButton.ttf_label, TX.restoreB, 15);
	
	strcopy( covers3dFatButton.ttf_label, TX.fatB, 15);
	strcopy( covers3dThinButton.ttf_label, TX.thinB, 15);
	
	strcpy( loadButton.ttf_label, TX.loadB);
	strcpy( backButton.ttf_label, TX.backB);	
	strcpy( gbackButton.ttf_label, TX.backB);
	
	strcpy( wiiMenuButton.ttf_label, TX.wiimenuB);	
	strcpy( loaderButton.ttf_label, TX.loaderB);
	
	strcopy( menuSettingsButton.ttf_label, TX.cflowSettings, 15);	
	strcopy( menuGraphicsButton.ttf_label, TX.graphics, 15);
	strcpy( menuLanguagesButton.ttf_label, TX.languagesB);	
	
	strcpy( themeBlackButton.ttf_label, TX.blackB);
	strcpy( themeWhiteButton.ttf_label, TX.whiteB);	
	strcpy( coversButton.ttf_label, TX.coversB);
	strcpy( titlesButton.ttf_label, TX.titlesB);	

	strcopy( flagUSButton.ttf_label, TX.English, 15);
	strcopy( flagITButton.ttf_label, TX.Italian, 15);	
	strcopy( flagDEButton.ttf_label, TX.German, 15);
	strcopy( flagTRButton.ttf_label, TX.Turkish, 15);	
	strcopy( flagBRButton.ttf_label, TX.PortugueseBR, 15);
	strcopy( flagFRButton.ttf_label, TX.French, 15);	
	strcopy( flagDAButton.ttf_label, TX.Dutch, 15);
	strcopy( flagFIButton.ttf_label, TX.Finnish, 15);	
	strcopy( flagDKButton.ttf_label, TX.Danish, 15);
	strcopy( flagESButton.ttf_label, TX.Spanish, 15);	
	strcopy( flagCTButton.ttf_label, TX.Catalan, 15);
	strcopy( flagHUButton.ttf_label, TX.Hungarian, 15);
	strcopy( flagMyLangButton.ttf_label, TX.MyLanguage, 15);
 #ifdef CHEAT_MANAGER
	strcopy( manageCheatsButton.ttf_label, TX.manageB, 15);
 #endif	
}


