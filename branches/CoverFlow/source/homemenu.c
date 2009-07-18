/*
 *  homemenu.c
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11, alexcarlosantao
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 */
#include "homemenu.h"
#include "localization.h"
#include "gfx.h"

extern s_self self;
extern s_pointer pointer;

extern u8 shutdown;
extern u8 reset;
extern s_settings settings;
extern s_title* titleList;

#define MAX_WIIMOTES				4

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

	int   slide_wiimote = 0;
	float moving_y;

	//animate Slide
	homeMenuTopButton.x = 0;
	homeMenuTopButton.y = -108;
	homeMenuBottomButton.x = 0;
	homeMenuBottomButton.y = 480;
	
	int i = 0;
	int fade = 0x00;
	
	for(i = 0; i <= 26; i++)
	{
		homeMenuTopButton.y += 4;
		homeMenuBottomButton.y -= 4;
		
		draw_covers();
		GRRLIB_2D_Init();
		GRRLIB_FillScreen(0x00000000|fade);
		
		fade += 7;
        
		Button_Paint(&homeMenuTopButton);
		Button_Paint(&homeMenuBottomButton);
		moving_y = change_scale(i, 0, 26, 108, 0);
		Do_Batteries(moving_y);
		moving_y = change_scale(i, 0, 26, -38, 70);
		CFreeTypeGX_DrawText(ttf24pt, 470, moving_y, TX.homeMenuTopB, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
		CFreeTypeGX_DrawText(ttf24pt, 30, moving_y, TX.homeMenu, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);

		WPAD_ScanPads();
		PAD_ScanPads();
		GetWiimoteData();
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		
		GRRLIB_Render();
	}
	
	wiiMenuButton.x = 40-400;
	wiiMenuButton.y = 187;
	loaderButton.x  = 367+400;
	loaderButton.y = 187;
	
	wiimoteButton.x = 54;
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
		// Draw the 'Cancel' text
		CFreeTypeGX_DrawText(ttf24pt, 470, moving_y, TX.homeMenuTopB, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
		CFreeTypeGX_DrawText(ttf24pt, 30, moving_y, TX.homeMenu, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
		Do_Batteries(0);

		Button_HomeMenu_Paint(&wiiMenuButton);
		Button_HomeMenu_Paint(&loaderButton);
		
		Button_Paint(&wiimoteButton);
		
		WPAD_ScanPads();
		GetWiimoteData();
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		
		GRRLIB_Render();
	}

	/*Render and control Settings*/
	do{
		WPAD_ScanPads();
		PAD_ScanPads();
		GetWiimoteData();

		if ((WPAD_ButtonsDown(0) & WPAD_BUTTON_B) || (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)||(PAD_ButtonsDown(0) & PAD_TRIGGER_Z))
		{
			WPAD_Rumble(0,0); // Kill the rumble
			doloop = false;
		}

		//if ((WPAD_ButtonsDown(0) & WPAD_BUTTON_A)||((PAD_ButtonsDown(0) & PAD_BUTTON_A)))
		//{
			#ifdef TEST_MODE
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
				doloop = false;
			#endif
			if((WPAD_ButtonsDown(0) & WPAD_BUTTON_A) && Button_Select(&homeMenuTopButton, pointer.p_x, pointer.p_y))
			{
				WPAD_Rumble(0,0); // Kill the rumble
				doloop = false;
			}
			else if(((WPAD_ButtonsDown(0) & WPAD_BUTTON_A) && Button_Select(&wiiMenuButton, pointer.p_x, pointer.p_y)) || PAD_ButtonsDown(0) & PAD_TRIGGER_L)
			{
				WPAD_Rumble(0,0); // Kill the rumble
				HomeMenu_Destroy();
				exitToSystemMenu() ;
			}
			else if(((WPAD_ButtonsDown(0) & WPAD_BUTTON_A) && Button_Select(&loaderButton, pointer.p_x, pointer.p_y)) || PAD_ButtonsDown(0) & PAD_TRIGGER_R)
			{
				WPAD_Rumble(0,0); // Kill the rumble
				HomeMenu_Destroy();
				launchTitle(0x0001000148415858LL, 0); //launch system menu
				//exit(1);        // eventually, return a value.
			}
			/*
			else if (Button_Select(&homeMenuBottomButton, pointer.p_x, pointer.p_y))
			{
				//TODO Show control Screen
			}
			*/
		//}
		
		draw_covers();
		// Draw menu dialog background
		GRRLIB_2D_Init();
		GRRLIB_FillScreen(0x000000CC);

		Button_Paint(&homeMenuTopButton);
		Button_Paint(&homeMenuBottomButton);
		// Draw the 'Cancel' text
		CFreeTypeGX_DrawText(ttf24pt, 470, moving_y, TX.homeMenuTopB, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
		Do_Batteries(0);

		Button_HomeMenu_Paint(&wiiMenuButton);
		Button_HomeMenu_Paint(&loaderButton);
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

		if(Button_Hover(&homeMenuTopButton, pointer.p_x, pointer.p_y))
			CFreeTypeGX_DrawText(ttf24pt, 30, moving_y, TX.homeMenu, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
		else
			CFreeTypeGX_DrawText(ttf24pt, 30, moving_y, TX.homeMenu, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
		

		if(Button_Hover(&homeMenuBottomButton, pointer.p_x, pointer.p_y))
		{
			CFreeTypeGX_DrawText(ttf20pt, 375, 440 , TX.homeWiimote, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_CENTER );
			if(slide_wiimote > -20)
			{
				slide_wiimote--;
				wiimoteButton.y -=1;
			}
		}
		else
		{
			CFreeTypeGX_DrawText(ttf20pt, 375, 440 , TX.homeWiimote, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_CENTER );
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

		// Draw the 'Cancel' text
		CFreeTypeGX_DrawText(ttf24pt, 470, moving_y, TX.homeMenuTopB, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
		CFreeTypeGX_DrawText(ttf24pt, 30, moving_y, TX.homeMenu, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
		Do_Batteries(0);

		Button_HomeMenu_Paint(&wiiMenuButton);
		Button_HomeMenu_Paint(&loaderButton);
		
		Button_Paint(&wiimoteButton);
		
		WPAD_ScanPads();
		GetWiimoteData();
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		
		GRRLIB_Render();
	}
	
	for(i = 0; i <= 26; i++)
	{
		homeMenuTopButton.y -= 4;
		homeMenuBottomButton.y += 4;
		
		fade-=7;
		
		draw_covers();
		
		GRRLIB_2D_Init();
		GRRLIB_FillScreen(0x00000000|fade);

		Button_Paint(&homeMenuTopButton);
		Button_Paint(&homeMenuBottomButton);
		// Draw the 'Cancel' text
		moving_y = change_scale(i, 0, 26, 70, -38);
		CFreeTypeGX_DrawText(ttf24pt, 470, moving_y, TX.homeMenuTopB, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
		CFreeTypeGX_DrawText(ttf24pt, 30, moving_y, TX.homeMenu, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
		moving_y = change_scale(i, 0, 26, 4, 112);
		Do_Batteries(moving_y);

		WPAD_ScanPads();
		GetWiimoteData();
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		
		GRRLIB_Render();
	}
	
} // End HomeMenu_Show()


void HomeMenu_Destroy()
{
	int i;

	for(i=0xff;i>=0;i-=8){
		GRRLIB_FillScreen(0x000000FF);
		
		draw_covers();
		GRRLIB_2D_Init();
		GRRLIB_FillScreen(0x000000CC);
		
		GRRLIB_DrawImg(homeMenuTopButton.x, homeMenuTopButton.y, homeMenuTopButton.texture, 0, 1, 1, 0xFFFFFF00|i);
		GRRLIB_DrawImg(homeMenuBottomButton.x, homeMenuBottomButton.y, homeMenuBottomButton.texture, 0, 1, 1, 0xFFFFFF00|i);
		// Draw the 'Cancel' text
		CFreeTypeGX_DrawText(ttf24pt, 470, 70, TX.homeMenuTopB, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
		CFreeTypeGX_DrawText(ttf24pt, 30, 70, TX.homeMenu, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
		GRRLIB_DrawImg(wiiMenuButton.x, wiiMenuButton.y, wiiMenuButton.texture, 0, 1, 1, 0xFFFFFF00|i);
		CFreeTypeGX_DrawText(ttf24pt, (wiiMenuButton.x + (wiiMenuButton.texture.w / 2)), (wiiMenuButton.y + (wiiMenuButton.texture.h / 2)), wiiMenuButton.ttf_label, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		GRRLIB_DrawImg(loaderButton.x, loaderButton.y, loaderButton.texture, 0, 1, 1, 0xFFFFFF00|i);
		CFreeTypeGX_DrawText(ttf24pt, (loaderButton.x + (loaderButton.texture.w / 2)), (loaderButton.y + (loaderButton.texture.h / 2)), loaderButton.ttf_label, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		GRRLIB_DrawImg(wiimoteButton.x, wiimoteButton.y, wiimoteButton.texture, 0, 1, 1, 0xFFFFFF00|i);
		
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFF00|i);
		
		GRRLIB_Render();
	}

	BUFFER_KillBuffer();
	freeResources();
	free(titleList);
}


void Do_Batteries(int y_modifier)
{
	int i,x;
	u8 ret;
	u8 level;
	char tUnit[4];
	
	
	CFreeTypeGX_DrawText(ttf20pt, 375, (440 + y_modifier), TX.homeWiimote, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_CENTER );

	
	for(x = 0; x <= 3; x++)
	{
		ret = WPAD_BatteryLevel(x);
		level = (ret * 4)/100;
		sprintf(tUnit, TX.batteryUnit, x+1);
		if(level > 4) level = 4; //restrict to maximum bars
		
		if(ret!=0) //wiimote found
		{
			if(level==0) //battery critical
			{
				CFreeTypeGX_DrawText(ttf18pt, 180+(106*x), (400 + y_modifier), tUnit, (GXColor){0xe9, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				GRRLIB_DrawImg(216+(106*x), (381 + y_modifier), battery_dead, 0, 1, 1, 0xFFFFFFFF); //draw red battery
			}
			else if(level==1) //battery level is in the red
			{
				CFreeTypeGX_DrawText(ttf18pt, 180+(106*x), (400 + y_modifier), tUnit, (GXColor){0xe9, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				GRRLIB_DrawImg(216+(106*x), (381 + y_modifier), battery_dead, 0, 1, 1, 0xFFFFFFFF); //draw red battery
				GRRLIB_DrawImg(221+((107*x)), (385 + y_modifier), battery_bar_red, 0, 1, 1, 0xFFFFFFFF); //draw red bars
			}
			else //level ok - draw white
			{
				CFreeTypeGX_DrawText(ttf18pt, 180+(106*x), (400 + y_modifier), tUnit, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
				GRRLIB_DrawImg(216+(106*x), (381 + y_modifier), battery, 0, 1, 1, 0xFFFFFFFF); //draw battery container
				
				for(i = 0; i < level; i++)
				{
					GRRLIB_DrawImg(221+((106*x)+(8*i)), (385 + y_modifier), battery_bar, 0, 1, 1, 0xFFFFFFFF); //draw the bars
				}
			}
		}
		else //no wiimote so write player ID in grey (matches sysmenu behaviour)
			CFreeTypeGX_DrawText(ttf18pt, 180+(106*x), (400 + y_modifier),  tUnit, (GXColor){0x80, 0x80, 0x80, 0xff}, FTGX_JUSTIFY_LEFT);
	}
} // End Do_Batteries()

