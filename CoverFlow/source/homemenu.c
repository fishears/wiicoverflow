#include "homemenu.h"

#include "gfx.h"

extern s_self self;
extern s_pointer pointer;

extern u8 shutdown;
extern u8 reset;
extern s_settings settings;
inline void HomeMenu_Init()
{
	//Nothing to do
}


void launchTitle(u64 titleID, int need_sys)
{
	//int retval;
	//if(need_sys)
	//	SystemMenuAuth();

	u32 cnt ATTRIBUTE_ALIGN(32);
	if(ES_GetNumTicketViews(titleID, &cnt)<0)
		return;

	tikview *views = (tikview *)memalign( 32, sizeof(tikview)*cnt );

	if(ES_GetTicketViews(titleID, views, cnt)<0)
	{
		free(views);
		return;
	}

	if(ES_LaunchTitle(titleID, &views[0])<0)
	{
		free(views);
		return;
	}
}

void exitToSystemMenu() 
{
	WPAD_Shutdown();
	usleep(500); 
	SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);	
}

void HomeMenu_Show()
{
	bool doloop = true;

	int slide_wiimote = 0;

	//animate Slide
	homeMenuTopButton.x = 0;
	homeMenuTopButton.y = -112;
	homeMenuBottomButton.x = 0;
	homeMenuBottomButton.y = 480;
	
	int i = 0;
	int fade = 0x00;
	
	for(i = 0; i <= 28; i++)
	{
		homeMenuTopButton.y += 4;
		homeMenuBottomButton.y -= 4;
		
		draw_covers();
		GRRLIB_2D_Init();
		GRRLIB_FillScreen(0x00000000|fade);
		
		fade+=7;
        
		Button_Paint(&homeMenuTopButton);
		Button_Paint(&homeMenuBottomButton);
		GRRLIB_Render();
	}
	
	wiiMenuButton.x = 28-400;
	wiiMenuButton.y = 180;
	loaderButton.x  = 355+400;
	loaderButton.y = 170;
	
	wiimoteButton.x = 64;
	wiimoteButton.y = 400 + 400;
	for(i = 0; i <= 10; i++)
	{
		wiiMenuButton.x += 40;
		loaderButton.x  -= 40;
		wiimoteButton.y -= 40;
		
		draw_covers();
		GRRLIB_2D_Init();
		GRRLIB_FillScreen(0x000000CC);
		
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
		
		GetWiimoteData();

//		WPAD_IR(WPAD_CHAN_0, &self.ir); // Let's get our infrared data
//		WPAD_Orientation(WPAD_CHAN_0, &self.orient);
//		wd = WPAD_Data(WPAD_CHAN_0);

//		pointer.p_x = self.ir.sx-160;
//		pointer.p_y = self.ir.sy-220;
//		pointer.p_ang = self.ir.angle/2; // Set angle/2 to translate correctly

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
				//launchTitle(0x0000000100000003LL, 0); //launch system menu
				HomeMenu_Destroy();
				exitToSystemMenu() ;
				//SYS_ResetSystem(SYS_RETURNTOMENU,0,0);%48%41%58%58
			}
			else if (Button_Select(&loaderButton, pointer.p_x, pointer.p_y))
			{
				WPAD_Rumble(0,0); // Kill the rumble
				HomeMenu_Destroy();
				launchTitle(0x0001000148415858LL, 0); //launch system menu
				//exit(1);        // eventually, return a value.
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
		GRRLIB_FillScreen(0x000000CC);

		//Button_Theme_Paint(&settingsButton, SETTING_theme);
		Button_Paint(&homeMenuTopButton);
		Button_Paint(&homeMenuBottomButton);
		Button_Paint(&wiiMenuButton);
		Button_Paint(&loaderButton);
		Button_Paint(&wiimoteButton);
		
		Button_Hover(&homeMenuTopButton, pointer.p_x, pointer.p_y);
		Button_Hover(&homeMenuBottomButton, pointer.p_x, pointer.p_y);
		Button_Hover(&wiiMenuButton, pointer.p_x, pointer.p_y);
		Button_Hover(&loaderButton, pointer.p_x, pointer.p_y);
			
		// Check for button-pointer intersections, and rumble
		if (Button_Hover(&homeMenuTopButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&homeMenuBottomButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&wiiMenuButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&loaderButton, pointer.p_x, pointer.p_y))
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

		// Draw the default pointer hand
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		// Spit it out
		GRRLIB_Render();
		
		if(shutdown == 1)
		{
			Sys_Shutdown();
		}
		else if(reset == 1)
		{
			Sys_Reboot(); 
		}
	}while(doloop);
	
	for(i = 0; i <= 10; i++)
	{
		wiiMenuButton.x -= 40;
		loaderButton.x  += 40;
		wiimoteButton.y += 40;
		
		draw_covers();
		
		GRRLIB_2D_Init();
		GRRLIB_FillScreen(0x000000CC);
		
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
		GRRLIB_FillScreen(0x00000000|fade);

		Button_Paint(&homeMenuTopButton);
		Button_Paint(&homeMenuBottomButton);
		GRRLIB_Render();
	}
	
}

void HomeMenu_Destroy()
{
	int i;

	for(i=0xff;i>=0;i-=8){
		GRRLIB_FillScreen(0x000000FF);
		
		draw_covers();
		//GRRLIB_2D_Init();
		GRRLIB_FillScreen(0x000000CC);
		
		GRRLIB_DrawImg(homeMenuTopButton.x, homeMenuTopButton.y, homeMenuTopButton.texture, 0, 1, 1, 0xFFFFFF00|i);
		GRRLIB_DrawImg(homeMenuBottomButton.x, homeMenuBottomButton.y, homeMenuBottomButton.texture, 0, 1, 1, 0xFFFFFF00|i);
		GRRLIB_DrawImg(wiiMenuButton.x, wiiMenuButton.y, wiiMenuButton.texture, 0, 1, 1, 0xFFFFFF00|i);
		GRRLIB_DrawImg(loaderButton.x, loaderButton.y, loaderButton.texture, 0, 1, 1, 0xFFFFFF00|i);
		GRRLIB_DrawImg(wiimoteButton.x, wiimoteButton.y, wiimoteButton.texture, 0, 1, 1, 0xFFFFFF00|i);
		
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFF00|i);
		
		GRRLIB_Render();
	}

	free(homeMenuTopButton.texture.data);
	free(homeMenuBottomButton.texture.data);
	free(wiiMenuButton.texture.data);
	free(loaderButton.texture.data);
	free(homeMenuTopButton.hoverTexture.data);
	free(homeMenuBottomButton.hoverTexture.data);
	free(wiiMenuButton.hoverTexture.data);
	free(loaderButton.hoverTexture.data);
	free(wiimoteButton.texture.data);
	
	freeResources();
}
