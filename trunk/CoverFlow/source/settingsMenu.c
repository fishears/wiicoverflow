#include "settingsMenu.h"

extern s_self self;
extern s_pointer pointer;
extern s_settings settings;
extern s_gameSettings gameSetting;

extern u8 shutdown;
extern u8 reset;

extern char languages[11][22];
extern char vidmodes[6][22];
extern char hooks[3][9];

void Graphic_Settings_Menu(){

	bool doloop = true;
	//bool dummy = false;

	/*Render and control Settings*/
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
				settings.coverSpacing -= 0.05;
			}
			else if(Button_Select(&spacingupButton, pointer.p_x, pointer.p_y))
			{
				settings.coverSpacing += 0.05;
			}
			else if(Button_Select(&angledownButton, pointer.p_x, pointer.p_y))
			{
				settings.coverAngle -= 1;
			}
			else if(Button_Select(&angleupButton, pointer.p_x, pointer.p_y))
			{
				settings.coverAngle += 1;
			}
			else if(Button_Select(&zoomdownButton, pointer.p_x, pointer.p_y))
			{
				settings.coverZoom -= 0.01;
			}
			else if(Button_Select(&zoomupButton, pointer.p_x, pointer.p_y))
			{
				settings.coverZoom += 0.01;
			}
		}
		
		/*Draw Covers*/ //PREVIEW
		draw_covers();
		// Draw menu dialog background
		GRRLIB_2D_Init();
		GRRLIB_DrawImg(115, 95, menu_bg_texture, 0, 1, 1.45, 0xFFFFFFCC);
		// Draw text
		GRRLIB_Printf(190, 55,  font_title, settings.fontColor, 1, localStr("M049", "Graphics Settings"));
		GRRLIB_Printf(145, 100, font_texture, settings.fontColor, 1, localStr("M050", "Zoom:") );
		GRRLIB_Printf(350, 100, font_texture, settings.fontColor, 1, "%f", settings.coverZoom);
		GRRLIB_Printf(145, 148, font_texture, settings.fontColor, 1, localStr("Spacing:") );
		GRRLIB_Printf(350, 148, font_texture, settings.fontColor, 1, "%f", settings.coverSpacing);
		GRRLIB_Printf(145, 197, font_texture, settings.fontColor, 1, localStr("Angle:") );
		GRRLIB_Printf(350, 197, font_texture, settings.fontColor, 1, "%f", settings.coverAngle);
		GRRLIB_Printf(145, 245, font_texture, settings.fontColor, 1, localStr("Draw Window:") );
		GRRLIB_Printf(350, 245, font_texture, settings.fontColor, 1, "%d", settings.drawWindow);
		GRRLIB_Printf(145, 292, font_texture, settings.fontColor, 1, localStr("Game Title:") );
		
		//Button_Theme_Paint(&settingsButton, settings.theme);
		Button_Paint(&spacingupButton);
		Button_Paint(&spacingdownButton);
		Button_Paint(&zoomupButton);
		Button_Paint(&zoomdownButton);
		Button_Paint(&angleupButton);
		Button_Paint(&angledownButton);
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
			Button_Hover(&angledownButton, pointer.p_x, pointer.p_y))
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
		{
			Sys_Shutdown();
		}
		else if(reset == 1)
		{
			Sys_Reboot(); 
		}
	}while(doloop);
}

