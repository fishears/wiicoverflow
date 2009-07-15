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


void showNewsWindow(){

	bool doloop = true;
	int fade = 5;
	int y, sp;
	char temp[100];
	
	sprintf(temp,"NEWSID %s", self.newsID);
	
	ScrollBox_Init( &sbnews, 40, 80, 560, 295, 2, 0x000000FF, 0xFFFFFFFF );
	ScrollBox_SetStartPos(&sbnews, SB_TOP );
	getNewsFormatData() ;
	
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
	
		ScrollBox_Paint(&sbnews);			//scrolls text in box
		
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
			else if(ScrollBox_Select(&sbnews, pointer.p_x, pointer.p_y))
			{
				sbnews.selected = (sbnews.selected == true) ? false : true;
			}
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
  // SPEED
  if (strcmp(name, "SPEED") == 0)
	{ 
	 ScrollBox_SetSpeed(&sbnews, atoi(val));				
	 return; 
	} 
  
  // DELAY
  if (strcmp(name, "DELAY") == 0)
	{ 
	 ScrollBox_SetDelay(&sbnews, atoi(val));				
	 return; 
	} 
  
  // LINES
  if (strcmp(name, "LINES") == 0)
	{ 
	 ScrollBox_SetLines(&sbnews, atoi(val));				
	 return; 
	} 
  
  // REPEAT
  if (strcmp(name, "REPEAT") == 0)
	{ 
	 if ( atoi(val) > 0 ) 
		 ScrollBox_SetRepeat(&sbnews, true);
	 else
		 ScrollBox_SetRepeat(&sbnews, false);
	 return; 
	} 
  
  // BACKCOLOR
  if (strcmp(name, "BACKCOLOR") == 0)
	{ 
	 u32 bc = strtoul(val,NULL,0);
	 ScrollBox_SetBackColor(&sbnews, (u32)bc);				
	 return; 
	} 
  
  // TX_SIZE
  if (strcmp(name, "TX_SIZE") == 0)
	{ 
	 ScrollBox_SetTextFormatDef(&sbnews, atoi(val));	
	 return; 
	} 
  
  // TX_STYLE
  if (strcmp(name, "TX_STYLE") == 0)
	{ 
	 if (strcmp(val, "FTGX_JUSTIFY_LEFT") == 0)
	   {
	    ScrollBox_SetTextStyling( &sbnews, 	FTGX_JUSTIFY_LEFT );
	    return;
	   }
	 if (strcmp(val, "FTGX_JUSTIFY_CENTER") == 0)
	   {
	    ScrollBox_SetTextStyling( &sbnews, 	FTGX_JUSTIFY_CENTER );
	    return;
	   }
	 if (strcmp(val, "FTGX_JUSTIFY_RIGHT") == 0)
	   {
	    ScrollBox_SetTextStyling( &sbnews, 	FTGX_JUSTIFY_RIGHT );
	    return;
	   }
	 ScrollBox_SetTextStyling( &sbnews, FTGX_JUSTIFY_LEFT );
	 return; 
	} 
  
  //TX_STYLELINE
  if (strcmp(name, "TX_STYLELINE") == 0)
	{ 
	 int cnt;
	 int line;
	 char buff[100];
		
	 cnt = sscanf(val, "%d, %s", &line, buff);
	 if (cnt == 2)
		{
		 if (strcmp(buff, "FTGX_JUSTIFY_LEFT") == 0)
		   {
			ScrollBox_SetTextStylingLine( &sbnews, FTGX_JUSTIFY_LEFT, line );
			return;
		   }
		 if (strcmp(buff, "FTGX_JUSTIFY_CENTER") == 0)
		   {
			ScrollBox_SetTextStylingLine( &sbnews, FTGX_JUSTIFY_CENTER, line );
			return;
		   }
		 if (strcmp(buff, "FTGX_JUSTIFY_RIGHT") == 0)
		   {
			ScrollBox_SetTextStylingLine( &sbnews, FTGX_JUSTIFY_RIGHT, line );
			return;
		   }
		}
	 return; 
	}   
  
  
  // TX_COLOR
  if (strcmp(name, "TX_COLOR") == 0)
	{ 
		int cnt;
		int R,G,B,A;
		
		cnt = sscanf(val, "0x%x, 0x%x, 0x%x, 0x%x", &R, &G, &B, &A);
		if (cnt == 4)
			{
			 ScrollBox_SetTextColor(&sbnews, (GXColor){R, G, B, A} );
			}
		else
			{
			 ScrollBox_SetTextColor(&sbnews, (GXColor){0x00, 0x00, 0x00, 0xff} );
			}
		return;
	}

  // TX_COLORLINE
  if (strcmp(name, "TX_COLORLINE") == 0)
	{ 
		int line;
		int cnt;
		int R,G,B,A;
		
		cnt = sscanf(val, "%d, 0x%x, 0x%x, 0x%x, 0x%x",&line, &R, &G, &B, &A);
		if (cnt == 5)
			{
			 ScrollBox_SetTextColorLine(&sbnews, (GXColor){R, G, B, A}, line );
			}
		return;
	}

  //TX_SIZELINE 
  if (strcmp(name, "TX_SIZELINE") == 0)
	{ 
		int cnt;
		int line;
		int size;
		
		cnt = sscanf(val, "%d, %d", &line, &size);
		if (cnt == 2)
			{
			 ScrollBox_SetTextFormatLine( &sbnews, size, line );
			}
		return;
	}
	
  //TX_TABLINE
  // Must be added behind reading lines with content of text
  if (strcmp(name, "TX_TABLINE") == 0)
	{ 
		char tmp[120] = "";
		int cnt;
		int line;
		int blanks;
		int i;
		
		cnt = sscanf(val, "%d, %d", &line, &blanks);
		if (cnt == 2)
			{
			 if (blanks > (SB_MAXCHARS -10)) blanks = SB_MAXCHARS -10;
			 if (blanks < 0) blanks = 0;
			 for (i=0;i<blanks;i++)
				strcat(tmp, " ");
			 strcat(tmp, sbnews.ttf_text[line-1]);
			 strcopy(sbnews.ttf_text[line-1],tmp, SB_MAXCHARS);
			}
		return;	
	}  
  
	
  // TEXTLINE
  if ( atoi(name) > 0 )
    {
	 int line = atoi(name);
	 ScrollBox_SetTextLine( &sbnews, val, line );
	 return;
	}
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



bool blinking( int Aon, int Aoff, int Bon, int Boff, int Con, int Coff )
{
	static long tic = 0;
	
	int a1 = Aon;
	int a2 = a1 + Aoff;
	int b1 = a2 + Bon;
	int b2 = b1 + Boff;
	int c1 = b2 + Con;
	int c2 = c1 + Coff;
		
	tic++;
	
    if ( tic <= a1 ) 			  return true;
	if ( tic > a1 && tic <= a2 ) return false;
	if ( tic > a2 && tic <= b1 ) return true;
	if ( tic > b1 && tic <= b2 ) return false;
	if ( tic > b2 && tic <= c1 ) return true;
	if ( tic > c1 && tic <= c2 ) return false;
    if ( tic > c2 ) tic = 0;
		
	return false;
}

#endif

