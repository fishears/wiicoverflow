/* 
  *  menuMultiDOL.c 
  * 
  *  Wii CoverFloader 
  *  Copyright 2009 Beardface April 29th, 2009 
  *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11 
  *  Licensed under the terms of the GNU GPL, version 2 
  *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt 
 */ 

#include "menuMultiDOL.h" 
  
 extern s_self 		self; 
 extern s_pointer 	pointer; 
 extern s_title* 	titleList;
 extern s_settings	settings;
 
 char episodeDOL[CFG_METROID_COUNT][16] =
{
 {"Metroid Prime"},          
 {"Metroid Prime 2"}, 
 {"Metroid Prime 3"} 
}; 

 
 
 int showDOLWindow(){ 
         int ret = 1;
         bool doloop = true; 
         int fade = 5; 
         int y, sp; 
  
		 okButton.x = 353;
		 okButton.y = 335;
  
         do{ 
			y = 115; 
			sp = 0; 

			WPAD_ScanPads(); 
			PAD_ScanPads(); 
			GetWiimoteData(); 

			draw_covers(); 
			GRRLIB_2D_Init(); 
			GRRLIB_FillScreen(0x00000000|fade); 

			GRRLIB_Rectangle(40, 106, 560, 276, 0xffffffdd, true);
			GRRLIB_Rectangle(42, 108, 556, 272, 0x737373FF, true);
			GRRLIB_Rectangle(268, 168, 304, 44, 0xffffffdd, true);
			GRRLIB_Rectangle(270, 170, 300, 40, 0x000000FF, true);

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
			CFreeTypeGX_DrawText(ttf20pt, 268, 149, "Episode to play ?", (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT);

			CFreeTypeGX_DrawText(ttf16pt, 350, 280, "Episode:", (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_RIGHT);
			CFreeTypeGX_DrawText(ttf16pt, 503, 280, episodeDOL[ret -1], (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_CENTER);




			// Draw the game cover
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
							{
								GRRLIB_DrawFlatCoverImg(60, 131, _texture_data[self.gameSelected], 0, AR_16_9, 1, 0xFFFFFFFF);
							}
							else
							{
								GRRLIB_DrawImg(60, 131, _texture_data[self.gameSelected], 0, AR_16_9, 1, 0xFFFFFFFF);
							}
						}
						else
						{
							if(settings.covers3d)
							{
								GRRLIB_DrawFlatCoverImg(60, 131, _texture_data[self.gameSelected], 0, 1, 1, 0xFFFFFFFF);
							}
							else
							{
								GRRLIB_DrawImg(60, 131, _texture_data[self.gameSelected], 0, 1, 1, 0xFFFFFFFF);
							}
						}
					}
					else
					{
						if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
						{
							if(settings.covers3d)
							{
								GRRLIB_DrawFlatCoverImg(60, 131 , cover_texture_3d, 0, AR_16_9, 1, 0xFFFFFFFF);
							}
							else
							{
								GRRLIB_DrawImg(60, 131, cover_texture, 0, AR_16_9, 1, 0xFFFFFFFF);
							}
						}
						else
						{
							if(settings.covers3d)
							{
								GRRLIB_DrawFlatCoverImg(60, 131, cover_texture_3d, 0, 1, 1, 0xFFFFFFFF);
							}
							else
							{
								GRRLIB_DrawImg(60, 131, cover_texture, 0, 1, 1, 0xFFFFFFFF);
							}
						}
					}
					
					pthread_mutex_unlock(&buffer_mutex[self.gameSelected]);
				}
				else
				{	if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
				{
					if(settings.covers3d)
					{
						GRRLIB_DrawFlatCoverImg(60, 131, cover_texture_3d, 0, AR_16_9, 1, 0xFFFFFFFF);
					}
					else
					{
						GRRLIB_DrawImg(60, 131, cover_texture, 0, AR_16_9, 1, 0xFFFFFFFF);
					}
				}
				else
				{
					if(settings.covers3d)
					{
						GRRLIB_DrawFlatCoverImg(60, 131, cover_texture_3d, 0, 1, 1, 0xFFFFFFFF);
					}
					else
					{
						GRRLIB_DrawImg(60, 131, cover_texture, 0, 1, 1, 0xFFFFFFFF);
					}
				}
				}
			}
			else
			{
				if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
				{
					if(settings.covers3d)
					{
						GRRLIB_DrawFlatCoverImg(60, 131, cover_texture_3d, 0, AR_16_9, 1, 0xFFFFFFFF);
					}
					else
					{
						GRRLIB_DrawImg(60, 131, cover_texture, 0, AR_16_9, 1, 0xFFFFFFFF);
					}
				}
				else
				{
					if(settings.covers3d)
					{
						GRRLIB_DrawFlatCoverImg(60, 131, cover_texture_3d, 0, 1, 1, 0xFFFFFFFF);
					}
					else
					{
						GRRLIB_DrawImg(60, 131, cover_texture, 0, 1, 1, 0xFFFFFFFF);
					}
				}
			}


			Button_TTF_Paint(&okButton); 
			Button_Paint(&gDOLdownButton);
			Button_Paint(&gDOLupButton);

			DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF); 

			if ( Button_Hover(&okButton, pointer.p_x, pointer.p_y) ||
				 Button_Hover(&gDOLupButton, pointer.p_x, pointer.p_y) ||
			     Button_Hover(&gDOLdownButton, pointer.p_x, pointer.p_y) ) 
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
			
			if((WPAD_ButtonsDown(0) & WPAD_BUTTON_A)||(PAD_ButtonsDown(0) & PAD_BUTTON_A))
			{
				if(Button_Select(&okButton, pointer.p_x, pointer.p_y) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A) 
				{
					doloop = false;
				}
				else if (Button_Select(&gDOLdownButton, pointer.p_x, pointer.p_y))
				{ // Clicked on the episodeDOL buttons
					if (ret > 1)
					{
						ret --;
					}
					else
					{
						ret = (CFG_METROID_COUNT);
					}
				}
				else if (Button_Select(&gDOLupButton, pointer.p_x, pointer.p_y))
				{
					if (ret < (CFG_METROID_COUNT))
					{
						ret ++;
					}
					else
					{
						ret = 1;
					}
				}
			}
			GRRLIB_Render(); 
                  
         }while(doloop); 
  return ret;

}
