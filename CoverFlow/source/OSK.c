/*
 *  OSK.c
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */
#include "coverflow.h"
#include "OSK.h"

#ifdef OSK
#define STRMAX 256

extern s_pointer pointer;
extern s_self self;

Key Keys[4][11] = {
	{
		{'1','!','1'},
		{'2','@','2'},
		{'3','#','3'},
		{'4','$','4'},
		{'5','%','5'},
		{'6','^','6'},
		{'7','&','7'},
		{'8','*','8'},
		{'9','(','9'},
		{'0',')','0'},
		{'\0','\0','\0'}
	},
	{
		{'q','Q','Q'},
		{'w','W','W'},
		{'e','E','E'},
		{'r','R','R'},
		{'t','T','T'},
		{'y','Y','Y'},
		{'u','U','U'},
		{'i','I','I'},
		{'o','O','O'},
		{'p','P','P'},
		{'-','_','-'}
	},
	{
		{'a','A','A'},
		{'s','S','S'},
		{'d','D','D'},
		{'f','F','F'},
		{'g','G','G'},
		{'h','H','H'},
		{'j','J','J'},
		{'k','K','K'},
		{'l','L','L'},
		{';',':',';'},
		{'\'','"','\''}
	},

	{
		{'z','Z','Z'},
		{'x','X','X'},
		{'c','C','C'},
		{'v','V','V'},
		{'b','B','B'},
		{'n','N','N'},
		{'m','M','M'},
		{',','<',','},
		{'.','>','.'},
		{'/','?','/'},
		{'\0','\0','\0'}
	}
	};

bool Shift;
bool Caps;

