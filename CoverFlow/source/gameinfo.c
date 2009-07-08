#include "gameinfo.h"
#include "scrollbox.h"

extern s_self self;
extern s_pointer pointer;

//////////////
ScrollBox sb1;
//////////////

char *mytext[]= { 
"@devs:" ,
"",
"This is a demo for a TextBox with",
"autoScrolling text.",
"",
"I think we could use it for AboutWindow",
"or synopsis-entries from wiitdb.xml",
"or similar ones...",
"",
"This Control contents only the basics",
"and there is a lot 'todo'...",
"",
"Maybe our specialists could improve",
"this control for fine adjustment,",
"dynamic memory usage, same speed",
"with all video modes or adding",
"scollbars for manual scrolling...",
"",
"Regards,",
"   LoudBob11",
"",
"",
"...and REPEAT  -->",
"",
""
};



void showGameInfoWindow(){

	bool doloop = true;
	int fade = 5;
	int y, sp;
///////////////////////////////////////
	ScrollBox_Init( &sb1, 100, 140, 440, 150, 2, 0x000000FF, 0xFFFFFFFF );
	ScrollBox_SetText(&sb1, mytext, 25, 4, true);
///////////////////////////////////////
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
		
		CFreeTypeGX_DrawText(ttf16pt, 177, 63, "ScrollText Demo", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);

///////////////////////////////		
		ScrollBox_Paint(&sb1);
///////////////////////////////		
		
		GRRLIB_DrawImg(430, 80, menu_logo_texture, 0, 1, 1, 0xFFFFFFFF);
		
		okButton.x = 320 - 54;
		okButton.y = 340; //y+sp + step;

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
