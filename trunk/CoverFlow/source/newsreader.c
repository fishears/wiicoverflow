/* 
  *  newsreader.c 
  * 
  *  Wii CoverFloader 
  *  Copyright 2009 Beardface April 29th, 2009 
  *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11 
  *  Licensed under the terms of the GNU GPL, version 2 
  *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt 
 */ 
#include "newsreader.h"
#include "scrollbox.h"
#include "fileParser.h" 

#ifdef NEWS_READER 

#define FTGX_NULL				0x0000
#define FTGX_JUSTIFY_LEFT		0x0001
#define FTGX_JUSTIFY_CENTER		0x0002
#define FTGX_JUSTIFY_RIGHT		0x0004

#define FTGX_ALIGN_TOP			0x0010
#define FTGX_ALIGN_MIDDLE		0x0020
#define FTGX_ALIGN_BOTTOM		0x0040

#define FTGX_STYLE_UNDERLINE	0x0100
#define FTGX_STYLE_STRIKE		0x0200


extern s_self self;
extern s_pointer pointer;


ScrollBox sbnews;			// creates scrollBox



char *mytext[]= { 
"News",
"July 13th, 2009",
"",
"This is a demo for coming NewsReader.",
"If available, a downladed file called...",
/////////////////////////////
"'CoverFloader.news'",
"...is shown and scrolled by automatic.",
"",
"The file contains also a header section",
"with several format-settings like:",
//////////////////////////////
"       NEWSID",
"       BACKCOLOR",
"       TX_COLOR",
"       TX_COLORLINE",
"       TX_SIZE",
////////////////////////////
"       TX_SIZELINE",
"       TX_STYLE",
"       TX_STYLELINE",
"       SPEED",
"       REPEAT",
////////////////////////////
"       DELAY",
"       LINES",
"",
"(functions for reading format-settings aren't coded yet.)",
"",
////////////////////////////
"",
"...and now REPEAT"
};

// ttf_size ( // 14, 16, 18, 20, 24 ( 0 -> Default = 16 )
int myFormat[] = { 	24,14,24, 0, 0,
					20, 0, 0, 0, 0,
					18,18,18,18,18,
					18,18,18,18,18,
					18,18,24,14,24,
					24, 0 };

void showNewsWindow(){

	bool doloop = true;
	int fade = 5;
	int y, sp;
	int i;
	char temp[100];
	
	sprintf(temp,"NEWSID %s", self.newsID);
	
///////////////////////////////////////
	ScrollBox_Init( &sbnews, 40, 80, 560, 295, 2, 0x000000FF, 0xFFFFFFFF );
	ScrollBox_SetText(&sbnews, mytext, 27);
	ScrollBox_SetTextStyling( &sbnews, FTGX_JUSTIFY_LEFT);	//optional
	ScrollBox_SetTextFormat( &sbnews, myFormat, 27);		//optional
	ScrollBox_SetLineSpace( &sbnews, 3);					//optional	  
	ScrollBox_SetSpeed(&sbnews, 4);							//optional
	ScrollBox_SetRepeat(&sbnews, true);						//optional
	ScrollBox_SetStartPos(&sbnews, SB_TOP );				//optional	
	ScrollBox_SetDelay(&sbnews, 5 );						//optional
	ScrollBox_SetTextColor(&sbnews, (GXColor){0x00, 0x00, 0x00, 0xff} ); //optional
	// #1
	ScrollBox_SetTextColorLine(&sbnews, (GXColor){0xFF, 0x00, 0x00, 0xFF}, 1 ); //red
	ScrollBox_SetTextStylingLine(&sbnews, FTGX_JUSTIFY_CENTER | FTGX_STYLE_UNDERLINE, 1 );
	// #2
	ScrollBox_SetTextStylingLine(&sbnews, FTGX_JUSTIFY_RIGHT, 2 );
	// #6
	ScrollBox_SetTextColorLine(&sbnews, (GXColor){0x00, 0x00, 0xFF, 0xAA}, 6 ); //blue
	ScrollBox_SetTextStylingLine(&sbnews, FTGX_JUSTIFY_CENTER, 6 );
	// #11 - #22
	for (i=11; i<23; i++)
		ScrollBox_SetTextColorLine(&sbnews, (GXColor){0x00, 0x00, 0xFF, 0xFF}, i ); //darkblue

	
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
		
		GRRLIB_Rectangle(30, 48, 580, 384, 0xffffffdd, true);
		GRRLIB_Rectangle(32, 50, 576, 380, 0x737373FF, true);
		GRRLIB_DrawImg(200, 39, dialog_box_titlebar_long_texture, 0, 1, 1, 0xFFFFFFFF);
		
		CFreeTypeGX_DrawText(ttf16pt, 320 , 52, "News Reader", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);

		CFreeTypeGX_DrawText(ttf14pt, 40 , 70, temp, (GXColor){0x00, 0x00, 0x00, 0xFF}, FTGX_JUSTIFY_LEFT);

///////////////////////////////		
		ScrollBox_Paint(&sbnews);			//scrolls text in box
///////////////////////////////		
		
		//GRRLIB_DrawImg(430, 80, menu_logo_texture, 0, 1, 1, 0xFFFFFFFF);
		
		okButton.x = 320 - 54;
		okButton.y = 388;

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
			else if(ScrollBox_Select(&sbnews, pointer.p_x, pointer.p_y))
			{
				sbnews.selected = (sbnews.selected == true) ? false : true;
			}
////////////////////////////////////////////			
		}
		
		GRRLIB_Render();
		
	}while(doloop);
}



 bool newsFileExist() 
 { 
	 FILE *fp; 
	 fp = fopen(NEWSFILE, "r"); 
	 if(fp != NULL) 
	 { 
		fclose(fp); 
		return true; 
	 } 
	 else 
		return false; 
 } 


 
 void getNewsFormatData() 
 { 
  cfg_parsefile(NEWSFILE, &setNewsFormatData); 
 } 
  
  
 void setNewsFormatData(char *name, char *val) 
 { 
  /*  
     if (strcmp(name, "username") == 0) { 
                 strcopy(self.url_username, val, sizeof(self.url_username)); 
                 return; 
         } 
  
         if (strcmp(name, "password") == 0) { 
                 strcopy(self.url_password, val, sizeof(self.url_password)); 
                 return; 
         } 
  */
 } 




bool getNewsID(char *value)
{
	FILE *f;
	char *eq;
	char line[100], name[50], val[50];

	strcpy(value, "");
	f = fopen(NEWSFILE, "rt");
	if (!f) 
	{
		return false;
	}

	while (fgets(line, sizeof(line), f)) 
	{
		if (line[0] == '#') continue;
		eq = strchr(line, '=');
		if (!eq) continue;
		*eq = 0;
		trimcopy(name, line, sizeof(name));
		trimcopy(val, eq+1, sizeof(val));		
 	    if (strcmp(name, "NEWSID") == 0)
			{
			fclose(f);
			strcpy(value, val);
			return true;
			}
	}
	fclose(f);
	return false;
}
















#endif

