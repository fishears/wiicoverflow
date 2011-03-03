#include "info.h"
#include "localization.h"
#include "OSK.h"

#define INFOPAGES 2

extern s_self self;
extern s_pointer pointer;
extern s_path dynPath;


void showInfoWindow(){
    int i;
	int page = 1;
	int tab	= 130;
	bool doloop = true;
	int fade = 5;
	int y, sp;
	int step  = 35;
	int step2 = 30;
	char temp[256];
	int hdSize = self.freeSpace + self.usedSpace;
	int x = 92;
	
	u32 filledColor = 0xff9510FF;
	u32 emptyColor = 0xe4e4e4FF;

	okButton.x = 320 - 54;
	okButton.y = 380;
	
	
	for(i=0; i<9; i++)
	{
	 Duplicate_Button(&button_edit[i], button_edit[0], 515, i*step2 +97);
	}
	
	do{
		
		WPAD_ScanPads();
		PAD_ScanPads();
		GetWiimoteData();
		
		draw_covers();
		GRRLIB_2D_Init();
		GRRLIB_FillScreen(0x00000000|fade);
		
		GRRLIB_Rectangle(80, 58, 480, 364, 0xffffffdd, true);
		GRRLIB_Rectangle(82, 60, 476, 360, 0x737373FF, true);
		GRRLIB_DrawImg(62, 50, dialog_box_titlebar_long_texture, 0, 1, 1, 0xFFFFFFFF);
		
		CFreeTypeGX_DrawText(ttf16pt, 177, 63, TX.sysInfo, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		
		if (page == 1 )
			{
				y = 115;
				sp = 0;

				sprintf(temp, "%s %u", TX.devID, self.deviceID);
				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, temp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step;

				sprintf(temp, "%s %s (r%d)", TX.loaderVer, RELEASE, SVN_VERSION);
				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, temp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step;
				
				sprintf(temp, "%s %d (rev%d)",TX.iosVer, IOS_GetVersion(), IOS_GetRevision());
				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, temp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step;
				
				sprintf(temp, TX.ipAddress);
				strcat(temp," ");
				if(strcmp(self.ipAddress, "000.000.000.000") == 0)
					strcat(temp, TX.noConnect);
				else
					strcat(temp, self.ipAddress);
				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, temp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step;
				
				sprintf(temp, TX.instGames, self.gameCnt);
				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, temp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step;
				
				sprintf(temp, TX.infoHDD, self.usedSpace, self.freeSpace);
				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, temp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step;
				
				sprintf(temp, TX.spaceUsed, (self.usedSpace/hdSize)*100);
				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, temp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				//sp+=step;

				GRRLIB_Rectangle(272, y+sp-16, 200, 20, emptyColor, 1);
				GRRLIB_Rectangle(272, y+sp-16, (self.usedSpace/hdSize)*200, 20, filledColor, 1);
				
				GRRLIB_DrawImg(272, y+sp-16, hdspace_texture, 0, 1, 1, 0xFFFFFFFF);
				GRRLIB_DrawImgReflection(272, y+sp-16 + hdspace_texture.h + 4, hdspace_texture, 0, 1, 1, 0.7);
				
				GRRLIB_DrawImg(430, 80, menu_logo_texture, 0, 1, 1, 0xFFFFFFFF);
			
			}
		else if (page == 2 )
			{
				y = 115;
				sp = 0;
				
				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, "BootDevice:", (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT);
				CFreeTypeGX_DrawText(ttf16pt, x+tab, y+sp, dynPath.bootDevice, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step2;

				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, "usb-loader:", (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT);
				CFreeTypeGX_DrawText(ttf16pt, x+tab, y+sp, dynPath.dir_usb_loader, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step2;
	
				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, "3D-Cover:", (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT);
				CFreeTypeGX_DrawText(ttf16pt, x+tab, y+sp, dynPath.dir_3dcovers, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step2;

				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, "2D-Cover:", (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT);
				CFreeTypeGX_DrawText(ttf16pt, x+tab, y+sp, dynPath.dir_covers, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step2;

				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, "Disc:", (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT);
				CFreeTypeGX_DrawText(ttf16pt, x+tab, y+sp, dynPath.dir_disks, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step2;

				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, "Games:", (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT);
				CFreeTypeGX_DrawText(ttf16pt, x+tab, y+sp, dynPath.dir_games, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step2;

				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, "Codes:", (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT);
				CFreeTypeGX_DrawText(ttf16pt, x+tab, y+sp, dynPath.dir_codes, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step2;

				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, "TxtCodes:", (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT);
				CFreeTypeGX_DrawText(ttf16pt, x+tab, y+sp, dynPath.dir_txtcodes, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step2;

				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, "alt.DOL", (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT);
				CFreeTypeGX_DrawText(ttf16pt, x+tab, y+sp, dynPath.dir_altdol, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
			
				for(i=0; i<9; i++)
				{
				 Button_Paint(&button_edit[i]);
				 Button_Hover(&button_edit[i], pointer.p_x, pointer.p_y);
				}
			
			}
/*
		else if (page == 3 )
			{
				y = 115;
				sp = 0;
			}
*/
		
		sprintf(temp, "%d/%d", page, INFOPAGES );
		CFreeTypeGX_DrawText(ttf18pt, 420, 400, temp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);

		Button_TTF_Paint(&okButton);
		Button_Paint(&infoLeftButton);
		Button_Paint(&infoRightButton);
		
		
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		
		if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_B)	
			doloop = false;
		
		
		//if(page > 1) Button_Hover(&infoLeftButton, pointer.p_x, pointer.p_y);
		//if(page < INFOPAGES) Button_Hover(&infoRightButton, pointer.p_x, pointer.p_y);
		
		if (Button_Hover(&okButton, pointer.p_x, pointer.p_y) ||
			((page > 1) ? Button_Hover(&infoLeftButton, pointer.p_x, pointer.p_y) : false) ||
			((page < INFOPAGES) ? Button_Hover(&infoRightButton, pointer.p_x, pointer.p_y) : false))
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
		
		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
		{
			if(Button_Select(&okButton, pointer.p_x, pointer.p_y))
			{
				doloop = false;
			}
		
			else if ((page > 1) && Button_Select(&infoLeftButton, pointer.p_x,pointer.p_y))
			{
				page --;
			}

			else if ((page < INFOPAGES) && Button_Select(&infoRightButton, pointer.p_x,pointer.p_y))
			{
				page ++;
			}
			
		    else 
			{
				for(i=0; i<9; i++)
				{
				  if(Button_Select(&button_edit[i], pointer.p_x, pointer.p_y))
				  {
					SOUND_PlaySound(FX_BUTTON_CLICK, 0);
					gotoEditor(i);
				  }
				}
			}			
		}
		
		GRRLIB_Render();
		
	}while(doloop);


	// clean up after Loop
	for( i=0; i<9; i++)
	{
	 FreeButtonResources(&button_edit[i]);
	}
}

void gotoEditor(int val)
{
	int ret;
	
	switch( val )
	{
		case 0:
			strcpy(self.kb_buffer,dynPath.bootDevice);
			ret = showOSK("BootDevice");
			if (ret > 0 )
			{
			 strcpy(dynPath.bootDevice, self.kb_buffer);
			}
			break;
			
		case 1:
			strcpy(self.kb_buffer,dynPath.dir_usb_loader);
			ret = showOSK("usb-loader");
			if (ret > 0 )
			{
			 strcpy(dynPath.dir_usb_loader, self.kb_buffer);
			}
			break;
			
		case 2:
			strcpy(self.kb_buffer,dynPath.dir_3dcovers);
			ret = showOSK("3D-Cover");
			if (ret > 0 )
			{
			 strcpy(dynPath.dir_3dcovers, self.kb_buffer);
			}
			break;
			
		case 3:
			strcpy(self.kb_buffer,dynPath.dir_covers);
			ret = showOSK("2D-Cover");
			if (ret > 0 )
			{
			 strcpy(dynPath.dir_covers, self.kb_buffer);
			}
			break;
			
		case 4:
			strcpy(self.kb_buffer,dynPath.dir_disks);
			ret = showOSK("Disc");
			if (ret > 0 )
			{
			 strcpy(dynPath.dir_disks, self.kb_buffer);
			}
			break;
			
		case 5:
			strcpy(self.kb_buffer,dynPath.dir_games);
			ret = showOSK("Games");
			if (ret > 0 )
			{
			 strcpy(dynPath.dir_games, self.kb_buffer);
			}
			break;
			
		case 6:
			strcpy(self.kb_buffer, dynPath.dir_codes);
			ret = showOSK("Codes");
			if (ret > 0 )
			{
			 strcpy(dynPath.dir_codes, self.kb_buffer);
			}
			break;
			
		case 7:
			strcpy(self.kb_buffer, dynPath.dir_txtcodes);
			ret = showOSK("TxtCodes");
			if (ret > 0 )
			{
			 strcpy(dynPath.dir_txtcodes, self.kb_buffer);
			}
			break;
			
		case 8:
			strcpy(self.kb_buffer, dynPath.dir_altdol);
			ret = showOSK("alt.DOL");
			if (ret > 0 )
			{
			 strcpy(dynPath.dir_altdol, self.kb_buffer);
			}
			break;
	}

}