int showOSK(char *kbtitle)
{
	bool doloop = true;
	int fade = 5;
	int i, j;
	char temp[2];
	char kb_buf[STRMAX];
	int ret = 0;	//ESC
        KEYBOARD_Init(NULL); //usb keyboard
	
	strcpy(kb_buf, self.kb_buffer);
	self.kb_OK = false;
	
	Shift = false;
	Caps  = false;
		
	Duplicate_Button_Key(&kb_function[0], kb_function[0], 10*42+90, 0*42+150, "Back");
	Duplicate_Button_Key(&kb_function[1], kb_function[0], 10*42+90, 4*42+150, "Clear");
	Duplicate_Button_Key(&kb_function[2], kb_function[0],  0*42+50, 2*42+150, "Caps");
	Duplicate_Button_Key(&kb_function[3], kb_function[0],  0*42+70, 3*42+150, "Shift");
	
	
	for(i=0; i<4; i++)
	{
		for(j=0; j<11; j++)
		{
			if(Keys[i][j].ch != '\0')
			{
			 temp[0] = Keys[i][j].ch;
			 temp[1] = '\0';
			 Duplicate_Button_Key(&kb_key[i][j], kb_key[0][0], j*42+21*i+90, i*42+150, temp);
			}
		}
	}

	
	kb_OK.x = 60;
	kb_OK.y = 420;
	kb_ESC.x = 472;
	kb_ESC.y = 420;
	
	do{
		
		WPAD_ScanPads();
		PAD_ScanPads();
		GetWiimoteData();
		
		draw_covers();
		GRRLIB_2D_Init();
		GRRLIB_FillScreen(0x00000000|fade);
		
		GRRLIB_Rectangle(30,  20, 580, 440, 0xffffffdd, true);
		GRRLIB_Rectangle(32,  22, 576, 436, 0x000000FF, true);
		GRRLIB_Rectangle(32, 410, 576,  48, 0x737373FF, true);
		
		GRRLIB_DrawImg(200, 10, dialog_box_titlebar_long_texture, 0, 1, 1, 0xFFFFFFFF);
		CFreeTypeGX_DrawText(ttf16pt, 320, 23, kbtitle, (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		
		GRRLIB_Rectangle(70, 70, 500, 40, 0xffffffdd, true);   // TextField
		CFreeTypeGX_DrawText(ttf18pt, 75, 98, kb_buf, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
		
		
		for(i=0; i<4; i++)
		{
			for(j=0; j<11; j++)
			{
				if(Keys[i][j].ch != '\0')
				{
					Button_Key_Paint(&kb_key[i][j]);
					Button_Hover(&kb_key[i][j], pointer.p_x, pointer.p_y);
				}
			}
			Button_Key_Paint(&kb_function[i]);
			Button_Hover(&kb_function[i], pointer.p_x, pointer.p_y);
		}

		Button_Key_Paint(&kb_space);
		Button_TTF_Paint(&kb_OK);
		Button_TTF_Paint(&kb_ESC);
		Button_Hover(&kb_space, pointer.p_x, pointer.p_y);
		
////////////  DirtyFix BEGIN
		if (((Shift == false) && (Caps == true )) || ((Shift == false) && (Caps == false))|| ((Shift == true) && (Caps == true)))
		{
			CFreeTypeGX_DrawText(ttf18pt, 549, 220, "-", (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
			CFreeTypeGX_DrawText(ttf18pt, 570, 260, "'", (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
		}
                else if((Shift==true) && (Caps==false))
		{
			CFreeTypeGX_DrawText(ttf18pt, 315, 177, "^", (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
			CFreeTypeGX_DrawText(ttf20pt, 400, 180, "*", (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
			CFreeTypeGX_DrawText(ttf18pt, 547, 220, "_", (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
			CFreeTypeGX_DrawText(ttf18pt, 570, 260, "\"", (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
		}
		
//////////// DirtyFix End	
	
		DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		
		
		if (Button_Hover(&kb_OK,  pointer.p_x, pointer.p_y) ||
			Button_Hover(&kb_ESC, pointer.p_x, pointer.p_y)   )
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
			if(Button_Select(&kb_OK, pointer.p_x, pointer.p_y))
			{
				strcpy(self.kb_buffer, kb_buf);
				self.kb_OK = true;
				ret = 1; //OK
				doloop = false;
			}
			
			else if(Button_Select(&kb_ESC, pointer.p_x, pointer.p_y))
			{
				strcpy(self.kb_buffer, "");
				self.kb_OK = false;
				ret = 0; //ESC
				doloop = false;
			}
			else if(Button_Select(&kb_space, pointer.p_x, pointer.p_y))
			{ // SPACE
				if(strlen(kb_buf) < STRMAX)
				{
					SOUND_PlaySound(FX_BUTTON_CLICK, 0);
					strcat(kb_buf, " ");
				}
			}
			else if(Button_Select(&kb_function[0], pointer.p_x, pointer.p_y))
			{ // Back
				if(strlen(kb_buf) > 0)
				{
					SOUND_PlaySound(FX_BUTTON_CLICK, 0);
					kb_buf[strlen(kb_buf)-1] = 0;
				}
			}
			else if(Button_Select(&kb_function[1], pointer.p_x, pointer.p_y))
			{ // Clear
			  int c;
			  SOUND_PlaySound(FX_BUTTON_CLICK, 0);
			  for (c = 0; c < STRMAX; c++) {
			     kb_buf[c] = 0; }
			}
			else if(Button_Select(&kb_function[2], pointer.p_x, pointer.p_y))
			{// Caps
				Caps = !Caps;
				SOUND_PlaySound(FX_BUTTON_CLICK, 0);
				setCharSet();
			}
			else if(Button_Select(&kb_function[3], pointer.p_x, pointer.p_y))
			{// Shift
				Shift = !Shift;
				SOUND_PlaySound(FX_BUTTON_CLICK, 0);
				setCharSet();
			}		   
		    else 
			{
				for(i=0; i<4; i++)
				{
					for(j=0; j<11; j++)
					{
						if(Keys[i][j].ch != '\0')
						{
						  if(Button_Select(&kb_key[i][j], pointer.p_x, pointer.p_y))
						  {
							SOUND_PlaySound(FX_BUTTON_CLICK, 0);
							strcat( kb_buf,kb_key[i][j].ttf_label);
							if ( Shift==true )
						    {
								Shift = false;
								setCharSet();
							}
						  }
						}
					}
				}
			}

		}
                //START handle USB Keyboards
                    keyboard_event ke;
                    s32 res = KEYBOARD_GetEvent(&ke);
                    char key[1];
                    if (res && (ke.type == KEYBOARD_PRESSED))
                    {
                        SOUND_PlaySound(FX_BUTTON_CLICK, 0);
                        if(ke.symbol == KS_Caps_Lock)
                        {
                            Caps = !Caps;
                            setCharSet();
                        }
                        if(ke.symbol == KS_Shift_R || ke.symbol == KS_Shift_L)
                        {
                            Shift = !Shift;
                            setCharSet();
                        }
                        if(ke.symbol == KS_Return)
                        {
                            strcpy(self.kb_buffer, kb_buf);
                            self.kb_OK = true;
                            ret = 1; //OK
                            doloop = false;
                        }
                        else if(ke.symbol == KS_BackSpace)
                        {
                            if(strlen(kb_buf) > 0)
                            {
                                kb_buf[strlen(kb_buf)-1] = 0;
                            }
                        }
                        else if(ke.symbol == KS_Escape)
                        {
                            strcpy(self.kb_buffer, "");
                            self.kb_OK = false;
                            ret = 0; //ESC
                            doloop = false;
                        }
                        else if (((ke.symbol >> 8) & 0xFF) == 0) //handle characters
                        {
                            sprintf(key,"%c",ke.symbol &0xff);
                            strcat(kb_buf,key);
                        }
                    }
                    else if(res && (ke.type == KEYBOARD_RELEASED))
                    {
                        if(ke.symbol == KS_Shift_R || ke.symbol == KS_Shift_L)
                        {
                            Shift = !Shift;
                            setCharSet();
                        }
                    }

                //END handle USB Keyboards
		GRRLIB_Render();

	}while(doloop);


// clean up after Loop

	for(i=0; i<4; i++)
	{
		for(j=0; j<11; j++)
		{
			if(Keys[i][j].ch != '\0')
			{
			  FreeButtonResources(&kb_key[i][j]);
			}
		}
		FreeButtonResources(&kb_function[i]);
	}
	return ret;
}



void setCharSet()
{
	int i,j;
	char temp[2];
	
	for(i=0; i<4; i++)
	{
		for(j=0; j<11; j++)
		{
			if(Keys[i][j].ch != '\0')
			{
                         if((Caps == true) && (Shift ==false))
				temp[0] = Keys[i][j].chCaps;
                         else if((Shift == true) && (Caps ==false))
                                temp[0] = Keys[i][j].chShift;
                         else
                                temp[0] = Keys[i][j].ch;
			 temp[1] = '\0';
			 strcpy(kb_key[i][j].ttf_label, temp);
			}
		}
	}
}
#endif



