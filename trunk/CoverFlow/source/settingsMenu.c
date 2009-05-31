/*
 *  settingsMenu.c
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11, alexcarlosantao
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 *  This file contains the methods to draw the two primary settings screens:
 *    - CoverFloader Settings
 *    - Graphics Settings
 */
#include "settingsMenu.h"
#include "localization.h"

extern char** languages;
extern char** vidmodes;

extern s_self         self;
extern s_pointer      pointer;
extern s_settings     settings;
extern s_gameSettings gameSetting;
extern s_title*       titleList;
extern u8             shutdown;
extern u8             reset;

char tFloat[12];
//hook types for ocarina
char hooks[3][9] =
{{"   VI"},
{" Wii Pad"},
{" GC Pad"}};


void Graphic_Settings_Menu(){

	bool doloop = true;

	do{
		WPAD_ScanPads();
		GetWiimoteData();

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
			doloop = false;
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
			doloop = false;

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
		{
			if (Button_Select(&settingsButton, pointer.p_x, pointer.p_y))
			{
				doloop = false;
			}
			else if (Button_Select(&resetButton, pointer.p_x, pointer.p_y))
			{
				graphics_SETTINGS_Init();
			}
			else if (Button_Select(&windowdownButton, pointer.p_x, pointer.p_y))
			{
				if (settings.drawWindow > 1)
					settings.drawWindow -= 1;
			}
			else if (Button_Select(&windowupButton, pointer.p_x, pointer.p_y))
			{
				if (settings.drawWindow < 100) // Allow for user to shoot self in foot
					settings.drawWindow += 1;
			}
			else if (Button_Select(&coverTextOnButton, pointer.p_x, pointer.p_y) || Button_Select(&coverTextOffButton, pointer.p_x, pointer.p_y))
			{
				settings.coverText = (settings.coverText) ? 0 : 1;
			}
			else if (Button_Select(&covers3dOnButton, pointer.p_x, pointer.p_y) || Button_Select(&covers3dOffButton, pointer.p_x, pointer.p_y))
			{
				settings.covers3d = (settings.covers3d) ? 0 : 1;
				ResetBuffer();
			}
                        else if (Button_Select(&hidescrollOnButton, pointer.p_x, pointer.p_y) || Button_Select(&hidescrollOffButton, pointer.p_x, pointer.p_y))
			{
				settings.hideScroll = (settings.hideScroll) ? 0 : 1;
			}
		}
		
		if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_A)
		{
			if(Button_Select(&spacingdownButton, pointer.p_x, pointer.p_y))
			{
				if (settings.coverSpacing <= 2)
					settings.coverSpacing = 2; // sanity check
				else
					settings.coverSpacing -= 0.05;
			}
			else if(Button_Select(&spacingupButton, pointer.p_x, pointer.p_y))
			{
				if (settings.coverSpacing >= 9)
					settings.coverSpacing = 9; // sanity check
				else
					settings.coverSpacing += 0.05;
			}
			else if(Button_Select(&angledownButton, pointer.p_x, pointer.p_y))
			{
				if (settings.coverAngle <= -140)
					settings.coverAngle = -140; // sanity check
				else
					settings.coverAngle -= 1;
			}
			else if(Button_Select(&angleupButton, pointer.p_x, pointer.p_y))
			{
				if (settings.coverAngle >= 140)
					settings.coverAngle = 140; // sanity check
				else
					settings.coverAngle += 1;
			}
			else if(Button_Select(&zoomdownButton, pointer.p_x, pointer.p_y))
			{
				if (settings.coverZoom <= -8.0)
					settings.coverZoom = -8.0; // sanity check
				else
					settings.coverZoom -= 0.03;
			}
			else if(Button_Select(&zoomupButton, pointer.p_x, pointer.p_y))
			{
                if (settings.coverZoom >= .69)
					settings.coverZoom  = .69; // sanity check
				else
					settings.coverZoom += 0.03;
			}
			else if(Button_Select(&falloffdownButton, pointer.p_x, pointer.p_y))
			{
				settings.coverFallOff -= 0.002;
			}
			else if(Button_Select(&falloffupButton, pointer.p_x, pointer.p_y))
			{
				settings.coverFallOff += 0.002;
			}
		}
		
		// Draw the covers behind the dialog
		draw_covers();
		// Draw menu dialog background
		GRRLIB_2D_Init();
		GRRLIB_DrawImg(115, 95, menu_bg_texture, 0, 1, 1.45, 0xFFFFFFCC);
		// Draw text features
		CFreeTypeGX_DrawText(ttf18pt, 320, 55, TX.graphicSetting, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_CENTER);
		CFreeTypeGX_DrawText(ttf14pt, 145,90, TX.zoom, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                sprintf(tFloat, "%f", settings.coverZoom);
                CFreeTypeGX_DrawText(ttf14pt, 350,90, tFloat, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                CFreeTypeGX_DrawText(ttf14pt, 145,123, TX.spacing, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                sprintf(tFloat, "%f", settings.coverSpacing);
                CFreeTypeGX_DrawText(ttf14pt, 350,123, tFloat, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                CFreeTypeGX_DrawText(ttf14pt, 145,156, TX.angle, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                sprintf(tFloat, "%f", settings.coverAngle);
                CFreeTypeGX_DrawText(ttf14pt, 350,156, tFloat, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                CFreeTypeGX_DrawText(ttf14pt, 145,189, TX.falloff, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                sprintf(tFloat, "%f", settings.coverFallOff);
                CFreeTypeGX_DrawText(ttf14pt, 350,189, tFloat, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                CFreeTypeGX_DrawText(ttf14pt, 145,222, TX.drawWindow, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                sprintf(tFloat, "%d", settings.drawWindow);
                CFreeTypeGX_DrawText(ttf14pt, 350,222, tFloat, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                CFreeTypeGX_DrawText(ttf14pt, 145,254, TX.gameTitle, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		
		//TODO add language setting for 3D Covers and Hide Scroll Bar
		CFreeTypeGX_DrawText(ttf14pt, 145,294, "3D Covers", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
                CFreeTypeGX_DrawText(ttf14pt, 145,334, "Hide Scroll Bar", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		
		//Button_Theme_Paint(&settingsButton, settings.theme);
		Button_Paint(&spacingupButton);
		Button_Paint(&spacingdownButton);
		Button_Paint(&zoomupButton);
		Button_Paint(&zoomdownButton);
		Button_Paint(&angleupButton);
		Button_Paint(&angledownButton);
		Button_Paint(&falloffupButton);
		Button_Paint(&falloffdownButton);
		Button_Paint(&windowupButton);
		Button_Paint(&windowdownButton);
		
		if (settings.coverText)
		{
			Button_Paint(&coverTextOnButton);
		}
		else Button_Paint(&coverTextOffButton);
		
		if (settings.covers3d)
		{
			Button_Paint(&covers3dOnButton);
		}
		else Button_Paint(&covers3dOffButton);

                if (settings.hideScroll)
		{
			Button_Paint(&hidescrollOnButton);
		}
                else Button_Paint(&hidescrollOffButton);
		
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
			Button_Hover(&angledownButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&falloffupButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&falloffdownButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&covers3dOnButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&covers3dOffButton, pointer.p_x, pointer.p_y) ||
                        Button_Hover(&hidescrollOnButton, pointer.p_x, pointer.p_y) ||
                        Button_Hover(&hidescrollOffButton, pointer.p_x, pointer.p_y))
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

		// Draw the pointer hand
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		// Spit it out
		GRRLIB_Render();
		
		if(shutdown == 1)
			Sys_Shutdown();
		else if(reset == 1)
			Sys_Reboot(); 

	}while(doloop);
}

void Settings_Menu(){

	bool doloop = true;

	do{
		WPAD_ScanPads();
		GetWiimoteData();

		// Handle button events
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME || WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
		{
			doloop = false;
		}
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_1) // Check for screen shot
		{
			GRRLIB_ScrShot(USBLOADER_PATH "/sshot.png");
		}
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
		{
			if (Button_Select(&settingsButton, pointer.p_x, pointer.p_y))
			{
				doloop = false; // Clicked the setting button, exit to main screen
			}
			else if (Button_Select(&cheatonButton, pointer.p_x, pointer.p_y) || Button_Select(&cheatoffButton, pointer.p_x, pointer.p_y))
			{
				settings.ocarina = (settings.ocarina) ? 0 : 1; // Clicked the Ocarina button, toggle state
			}
			else if (Button_Select(&vidtvonButton, pointer.p_x, pointer.p_y) || Button_Select(&vidtvoffButton, pointer.p_x, pointer.p_y))
			{
				settings.vipatch = (settings.vipatch) ? 0 : 1; // Clicked the VIPATCH button, toggle state
			}
			else if (Button_Select(&graphicsButton, pointer.p_x, pointer.p_y))
			{
				Graphic_Settings_Menu(); // Clicked the Graphics Setting button, launch menu routine
			}
			else if (Button_Select(&langdownButton, pointer.p_x, pointer.p_y))
			{ // Clicked on the language buttons
				if (settings.language > 0)
					settings.language --;
				else
					settings.language = (CFG_LANG_COUNT - 1);
			}
			else if (Button_Select(&langupButton, pointer.p_x, pointer.p_y))
			{
				if (settings.language < (CFG_LANG_COUNT - 1))
					settings.language ++;
				else
					settings.language = 0;
			}
			else if (Button_Select(&hookdownButton, pointer.p_x, pointer.p_y))
			{ // Clicked on the hooktype buttons
				if (settings.hooktype > 0)
					settings.hooktype --;
				else
					settings.hooktype = (CFG_HOOK_COUNT - 1);
			}
			else if (Button_Select(&hookupButton, pointer.p_x, pointer.p_y))
			{
				if (settings.hooktype < (CFG_HOOK_COUNT - 1))
					settings.hooktype ++;
				else
					settings.hooktype = 0;
			}
			else if (Button_Select(&coversButton, pointer.p_x, pointer.p_y))
			{
				// Clicked on the Download Covers button
				if (WindowPrompt(TX.coverDownload, TX.opNoCancel , &okButton, &cancelButton))
				{
					WPAD_Rumble(0,0); //sometimes rumble remain active
					if(networkInit(self.ipAddress))
					{
						batchDownloadCover(self.gameList);
						CoversDownloaded();
					}
					else
						WindowPrompt(TX.error, TX.iniNetErr , &okButton, 0);
				}
			}
			else if(Button_Select(&titlesButton, pointer.p_x, pointer.p_y)){
				WPAD_Rumble(0,0); //sometimes rumble remain active
				if(networkInit(self.ipAddress)){
					if(!downloadTitles())
						WindowPrompt( TX.error, TX.errTitles, &okButton, 0);
					else
					{
						if(self.usingTitlesTxt){
							self.usingTitlesTxt = false;
							self.titlesTxtSize = 0;
							free(titleList);
						}
						
						int numLines = initTitle();
						if(numLines > 0){
							self.usingTitlesTxt = true;
							self.titlesTxtSize = numLines;
							titleList = (s_title *) malloc (numLines * sizeof(s_title));
							fillTitleStruct(titleList, numLines);
							char numLinesTxt[250];
							sprintf(numLinesTxt, TX.successTitles, numLines);
							WindowPrompt(TX.Success, numLinesTxt, &okButton, 0);
						}
					}
				}
				else
					WindowPrompt(TX.error, TX.errNetTitles, &okButton, 0);
			}
			else if (Button_Select(&viddownButton, pointer.p_x,pointer.p_y))
			{
				// Clicked on the video down button
				if (settings.video > 0)
					settings.video --;
				else
					settings.video = (CFG_VIDEO_COUNT -1);
			}
			else if (Button_Select(&vidupButton, pointer.p_x,pointer.p_y))
			{
				// Clicked on the video up button
				if (settings.video <(CFG_VIDEO_COUNT -1))
					settings.video ++;
				else
					settings.video = 0;
			}
			else if (Button_Select(&themeWhiteButton, pointer.p_x, pointer.p_y) || Button_Select(&themeBlackButton, pointer.p_x, pointer.p_y))
			{
				// Clicked on the Theme button, toggle state
				settings.theme = (settings.theme) ? 0 : 1;
				if (settings.theme)
				{	// black fonts for white theme
					settings.fontColor = 0xFFFFFFFF;
					GRRLIB_SetBGColor(1); // set BG to white
				}
				else
				{   // white fonts for black theme
					settings.fontColor = 0xFFFFFFFF;
					GRRLIB_SetBGColor(0);  // set BG to black
				}
			}
			else if (Button_Select(&quickstartOnButton, pointer.p_x, pointer.p_y) || Button_Select(&quickstartOffButton, pointer.p_x, pointer.p_y))
			{
				settings.quickstart = (settings.quickstart) ? 0 : 1; // Clicked the "1-Click Launch" button, toggle state
			}
			else if (Button_Select(&rumbleOnButton, pointer.p_x, pointer.p_y) || Button_Select(&rumbleOffButton, pointer.p_x, pointer.p_y))
			{
				settings.rumble = (settings.rumble) ? 0 : 1; // Clicked the Rumble button, toggle state
			}
			else if (Button_Select(&musicOnButton, pointer.p_x, pointer.p_y) || Button_Select(&musicOffButton, pointer.p_x, pointer.p_y))
			{
				settings.music = (settings.music) ? 0 : 1; // Clicked the music button, toggle state
			}
		}
		
		// Draw the covers behind the dialog
		draw_covers();
		// Draw menu dialog background
		GRRLIB_2D_Init();
		GRRLIB_DrawImg(115, 136, menu_bg_texture, 0, 1, 1.8, 0xFFFFFFCC);
		// Draw text features
		CFreeTypeGX_DrawText(ttf18pt, 320, 55,  TX.cflowSettings, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_CENTER);
		CFreeTypeGX_DrawText(ttf14pt, 145,93,  TX.ocarina, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		CFreeTypeGX_DrawText(ttf14pt, 310,93,  TX.hook, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		CFreeTypeGX_DrawText(ttf14pt, 390,93,  hooks[settings.hooktype], (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		CFreeTypeGX_DrawText(ttf14pt, 145,128,  TX.language, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		CFreeTypeGX_DrawText(ttf14pt, 330,128, languages[settings.language], (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		CFreeTypeGX_DrawText(ttf14pt, 145,157, TX.videoMode, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		CFreeTypeGX_DrawText(ttf14pt, 330,155, vidmodes[settings.video], (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		CFreeTypeGX_DrawText(ttf14pt, 145,187, TX.patchVIDTV, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		CFreeTypeGX_DrawText(ttf14pt, 145,227, TX.graphics, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		CFreeTypeGX_DrawText(ttf14pt, 145,267, TX.getAddData, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		CFreeTypeGX_DrawText(ttf14pt, 145,307, TX.theme, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		CFreeTypeGX_DrawText(ttf14pt, 145,347, TX.oneClickLaunch, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		CFreeTypeGX_DrawText(ttf14pt, 145,387, TX.rumble, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
		CFreeTypeGX_DrawText(ttf14pt, 145,427, TX.sound, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);

		// Draw stateless buttons
		Button_Paint(&langupButton);
		Button_Paint(&langdownButton);
		Button_Paint(&vidupButton);
		Button_Paint(&viddownButton);
		Button_Paint(&hookupButton);
		Button_Paint(&hookdownButton);
		Button_Paint(&graphicsButton);
		Button_Paint(&titlesButton);
		Button_Paint(&coversButton);
		// Draw stateful buttons
		Button_Toggle_Paint(&cheatoffButton, &cheatonButton, settings.ocarina);
		Button_Toggle_Paint(&vidtvoffButton, &vidtvonButton, settings.vipatch);
		Button_Toggle_Paint(&themeBlackButton, &themeWhiteButton, settings.theme);
		Button_Toggle_Paint(&quickstartOffButton, &quickstartOnButton, settings.quickstart);
		Button_Toggle_Paint(&rumbleOffButton, &rumbleOnButton, settings.rumble);
		Button_Toggle_Paint(&musicOffButton, &musicOnButton, settings.music);
		
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
			Button_Hover(&quickstartOnButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&quickstartOffButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&rumbleOnButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&rumbleOffButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&musicOnButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&musicOffButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&graphicsButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&titlesButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&coversButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&hookupButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&hookdownButton, pointer.p_x, pointer.p_y))
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
		
		// Draw the default pointer hand
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		GRRLIB_Render();
		
		if(shutdown == 1)
			Sys_Shutdown();
		else if(reset == 1)
			Sys_Reboot(); 

	}while(doloop);
	
	WPAD_Rumble(0,0);
	self.rumbleAmt = 0;
}
