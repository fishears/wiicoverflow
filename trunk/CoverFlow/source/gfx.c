#include "gfx.h"

extern s_self self;

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
	
        cheatonButton = Button_Init(yes_png, yes_hover_png, 350,95);
        cheatoffButton = Button_Init(no_png, no_hover_png, 350,95);

        langupButton = Button_Init(plus_button_png, plus_button_hover_png,456,138);
        langdownButton = Button_Init(minus_button_png, minus_button_hover_png, 300,138);
	
	yesButton  = Button_Init(yes_png, yes_hover_png, 220, 250);
	noButton   = Button_Init(no_png, no_hover_png, 340, 250);
	
	settingsButton = Button_Init(settings_png, settings_hover_png, 30, 420);
}

void Hover_Buttons()
{
	Button_Hover(&addButton, self.p_x, self.p_y);
	Button_Hover(&okButton, self.p_x, self.p_y);
	Button_Hover(&slideButton, self.p_x, self.p_y);
	Button_Hover(&loadButton, self.p_x, self.p_y);
	Button_Hover(&backButton, self.p_x, self.p_y);
	Button_Hover(&cancelButton, self.p_x, self.p_y);
	Button_Hover(&yesButton, self.p_x, self.p_y);
	Button_Hover(&noButton, self.p_x, self.p_y);
	Button_Hover(&deleteButton, self.p_x, self.p_y);
	Button_Hover(&settingsButton, self.p_x, self.p_y);
        Button_Hover(&langupButton, self.p_x, self.p_y);
        Button_Hover(&langdownButton, self.p_x, self.p_y);
	
	if(ocarinaChoice)
		Button_Hover(&cheatonButton, self.p_x, self.p_y);
	else
		Button_Hover(&cheatoffButton, self.p_x, self.p_y);

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
	  {
		return;
	  }

	  static const float SPACING = 2.8;
	  float dir = 1;
	  float loc, scale, angle;

	  if (pos < 0) {
		dir *= -1;
		pos *= -1;
	  }
	  
	  loc = SPACING * dir * (pow(pos + 1, -1) - 1);
	  scale = pow(pos + 1, -2);
	  angle = -1 * dir * change_scale(scale, 0, 1, 90, 0);
	
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
		if(abs(self.shift+i) < DRAW_WINDOW)
			GRRLIB_Cover(i+self.shift, i+(COVER_COUNT/2.0));
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
