#include "info.h"

#define INFOPAGES 2

extern s_self self;
extern s_pointer pointer;
extern s_path dynPath;


void showInfoWindow(){
    int page = 1;
	int tab	= 130;
	bool doloop = true;
	int fade = 5;
	int y, sp;
	int step  = 35;
	int step2 = 25;
	char temp[256];
	int hdSize = self.freeSpace + self.usedSpace;
	int x = 92;
	
	u32 filledColor = 0xff9510FF;
	u32 emptyColor = 0xe4e4e4FF;

	okButton.x = 320 - 54;
	okButton.y = 380;
	
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

				sprintf(temp, "DeviceID: %u", self.deviceID);
				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, temp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step;

				sprintf(temp, "Version: %s (r%d)", RELEASE, SVN_VERSION);
				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, temp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
				sp+=step;
				
				sprintf(temp, "IOS Version: %d (rev%d)", IOS_GetVersion(), IOS_GetRevision());
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

				CFreeTypeGX_DrawText(ttf16pt, x, y+sp, "alt.Dol:", (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT);
				CFreeTypeGX_DrawText(ttf16pt, x+tab, y+sp, dynPath.dir_altdol, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
			}
/*
		else if (page == 3 )
			{
				y = 115;
				sp = 0;
			}
*/
		GRRLIB_DrawImg(430, 80, menu_logo_texture, 0, 1, 1, 0xFFFFFFFF);
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
		}
		
		GRRLIB_Render();
		
	}while(doloop);
}
