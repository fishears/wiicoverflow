#include "homemenu.h"

#include "gfx.h"

extern s_self self;
extern s_pointer pointer;

inline void HomeMenu_Init()
{
	//Nothing to do
}


inline void HomeMenu_Show()
{

	bool doloop = true;

	int slide_wiimote = 0;

	//animate Slide
	homeMenuTopButton.x = 0;
	homeMenuTopButton.y = -112;
	homeMenuBottomButton.x = 0;
	homeMenuBottomButton.y = 480;
	
	int i = 0;
	for(i = 0; i <= 28; i++)
	{
		homeMenuTopButton.y += 4;
		homeMenuBottomButton.y -= 4;
		
		draw_covers();
		GRRLIB_2D_Init();

		Button_Paint(&homeMenuTopButton);
		Button_Paint(&homeMenuBottomButton);
		GRRLIB_Render();
	}
	
	wiiMenuButton.x = 34-400;
	wiiMenuButton.y = 180;
	loaderButton.x  = 174-400;
	loaderButton.y = 180;
	
	wiimoteButton.x = 64;
	wiimoteButton.y = 400 + 400;
	for(i = 0; i <= 20; i++)
	{
		wiiMenuButton.x += 20;
		loaderButton.x  += 20;
		wiimoteButton.y -= 20;
		
		draw_covers();
		GRRLIB_2D_Init();
		
		Button_Paint(&homeMenuTopButton);
		Button_Paint(&homeMenuBottomButton);
		
		Button_Paint(&wiiMenuButton);
		Button_Paint(&loaderButton);
		
		Button_Paint(&wiimoteButton);
		
		GRRLIB_Render();
	}


	/*Render and control Settings*/
	do{

		WPAD_ScanPads();
		WPAD_IR(WPAD_CHAN_0, &self.ir); // Let's get our infrared data
		WPAD_Orientation(WPAD_CHAN_0, &self.orient);
		wd = WPAD_Data(WPAD_CHAN_0);

		pointer.p_x = self.ir.sx-200;
		pointer.p_y = self.ir.sy-250;
		pointer.p_ang = self.ir.angle/2; // Set angle/2 to translate correctly

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			WPAD_Rumble(0,0); // Kill the rumble
			doloop = false;
		}
			
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
		{
			if (Button_Select(&homeMenuTopButton, pointer.p_x, pointer.p_y))
			{
				WPAD_Rumble(0,0); // Kill the rumble
				doloop = false;
			}
			else if (Button_Select(&wiiMenuButton, pointer.p_x, pointer.p_y))
			{
				WPAD_Rumble(0,0); // Kill the rumble
				SYS_ResetSystem(SYS_RETURNTOMENU,0,0);
			}
			else if (Button_Select(&loaderButton, pointer.p_x, pointer.p_y))
			{
				WPAD_Rumble(0,0); // Kill the rumble
				exit(1);        // eventually, return a value.
			}
			else if (Button_Select(&homeMenuBottomButton, pointer.p_x, pointer.p_y))
			{
				//TODO Show control Screen
			}
		}
		
		/*Draw Covers*/ //PREVIEW
		draw_covers();
		// Draw menu dialog background
		GRRLIB_2D_Init();

		//Button_Theme_Paint(&settingsButton, SETTING_theme);
		Button_Paint(&homeMenuTopButton);
		Button_Paint(&homeMenuBottomButton);
		Button_Paint(&wiiMenuButton);
		Button_Paint(&loaderButton);
		Button_Paint(&wiimoteButton);
	
		// Check for button-pointer intersections, and rumble
		if (Button_Hover(&homeMenuTopButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&homeMenuBottomButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&wiiMenuButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&loaderButton, pointer.p_x, pointer.p_y))
		{
			// Should we be rumbling?
			if (--self.rumbleAmt > 0)
			{
				if(SETTING_rumble)
					WPAD_Rumble(0,1); // Turn on Wiimote rumble
			}
			else 
				WPAD_Rumble(0,0); // Kill the rumble
		}
		else
		{ // If no button is being hovered, kill the rumble
			WPAD_Rumble(0,0);
			self.rumbleAmt = 4;
		}

		if(Button_Hover(&homeMenuBottomButton, pointer.p_x, pointer.p_y))
		{
			if(slide_wiimote > -20)
			{
				slide_wiimote--;
				wiimoteButton.y -=1;
			}
		}
		else
		{
			if(slide_wiimote < 0)
			{
				slide_wiimote++;
				wiimoteButton.y += 1;
			}
		}

		// Draw the pointer hand
		GRRLIB_DrawImg(pointer.p_x, pointer.p_y, pointer_texture, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		// Spit it out
		GRRLIB_Render();
		
	}while(doloop);
	
	for(i = 0; i <= 20; i++)
	{
		wiiMenuButton.x -= 20;
		loaderButton.x  -= 20;
		wiimoteButton.y += 20;
		
		draw_covers();
		
		GRRLIB_2D_Init();
		
		Button_Paint(&homeMenuTopButton);
		Button_Paint(&homeMenuBottomButton);
		
		Button_Paint(&wiiMenuButton);
		Button_Paint(&loaderButton);
		
		Button_Paint(&wiimoteButton);
		
		GRRLIB_Render();
	}
	
	for(i = 0; i <= 28; i++)
	{
		homeMenuTopButton.y -= 4;
		homeMenuBottomButton.y += 4;
		
		draw_covers();
		
		GRRLIB_2D_Init();

		Button_Paint(&homeMenuTopButton);
		Button_Paint(&homeMenuBottomButton);
		GRRLIB_Render();
	}
	
}

inline void HomeMenu_Destroy()
{
	//Nothing to do...
}
