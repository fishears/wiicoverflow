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

extern s_pointer pointer;
extern s_self self;

int showOSK(char *kbtitle)
{
	bool doloop = true;
	int fade = 5;
	int i, j;
	char temp[2];
	char kb_buf[256];
	int ret = 0;	//ESC
	
	strcpy(kb_buf, self.kb_buffer);
	self.kb_OK = false;
	
	Key Keys[4][11] = {
	{
		{'1','!'},
		{'2','@'},
		{'3','#'},
		{'4','$'},
		{'5','%'},
		{'6','^'},
		{'7','&'},
		{'8','*'},
		{'9','('},
		{'0',')'},
		{'\0','\0'}
	},
	{
		{'q','Q'},
		{'w','W'},
		{'e','E'},
		{'r','R'},
		{'t','T'},
		{'y','Y'},
		{'u','U'},
		{'i','I'},
		{'o','O'},
		{'p','P'},
		{'-','_'}
	},
	{
		{'a','A'},
		{'s','S'},
		{'d','D'},
		{'f','F'},
		{'g','G'},
		{'h','H'},
		{'j','J'},
		{'k','K'},
		{'l','L'},
		{':',';'},
		{'\'','"'}
	},

	{
		{'z','Z'},
		{'x','X'},
		{'c','C'},
		{'v','V'},
		{'b','B'},
		{'n','N'},
		{'m','M'},
		{',','<'},
		{'.','>'},
		{'/','?'},
		{'\0','\0'}
	}
	};

		
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
			{
				if(strlen(kb_buf) < 255)
				{
					kb_buf[strlen(kb_buf)] = ' ';
				}
			}
			else if(Button_Select(&kb_function[0], pointer.p_x, pointer.p_y))
			{ // Back
				if(strlen(kb_buf) > 0)
				{
					kb_buf[strlen(kb_buf)-1] = 0;
				}
			}
			else if(Button_Select(&kb_function[1], pointer.p_x, pointer.p_y))
			{ // Clear
			  strcpy( kb_buf, "" );
			}
			else if(Button_Select(&kb_function[2], pointer.p_x, pointer.p_y))
			{ // Caps
			
			}
			else if(Button_Select(&kb_function[3], pointer.p_x, pointer.p_y))
			{ // Shift
			
			}		   
		    else //if (true)
			{
				for(i=0; i<4; i++)
				{
					for(j=0; j<11; j++)
					{
						if(Keys[i][j].ch != '\0')
						{
						  if(Button_Select(&kb_key[i][j], pointer.p_x, pointer.p_y))
						  {
						   strcat( kb_buf,kb_key[i][j].ttf_label);
						  }
						}
					}
				}
			}

		}
		
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









