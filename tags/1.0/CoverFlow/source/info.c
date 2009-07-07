#include "info.h"

extern s_self self;
extern s_pointer pointer;

void showInfoWindow(){

	bool doloop = true;
	int fade = 5;
	int y, sp;
	int step = 35;
	char temp[256];
	int hdSize = self.freeSpace + self.usedSpace;
	int x = 92;
	
	u32 filledColor = 0xff9510FF;
	u32 emptyColor = 0xe4e4e4FF;

	do{
		y = 115;
		sp = 0;
		
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
		
		GRRLIB_DrawImg(430, 80, menu_logo_texture, 0, 1, 1, 0xFFFFFFFF);
		
		okButton.x = 320 - 54;
		okButton.y = y+sp + step;

		Button_TTF_Paint(&okButton);
		
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		
		if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_B)	
			doloop = false;
		
		if (Button_Hover(&okButton, pointer.p_x, pointer.p_y))
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
		}
		
		GRRLIB_Render();
		
	}while(doloop);
}
