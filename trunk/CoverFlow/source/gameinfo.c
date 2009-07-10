#include "gameinfo.h"
#include "scrollbox.h"

extern s_self self;
extern s_pointer pointer;

//////////////
ScrollBox sb1;			// creates scrollBox
//////////////

char *mytext[]= { 
"@devs:" ,
"",
"This is a demo for a TextBox with",
"     autoScrolling text.",
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
"scrollbars for manual scrolling...",
"",
"Regards,",
"   LoudBob11",
"",
"",
"...and REPEAT  -->"
};

// ttf_size ( // 14, 16, 18, 20, 24 ( 0 -> Default = 16 )
int myFormat[] = { 	20, 0, 0,24, 0,
					 0, 0, 0, 0, 0,
					 0, 0,20,20,20,
					20,20, 0, 0,18,
					 0, 0,14};

void showGameInfoWindow(){

	bool doloop = true;
	int fade = 5;
	int y, sp;
///////////////////////////////////////
	ScrollBox_Init( &sb1, 100, 140, 440, 150, 2, 0x000000FF, 0xFFFFFFFF );
	ScrollBox_SetText(&sb1, mytext, 23);
	ScrollBox_SetTextStyling( &sb1, FTGX_JUSTIFY_LEFT);	//optional
	ScrollBox_SetTextFormat( &sb1, myFormat, 23);		//optional
	ScrollBox_SetLineSpace( &sb1, 3);					//optional	  
	ScrollBox_SetSpeed(&sb1, 4);						//optional
	ScrollBox_SetRepeat(&sb1, true);					//optional
	ScrollBox_SetStartPos(&sb1, SB_TOP );				//optional	
	ScrollBox_SetDelay(&sb1, 5 );						//optional
	ScrollBox_SetTextColor(&sb1, (GXColor){0xff, 0x00, 0x00, 0xaa} ); //optional
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
		ScrollBox_Paint(&sb1);			//scrolls text in box
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
////////////////////////////////////////////
			else if(ScrollBox_Select(&sb1, pointer.p_x, pointer.p_y))
			{
				sb1.selected = (sb1.selected == true) ? false : true;
			}
////////////////////////////////////////////			
		}
		
		GRRLIB_Render();
		
	}while(doloop);
}