void Settings_Menu(){

	bool doloop = true;

	/*Render and control Settings*/
	do{
		WPAD_ScanPads();
		GetWiimoteData();

		// Handle button events
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME || WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
		{
			doloop = false;
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
				{
					settings.language --;
				}
				else
				{
					settings.language = (CFG_LANG_COUNT - 1);
				}
			}
			else if (Button_Select(&langupButton, pointer.p_x, pointer.p_y))
			{
				if (settings.language < (CFG_LANG_COUNT - 1))
				{
					settings.language ++;
				}
				else
				{
					settings.language = 0;
				}
			}
                        else if (Button_Select(&hookdownButton, pointer.p_x, pointer.p_y))
			{ // Clicked on the hooktype buttons
				if (settings.hooktype > 0)
				{
					settings.hooktype --;
				}
				else
				{
					settings.hooktype = (CFG_HOOK_COUNT - 1);
				}
			}
			else if (Button_Select(&hookupButton, pointer.p_x, pointer.p_y))
			{
				if (settings.hooktype < (CFG_HOOK_COUNT - 1))
				{
					settings.hooktype ++;
				}
				else
				{
					settings.hooktype = 0;
				}
			}
			else if (Button_Select(&downloadButton, pointer.p_x, pointer.p_y))
			{
				// Clicked on the Download Covers button
				//if (WindowPrompt("Cover download","This operation can't be canceled, continue?", &okButton, &cancelButton))
				if (WindowPrompt(localStr("M055", "Cover Download"),localStr("M056", "This operation can't be canceled, continue?"), &okButton, &cancelButton))
				{
					batchDownloadCover(self.gameList);
					CoversDownloaded();
				}
			}
			else if (Button_Select(&viddownButton, pointer.p_x,pointer.p_y))
			{
				// Clicked on the video down button
				if (settings.video > 0)
				{
					settings.video --;
				}
				else
				{
					settings.video = (CFG_VIDEO_COUNT -1);
				}
			}
			else if (Button_Select(&vidupButton, pointer.p_x,pointer.p_y))
			{
				// Clicked on the video up button
				if (settings.video <(CFG_VIDEO_COUNT -1))
				{
					settings.video ++;
				}
				else
				{
					settings.video = 0;
				}
			}
			else if (Button_Select(&themeWhiteButton, pointer.p_x, pointer.p_y) || Button_Select(&themeBlackButton, pointer.p_x, pointer.p_y))
			{
				// Clicked on the Theme button, toggle state
				settings.theme = (settings.theme) ? 0 : 1;
				if (settings.theme)
				{	// black fonts for white theme
					settings.fontColor = 0xFFFFFFFF; //temp until I fix the dialogs for the white theme
//					settings.fontColor = 0x000000FF;
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
		
		// Draw screen background
		draw_covers();
		// Draw menu dialog background
		GRRLIB_2D_Init();
		GRRLIB_DrawImg(115, 136, menu_bg_texture, 0, 1, 1.8, 0xFFFFFFCC);//old one bg renamed to _old
		// Draw text
		GRRLIB_Printf(184, 55,  font_title, 0xFFFFFFFF, 1, localStr("M057", "Coverflow Settings") );
		GRRLIB_Printf(145, 93,  font_texture, settings.fontColor, 1, localStr("M135", "Ocarina") );
		GRRLIB_Printf(310, 93,  font_texture, settings.fontColor, 1, localStr("M037", "Hook") );
		GRRLIB_Printf(385, 93,  font_texture, 0xFFFFFFFF, 1, "%s",hooks[settings.hooktype]);
		GRRLIB_Printf(145, 128, font_texture, settings.fontColor, 1, localStr("M038", "Language") );
		GRRLIB_Printf(330, 128, font_texture, 0xFFFFFFFF, 1, "%s",languages[settings.language]);
		GRRLIB_Printf(145, 157, font_texture, settings.fontColor, 1, localStr("M039", "Video mode") );
		GRRLIB_Printf(330, 155, font_texture, 0xFFFFFFFF, 1, "%s",vidmodes[settings.video]);
		GRRLIB_Printf(145, 189, font_texture, settings.fontColor, 1, localStr("M040", "VIDTV patch") );
		GRRLIB_Printf(145, 221, font_texture, settings.fontColor, 1, localStr("M058", "Graphics") );
		GRRLIB_Printf(145, 260, font_texture, settings.fontColor, 1, localStr("M059", "Get Missing Covers") );
		GRRLIB_Printf(145, 300, font_texture, settings.fontColor, 1, localStr("M060", "Theme") );
		GRRLIB_Printf(145, 340, font_texture, settings.fontColor, 1, localStr("M061", "1-Click Launch") );
		GRRLIB_Printf(145, 380, font_texture, settings.fontColor, 1, localStr("M062", "Rumble") );
		GRRLIB_Printf(145, 420, font_texture, settings.fontColor, 1, localStr("M063", "Sound") );
		//GRRLIB_Printf(218, 446, font_texture, settings.fontColor, 1.15, "Press B to return");

		// Draw stateless buttons
		Button_Paint(&langupButton);
		Button_Paint(&langdownButton);
		Button_Paint(&vidupButton);
		Button_Paint(&viddownButton);
		Button_Paint(&hookupButton);
		Button_Paint(&hookdownButton);
		Button_Paint(&graphicsButton);
		Button_Paint(&downloadButton);
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
			Button_Hover(&downloadButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&hookupButton, pointer.p_x, pointer.p_y) ||
			Button_Hover(&hookdownButton, pointer.p_x, pointer.p_y))
		{
			// Should we be rumbling?
			if (--self.rumbleAmt > 0)
			{
				if(settings.rumble)
				{
					WPAD_Rumble(0,1); // Turn on Wiimote rumble
				}
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
	
	WPAD_Rumble(0,0);
	self.rumbleAmt = 0;
}
