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

extern s_self         self;
extern s_pointer      pointer;
extern s_settings     settings;
extern s_gameSettings gameSetting;
extern s_title*       titleList;
extern u8             shutdown;
extern u8             reset;

// Language selection config
char languages[11][22] =
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
char vidmodes[6][22] =
{{ "  Game Default" },
{ "   Automatic", },
{ "  Force PAL50", },
{ "  Force PAL60", },
{ "  Force NTSC", },
{ "Console Default"}};
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
		CFreeTypeGX_DrawTextWithShadow(ttf18pt, 320, 55, CFreeTypeGX_charToWideChar(ttf18pt, TX.graphicSetting), (GXColor){0xff, 0xff, 0xff, 0xff}, (GXColor){0x33, 0x33, 0x33, 0x99}, FTGX_JUSTIFY_CENTER);
		GRRLIB_Printf(145, 100, font_texture, settings.fontColor, 1, TX.zoom );
		GRRLIB_Printf(350, 100, font_texture, settings.fontColor, 1, "%f", settings.coverZoom);
		GRRLIB_Printf(145, 138, font_texture, settings.fontColor, 1, TX.spacing );
		GRRLIB_Printf(350, 138, font_texture, settings.fontColor, 1, "%f", settings.coverSpacing);
		GRRLIB_Printf(145, 176, font_texture, settings.fontColor, 1, TX.angle );
		GRRLIB_Printf(350, 176, font_texture, settings.fontColor, 1, "%f", settings.coverAngle);
		GRRLIB_Printf(145, 214, font_texture, settings.fontColor, 1, TX.falloff );
		GRRLIB_Printf(350, 214, font_texture, settings.fontColor, 1, "%f", settings.coverFallOff);
		GRRLIB_Printf(145, 252, font_texture, settings.fontColor, 1, TX.drawWindow );
		GRRLIB_Printf(350, 252, font_texture, settings.fontColor, 1, "%d", settings.drawWindow);
		GRRLIB_Printf(145, 290, font_texture, settings.fontColor, 1, TX.gameTitle );
		
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
			Button_Hover(&falloffdownButton, pointer.p_x, pointer.p_y))
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
		CFreeTypeGX_DrawTextWithShadow(ttf18pt, 320, 55, CFreeTypeGX_charToWideChar(ttf18pt, TX.cflowSettings), (GXColor){0xff, 0xff, 0xff, 0xff}, (GXColor){0x33, 0x33, 0x33, 0x99}, FTGX_JUSTIFY_CENTER);
		GRRLIB_Printf(145, 93,  font_texture, settings.fontColor, 1, TX.ocarina );
		GRRLIB_Printf(310, 93,  font_texture, settings.fontColor, 1, TX.hook );
		GRRLIB_Printf(385, 93,  font_texture, 0xFFFFFFFF, 1, "%s",hooks[settings.hooktype]);
		GRRLIB_Printf(145, 128, font_texture, settings.fontColor, 1, TX.language );
		GRRLIB_Printf(330, 128, font_texture, 0xFFFFFFFF, 1, "%s",languages[settings.language]);
		GRRLIB_Printf(145, 157, font_texture, settings.fontColor, 1, TX.videoMode );
		GRRLIB_Printf(330, 155, font_texture, 0xFFFFFFFF, 1, "%s",vidmodes[settings.video]);
		GRRLIB_Printf(145, 187, font_texture, settings.fontColor, 1, TX.patchVIDTV );
		GRRLIB_Printf(145, 227, font_texture, settings.fontColor, 1, TX.graphics );
		GRRLIB_Printf(145, 267, font_texture, settings.fontColor, 1, TX.getAddData );
		GRRLIB_Printf(145, 307, font_texture, settings.fontColor, 1, TX.theme );
		GRRLIB_Printf(145, 347, font_texture, settings.fontColor, 1, TX.oneClickLaunch );
		GRRLIB_Printf(145, 387, font_texture, settings.fontColor, 1, TX.rumble );
		GRRLIB_Printf(145, 427, font_texture, settings.fontColor, 1, TX.sound );
		//GRRLIB_Printf(218, 446, font_texture, settings.fontColor, 1.15, "Press B to return");

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
