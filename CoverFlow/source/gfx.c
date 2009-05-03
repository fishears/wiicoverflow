#include "gfx.h"

#include "settings.h"

extern s_self self;
extern s_pointer pointer;

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

void DrawBufferedCover(int i, float loc, float angle)
{
	if(i < MAX_BUFFERED_COVERS || i >= 0)
	{
		if(BUFFER_IsCoverReady(i))
		{
			pthread_mutex_lock(&buffer_mutex[i]);
			if(_texture_data[i].data)
			{
				GRRLIB_DrawCoverImg(loc*1.2,_texture_data[i],angle,1.0,0xFFFFFFFF);
			}
			else
			{
				GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF);
			}
			pthread_mutex_unlock(&buffer_mutex[i]);
		}
		else
		{
			GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF);
		}	
	}
	else
	{
		GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF);
	}	

}

void Paint_Progress(float v, char* msg)
{
	int i;
	int count = (int)(v*10);
	
	if(count > 40)
		count = 40;
	
	GRRLIB_DrawImg(0, 0, gradient_texture, 0, 1, 1, 0xFFFFFFFF);
	
	for(i = 0; i < count; i++)
	{
		GRRLIB_DrawImg(165+12*i, 231, progress_texture, 0, 1, 1, 0xFFFFFFFF);
	}

	GRRLIB_DrawImg(0, 0, loader_main_texture, 0, 1, 1, 0xFFFFFFFF);
	
	#ifdef DEBUG
	if(msg != NULL)
		GRRLIB_Printf(160, 255, tex_BMfont5,  0x444444FF, 1, "%s", msg);
    #endif
    
	GRRLIB_Render();
}

void Init_Buttons()
{

    addButton   = Button_Init(add_button_png, add_button_hover_png, 580, 417);
	slideButton = Button_Init(slide_png,  slide_hover_png, 580, 400);
	okButton    = Button_Init(ok_png,   ok_hover_png, 220, 250);
	loadButton  = Button_Init(load_png,   load_hover_png, 220, 300);
	deleteButton  = Button_Init(delete_png,   delete_hover_png, 220, 400);
	backButton  = Button_Init(back_png,   back_hover_png, 340, 300);
	cancelButton = Button_Init(cancel_png, cancel_hover_png, 340, 250);
	
	cheatonButton = Button_Init(toggle_on_png, toggle_on_png, 350,95);
	cheatoffButton = Button_Init(toggle_off_png, toggle_off_png, 350,95);

	langupButton = Button_Init(plus_button_png, plus_button_hover_png,456,138);
	langdownButton = Button_Init(minus_button_png, minus_button_hover_png, 300,138);

	toggleOnButton = Button_Init(toggle_on_png, toggle_on_png, 350, 180);
    toggleOffButton = Button_Init(toggle_off_png, toggle_off_png, 350, 180);
	
    graphicsButton = Button_Init(ok_png, ok_hover_png, 350, 180);
	
	//toggleOnButton = Button_Init(toggle_on_png, toggle_on_png, 350, 180);
    //toggleOffButton = Button_Init(toggle_off_png, toggle_off_png, 350, 180);
	
	yesButton  = Button_Init(yes_png, yes_hover_png, 220, 250);
	noButton   = Button_Init(no_png, no_hover_png, 340, 250);
	
	settingsButton = Button_Init(settings_png, settings_hover_png, 30, 420);
	
	downloadButton = Button_Init(download_png, download_hover_png, 350, 240);
	
	spacingupButton = Button_Init(plus_button_png, plus_button_hover_png,456,138);
	spacingdownButton = Button_Init(minus_button_png, minus_button_hover_png, 300,138);
	
	zoomupButton = Button_Init(plus_button_png, plus_button_hover_png,456,95);
	zoomdownButton = Button_Init(minus_button_png, minus_button_hover_png, 300,95);
	
	angleupButton = Button_Init(plus_button_png, plus_button_hover_png,456,191);
	angledownButton = Button_Init(minus_button_png, minus_button_hover_png, 300,191);
	
	windowupButton = Button_Init(plus_button_png, plus_button_hover_png,456, 239);
	windowdownButton = Button_Init(minus_button_png, minus_button_hover_png, 300,239);
}

void Hover_Buttons()
{
	Button_Hover(&addButton, pointer.p_x, pointer.p_y);
	Button_Hover(&okButton, pointer.p_x, pointer.p_y);
	Button_Hover(&slideButton, pointer.p_x, pointer.p_y);
	Button_Hover(&loadButton, pointer.p_x, pointer.p_y);
	Button_Hover(&backButton, pointer.p_x, pointer.p_y);
	Button_Hover(&cancelButton, pointer.p_x, pointer.p_y);
	Button_Hover(&yesButton, pointer.p_x, pointer.p_y);
	Button_Hover(&noButton, pointer.p_x, pointer.p_y);
	Button_Hover(&deleteButton, pointer.p_x, pointer.p_y);
	Button_Hover(&settingsButton, pointer.p_x, pointer.p_y);
    Button_Hover(&langupButton, pointer.p_x, pointer.p_y);
    Button_Hover(&langdownButton, pointer.p_x, pointer.p_y);
	Button_Hover(&downloadButton, pointer.p_x, pointer.p_y);
	Button_Hover(&graphicsButton, pointer.p_x, pointer.p_y);
	
    Button_Hover(&spacingupButton, pointer.p_x, pointer.p_y);
    Button_Hover(&spacingdownButton, pointer.p_x, pointer.p_y);
	
    Button_Hover(&zoomupButton, pointer.p_x, pointer.p_y);
    Button_Hover(&zoomdownButton, pointer.p_x, pointer.p_y);
	
    Button_Hover(&angleupButton, pointer.p_x, pointer.p_y);
    Button_Hover(&angledownButton, pointer.p_x, pointer.p_y);
	
    Button_Hover(&windowupButton, pointer.p_x, pointer.p_y);
    Button_Hover(&windowdownButton, pointer.p_x, pointer.p_y);
	
	if(CFG.ocarina)
		Button_Hover(&cheatonButton, pointer.p_x, pointer.p_y);
	else
		Button_Hover(&cheatoffButton, pointer.p_x, pointer.p_y);

}

void DrawSlider()
{
	int min_loc = 0;
	int max_loc = 313;
	
	GRRLIB_DrawImg(120, 410, slide_bar_texture, 0, 1, 1, 0xFFFFFFFF);
	
	int x = change_scale(self.shift, -1*(COVER_COUNT/2.0), COVER_COUNT/2.0, min_loc, max_loc);
	
	slideButton.x = 126+x;
	slideButton.y = 426;
	
	//GRRLIB_DrawImg(126+x, 426, slide_texture, 0, 1, 1, 0xFFFFFFFF);
	
	Button_Paint(&slideButton);
}

void GRRLIB_Cover(float pos, int texture_id)
{
	if(pos == 0)
		self.gameSelected = texture_id;

	if((self.selected || self.animate_flip > 0) && pos == 0)
		return;

	//static const float SPACING = 2.8;
	float dir = 1;
	float loc, scale, angle;

	if (pos < 0) {
		dir *= -1;
		pos *= -1;
	}

	loc = SETTING_coverSpacing * dir * (pow(pos + 1, -1) - 1);
	scale = pow(pos + 1, -2);
	angle = -1 * dir * change_scale(scale, 0, 1, SETTING_coverAngle, 0);

	#ifdef BUFFER_TEST
	DrawBufferedCover(texture_id, loc, angle);
	#else

	if(texture_id != -1 && texture_id < self.array_size)
		GRRLIB_DrawCoverImg(loc*1.2,covers[texture_id],angle,1.0,0xFFFFFFFF);
	else
		GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF);
	#endif
}

void draw_covers()
{
	int i;
	
	for(i = (-1*(COVER_COUNT/2.0)); i < (COVER_COUNT/2.0); i++)
	{
		/*Some logic to avoid drawing everything*/
		if(abs(self.shift+i) < SETTING_drawWindow)
			GRRLIB_Cover(i+self.shift, i+(COVER_COUNT/2.0));
	}
}

void draw_game_title(int index, struct discHdr *gameList)
{
	if(index != -1)
	{
		float tsize = 1;
		int len = 0;
		struct discHdr *header = NULL;
		
		header = &gameList[index];
		len = strlen(header->title);
		
		int offset = (len*5);
		
		if(offset > 240)
			offset = 240;
			
		GRRLIB_Printf(340 - offset, 400, tex_BMfont5, 0xFFFFFFFF, tsize, "%s", header->title);
	}
}

void draw_selected(struct discHdr *gameList)
{
	
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
	
	
//	static const float SPACING = 2.8;
	float dir = 1;
	float loc, scale, angle;

	loc = SETTING_coverSpacing * dir * (pow(1, -1) - 1);
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
			
			self.animate_rotate++;
			
			if(self.animate_rotate == 360) self.animate_rotate = 0;
			
			GRRLIB_DrawImg(230,100, current_cover_texture, self.animate_rotate, 1, 1, 0xFFFFFFFF);
			
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
			
			if(offset > 240)
				offset = 240;
			
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
		GRRLIB_DrawCoverImg(loc*1.2,covers[self.gameSelected],angle,1.0,0xFFFFFFFF);
	else
		GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF);
	#endif
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

		pointer.p_x = ir.sx-200;
		pointer.p_y = ir.sy-250;
		pointer.p_ang = ir.angle/2; // Set angle/2 to translate correctly

		Hover_Buttons();

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			//quit();
			exit(0);
		}
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
		{
			return false;
		}
		
		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
		{
			if(choice_a != 0)
			{
				if(Button_Select(choice_a, pointer.p_x, pointer.p_y))
				{
					return true;
				}
			}
			
			if(choice_b != 0)
			{
				if(Button_Select(choice_b, pointer.p_x, pointer.p_y))
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
		
		
		if(choice_a != 0 && choice_b != 0)
		{
			Button_Paint(choice_a);
			Button_Paint(choice_b);
		}
		else if(choice_a  == 0)
		{
			choice_b->x = 320-40;
			Button_Paint(choice_b);
		}
		else
		{
			choice_a->x = 320-40;
			Button_Paint(choice_a);
		}
		
		/*Draw Text*/
        GRRLIB_Printf(140, 70, tex_BMfont5,  0xFFFFFFFF, 1.5, "%s", title);
        GRRLIB_Printf(160, 110, tex_BMfont5, 0xFFFFFFFF, 1, "%s", txt);
		
		if(doloop)
			GRRLIB_DrawImg(pointer.p_x, pointer.p_y, pointer_texture, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		
		GRRLIB_Render();
		
	}while(doloop);
	
	return false;
}
