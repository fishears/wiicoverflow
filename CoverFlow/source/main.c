/*
 *  main.c
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11, alexcarlosantao
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 *  This file contains the main() entry point to the application
 *  Build configuration settings can be found in "coverflow.h"
 */
#include "coverflow.h"
#include "filter.h"
	
extern int COVER_COUNT;
#ifdef TEST_MODE
int COVER_COUNT = 29;
#else
int COVER_COUNT = 0;
#endif

extern u8 shutdown;
extern u8 reset;
extern s_settings settings;

s_title       *titleList;
s_self         self;
s_pointer      pointer;
s_gameSettings gameSetting;
s_coverFlip    coverFlip[500];
//static s_Filter gameFilter;

void initVars()
{
	pointer.p_x = 0;
	pointer.p_y = 0;
	pointer.p_ang = 0;
	pointer.p_type = 0;
	
	self.shift = 0;
	self.select_shift = 0;
	self.gameCnt = 0;
    self.gameSelected=0;
    self.gameStart = 0;
	self.selected = false;
	self.animate_flip = 0.0;
	self.animate_rotate = 0.0;
	self.array_size = 0;
	self.rumbleAmt = 0;
	self.progress = 0.0;
	//self.firstTimeDownload = true;
	//self.inetOk = false;
	self.dummy = 0;
	self.gsize = 0;
	self.my_wbfsDev = WBFS_DEVICE_USB;
	self.hdd = NULL;
	self.gameList = NULL;
	//self.max_cover = 1;
	self.max_cover = 0;
	self.min_cover = 0;
	self.slot_glow = 0;
	self.scroll_speed = 0.050;
	strcpy(self.ipAddress, "000.000.000.000");
	
	initGameSettings(&gameSetting);
}


//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
int main( int argc, char **argv )
{

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
#ifndef TEST_MODE
	int ret;
	
	/* Load Custom IOS */
    //ret = IOS_ReloadIOS(222);
	
    //if (ret < 0) 
//	{
        ret = IOS_ReloadIOS(249);
//    }

	/* Check if Custom IOS is loaded */
	if (ret < 0)
	{
		printf("[+] ERROR:\n");
		printf("    Custom IOS could not be loaded! (ret = %d)\n", ret);
		return 0;
	}
#endif
	u8 FPS = 0; // frames per second counter
	languageInit();     // loads default msgs
	SETTINGS_Init();
	LoadFonts();
	initVars();
	
	GRRLIB_Init();
    GRRLIB_FillScreen(0x000000FF);
    GRRLIB_Render();
	
    progress_step_texture = GRRLIB_LoadTexture(progress_step_png);
    progress_bar_texture  = GRRLIB_LoadTexture(progress_bar_png);

	self.progress += .1;
	sprintf(self.debugMsg, "Loading textures" );

	Paint_Progress(self.progress,self.debugMsg);
	
	ClearBufferSlotMemory();
	LoadTextures();		// load textures
	Init_Buttons();		// load buttons so they can be used for error msgs

	self.progress += .1;
	sprintf(self.debugMsg, "Init WPAD" );
	Paint_Progress(self.progress,self.debugMsg);

	sprintf(self.debugMsg, "Init USB" );
	Paint_Progress(self.progress,self.debugMsg);

#ifndef TEST_MODE
	if(!init_usbfs())
	{
		WindowPrompt("ERROR!", "Cannot init USBFS, quitting.", &okButton, 0);
		return 0;
	}
	
	//self.progress += .1;
	sprintf(self.debugMsg, "Initializing FileSystem" );
	Paint_Progress(self.progress,self.debugMsg);
	
	self.my_wbfsDev = WBFS_DEVICE_USB;
	
	checkDirs();
	checkFiles();
	Sys_Init();
	Subsystem_Init();
	// I'll have to read a fair bit of code but this should be done as early as possible
	initWBFS();
	SOUND_Init();
	// the pad needs to be init after a usb retry but before anything else
	PAD_Init();
        /* Initialize Wiimote subsystem */
	Wpad_Init();
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

//moved here as it sets the self.usingTitlesTxt used in initUSBFS sorting
	int numLines = initTitle();
	if(numLines > 0){
		self.usingTitlesTxt = true;
		self.titlesTxtSize = numLines;
		titleList = (s_title *) malloc (numLines * sizeof(s_title));
		fillTitleStruct(titleList, numLines);
	}

	initUSBFS();
	
#else
	PAD_Init();
	self.gameCnt = 29;
#endif
	
#ifndef TEST_MODE
	SETTINGS_Load();	// load user settings from xml file in SD:/usb-loader/
#endif

	
	
	BUFFER_InitBuffer();
	InitializeBuffer(self.gameList, self.gameCnt,BUFFER_WINDOW,COVER_COUNT/2.0 +self.shift,settings.covers3d);
//	UpdateBufferedImages();
	
	float wait = 10; //ms
	float prog = 2.1/wait;
	
	sprintf(self.debugMsg, "Initializing Threaded Image Buffer..." );	
	while(wait > 0)
	{
		wait--;
		self.progress += prog;
		Paint_Progress(self.progress, self.debugMsg);
		Sleep(10);
	}
	

	// set the background
	sprintf(self.debugMsg, TX.setBackground );
	Paint_Progress(self.progress,self.debugMsg);
	if (settings.theme)
	{	// black fonts for white theme
		settings.fontColor = 0xFFFFFFFF; //temp until I fix the dialogs for the white theme
		//settings.fontColor = 0x000000FF;
		GRRLIB_SetBGColor(1); // set BG to white
	}
	else
	{   // white fonts for black theme
		settings.fontColor = 0xFFFFFFFF;
		GRRLIB_SetBGColor(0);  // set BG to black
	}

	self.selected        = false;
	bool select_ready    = false;
	bool dragging        = false;
	bool twisting        = false;
	self.animate_count   = 50;
	self.animate_slide_x = 0;
	self.animate_flip    = 0;
	
	GRRLIB_FillScreen(0x000000FF);
	GRRLIB_Render();
#ifndef TEST_MODE
    ios_version_check(); //Warn if cIOS is less than REQUIRED_IOS_REV
#endif

	//ee();
	languageLoad();		    // load localization 
	Label_Buttons();		// Localize buttons
#ifdef ONE_AT_A_TIME
        bool LEFT = false, RIGHT = false;
        int L_CNT = 0, R_CNT = 0;
#endif
	//get last game played and select it as centre cover at startup
	if(strcmp(settings.lastplayed,"")!=0){
		int i;
		char titleID[7];
		for(i=0;i<=self.gameCnt;i++){
			struct discHdr *header = &self.gameList[i];
			sprintf(titleID, "%s", header->id);
			if(strcmp(titleID,settings.lastplayed)==0)
				self.shift = (COVER_COUNT/2)-i;
		}
	}

	//WPAD_ScanPads();
	//GetWiimoteData();
	//if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_1){
		//ee();
	//}


	//////////////////////////
	// main screen gui loop //
	//////////////////////////
	while(1) 
	{
	  START_LOOP:
	#ifndef TEST_MODE
		WPAD_ScanPads();
		GetWiimoteData();
	#endif //_TEST_MODE
		PAD_ScanPads();
		twisting = false;
	
	
		if(CheckKonami())
		{
			WindowPrompt("Konami Code!!", "Click OK to enable GC Backups..." , &okButton, 0);
			
			int c = 0;
			for(;c < 25; c++)
			{
				Sleep(100);
				Paint_Progress_Generic(c, 27,"Enabling GC Backup Support...");
			}	
			
			Sleep(2000);
			
			for(;c >= 0; c--)
			{
				Sleep(100);
				Paint_Progress_Generic(c, 27,"Enabling GC Backup Support...");
			}	
			
			ee();
			goto START_LOOP;
		}
	
		// Check for 'HOME' button press
		if((WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) || (PAD_ButtonsDown(0) & PAD_TRIGGER_Z))
		{
			SOUND_PlaySound(FX_COVER_FLIP, 0);
			SETTINGS_Save();
			HomeMenu_Show();
					
			WPAD_Rumble(0,0);
			self.rumbleAmt = 0;
		}
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B || PAD_ButtonsDown(0) & PAD_BUTTON_B)
		{
			if(self.selected && self.animate_flip >= 1.0)
				self.selected = false;
		}
		
		if(dragging) // if the user is dragging the slider
		{
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_A) //with the A button
				DragSlider(pointer.p_x);
			else
				dragging = false; // they let go
		}
		
	 	// Check for the A button action
		else if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A || PAD_ButtonsDown(0) & PAD_BUTTON_A)
		{
			//First Check if any UI buttons or slider are selected
			if((!settings.parentalLock) && (Button_Select(&addButton, pointer.p_x, pointer.p_y) || PAD_ButtonsDown(0) & PAD_TRIGGER_R))
			{
				Menu_Install();
			}
			else if((!settings.parentalLock) && Button_Select(&settingsButton, pointer.p_x, pointer.p_y))
			{
				Settings_Menu_Show();
			}
			else if(Button_Select(&slideButton, pointer.p_x, pointer.p_y))
			{
				dragging = true;
			}
			else // user clicked A somewhere on the screen
			{
				if(self.gameCnt) // any games?
				{
					if(!self.selected && self.animate_flip <= 0.0)
					{
						// nothing yet selected and nothing animating
						// Check to see if in the cover area
						if(CoverHoverCenter() || PAD_ButtonsDown(0) & PAD_BUTTON_A)
						{
							if(select_ready && self.select_shift == 0.0)
							{
								// the center cover was selected so set the flag and load the game texture into buffer
								SOUND_PlaySound(FX_COVER_FLIP, 0);
								self.selected = true;
								
								f32 size;
								struct discHdr *header = NULL;
								
								header = &self.gameList[self.gameSelected];
								WBFS_GameSize(header->id, &size);
								self.gsize = size;
								
								LoadCurrentCover(self.gameSelected, self.gameList);
								
							}
						}
						else if(CoverHoverLeft())
						{
							// the pointer is within the left set of covers, start to scroll
							// TODO this still needs work, un-hard code and take cover spacing into account
							float fx_l_edge  = change_scale(settings.coverZoom, -8.0, 0.69, 206.0, 130.0);
							float shift_move = change_scale(pointer.p_x, -100, fx_l_edge, 5.0, 1.0); 
							self.select_shift = (-1)*(shift_move);
						}
						else if(CoverHoverRight())
						{
							// the pointer is within the right set of covers, start to scroll
							// TODO this still needs work, un-hard code and take cover spacing into account
							float fx_r_edge  = change_scale(settings.coverZoom, -8.0, 0.69, 350.0, 430.0);
							float shift_move = change_scale(pointer.p_x, fx_r_edge, 660, 1.0, 5.0); 
							self.select_shift = shift_move;
						}
					}
					// Game is selected and finished animating the launch game dialog
					if(self.selected && self.animate_flip == 1.0)
					{
						// Check the buttons
						if(Button_Select(&loadButton, pointer.p_x, pointer.p_y) || PAD_ButtonsDown(0) & PAD_BUTTON_A) // load
						{
						if(!(settings.parentalLock && gameSetting.lock))
                                                {
                                                    // User clicked on the load game, so save settings before launching
							struct discHdr *header = &self.gameList[self.gameSelected];
							char titleID[7];
							sprintf(titleID, "%s", header->id);
                                                        strcpy(settings.lastplayed,titleID); //save this game as last game played
                                                        SETTINGS_Save();
                                                                                    if(getGameSettings(titleID, &gameSetting))
                                                                                            apply_settings();
                                                                                    setGameSettings(titleID, &gameSetting,1);
                                                        WiiLight(0); // turn off the slot light
							
							
							
                                                        if(!LaunchGame())
                                                        {
                                                                SETTINGS_Load(); //failed to launch so get the globals back
                                                                return 0;
                                                        }
                                                }
						}
						else if((!settings.parentalLock) && Button_Select(&deleteButton, pointer.p_x, pointer.p_y)) // delete
						{
							// User clicked delete button
							Menu_Delete();
                            //self.selected = false;
						}
						else if(Button_Select(&backButton, pointer.p_x, pointer.p_y)) // back
						{
							// User clicked back button
							self.selected = false;
						}
						else if(!settings.parentalLock && Button_Select(&gsettingsButton, pointer.p_x, pointer.p_y))
                        {
							//clicked settings button on launch screen
                            Game_Settings_Menu_Show();
                        }
						else if(Button_Select(&bookmarkOnButton, pointer.p_x, pointer.p_y) || Button_Select(&bookmarkOffButton, pointer.p_x, pointer.p_y))
						{	
							self.dummy ^= 1;
						}
					}
				}
			}
		}

		// Check for non-A activity
		// Nothing is selected and nothing is flipped
		else if (!self.selected && self.animate_flip == 0)
		{       // Check for LEFT, flip cover left
                        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT || PAD_ButtonsHeld(0) & PAD_BUTTON_LEFT)
			{	
				select_ready = false;
				if ((int)self.shift < self.max_cover)
					self.shift += self.scroll_speed;
				else if ((int)self.shift >= self.max_cover)
					self.shift = self.max_cover;
				else if ((int)self.shift <= self.min_cover)
					self.shift = self.min_cover;
			} // now check for right, flip cover right
                        else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT ||	PAD_ButtonsHeld(0) & PAD_BUTTON_RIGHT)
			{
				select_ready = false;
				if ((int)self.shift > self.min_cover)
					self.shift -= self.scroll_speed;
				else if ((int)self.shift >= self.max_cover)
					self.shift = self.max_cover;
				else if ((int)self.shift <= self.min_cover)
					self.shift = self.min_cover;
			}// Check for MINUS, flip cover left
#ifdef ONE_AT_A_TIME
                        else if (LEFT || WPAD_ButtonsHeld(0) & WPAD_BUTTON_MINUS)
			{	
                            LEFT = true;
                            RIGHT = false;
                            select_ready = false;
				if ((int)self.shift < self.max_cover){
                                    self.shift += self.scroll_speed;
                                    L_CNT++;
                                    if(L_CNT==19){LEFT=false;L_CNT=0;}
                                }
				else if ((int)self.shift >= self.max_cover)
                                {self.shift = self.max_cover; LEFT = false;L_CNT=0;}
				else if ((int)self.shift <= self.min_cover)
                                {self.shift = self.min_cover; LEFT = false;L_CNT=0;}
			} // now check for PLUS, flip cover right
			else if (RIGHT || WPAD_ButtonsHeld(0) & WPAD_BUTTON_PLUS)
			{
                            RIGHT = true;
                            LEFT = false;
				select_ready = false;
				if ((int)self.shift > self.min_cover){
					self.shift -= self.scroll_speed;
                                        R_CNT++;
                                        if(R_CNT==19){RIGHT=false;R_CNT=0;}
                                    }
                                else if ((int)self.shift >= self.max_cover)
                                {self.shift = self.max_cover; RIGHT = false;R_CNT=0;}
				else if ((int)self.shift <= self.min_cover)
                                {self.shift = self.min_cover; RIGHT = false;R_CNT=0;}
			}
#endif
                        // Check for UP button held to zoom in
			else if (!settings.parentalLock && (WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP || PAD_ButtonsHeld(0) & PAD_BUTTON_UP))
			{	
				if (settings.coverZoom >= .69)
					settings.coverZoom = .69; // limit how far we can zoom in
				else
					settings.coverZoom += 0.03; // zoom in
			} // Check for DOWN button held to zoom out
			else if (!settings.parentalLock && (WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN || PAD_ButtonsHeld(0) & PAD_BUTTON_DOWN))
			{	
				if (settings.coverZoom <= -8.0)
					settings.coverZoom = -8.0; // limit how far we can zoom out
				else
					settings.coverZoom -= 0.03; // zoom out
			} // Check for B to control flow with wrist twist
			else if (((WPAD_ButtonsHeld(0) & WPAD_BUTTON_B) || (PAD_ButtonsHeld(0) & PAD_BUTTON_B)) && (self.gameCnt > 1))
			{	
				pointer.p_type = 1; //set cursor to rotating hand

				if ((self.orient.roll < -10.0) || (self.orient.roll > 10.0)) // check for movement out of the -10.0 to 10.0 deg range (dead soze)
				{
					if ( ((self.shift > self.min_cover) && (self.shift < self.max_cover)) ||
						((self.shift == self.min_cover) && (self.orient.roll < 0)) ||
						((self.shift == self.max_cover) && (self.orient.roll > 0)) )
					{
						self.shift -= change_scale(self.orient.roll, -90.0, 90.0, -0.3, 0.3);
						twisting = true;
					}
					else if (self.shift >= self.max_cover)
					{
						self.shift = self.max_cover;
						twisting = true;
					}
					else if (self.shift <= self.min_cover)
					{
						self.shift = self.min_cover;
						twisting = true;
					}
				}
				if (settings.enablepitch)
				{
					if ((self.orient.pitch > 5.0) || (self.orient.pitch < -5.0)) // check for movement out of the -20.0 to -30.0 deg range (dead soze)
					{
						if (self.orient.pitch < -5.0)
						{
							if (settings.coverZoom < -6.0) // pitch back (zoom out)
								settings.coverZoom = -6.0; // limit how far we can zoom out
							else
								settings.coverZoom -= change_scale(self.orient.pitch, -180.0, -5.0, -0.06, 0.06);
						}
						if (self.orient.pitch > 5.0)
						{
							if (settings.coverZoom > 0.69) // pitch forward (zoom in)
							{
								if (settings.enablepitch) // limit how far we can zoom in
									settings.coverZoom = 0.69;
							}
							else
							{
								if (settings.enablepitch)
									settings.coverZoom -= change_scale(self.orient.pitch, 5, 180.0, -0.06, 0.06);
							}
						}
					}
				}
			}
			else if ( ((WPAD_ButtonsHeld(0) & WPAD_BUTTON_1) && (WPAD_ButtonsDown(0) & WPAD_BUTTON_UP)) || (PAD_ButtonsDown(0) & PAD_BUTTON_X))// Check for button 1 hold
			{
				// Take a screen shot
				GRRLIB_ScrShot(USBLOADER_PATH "/sshot.png");

				//Beardfaces stuff
					
			
				//
				// SCOGNITO'S TRASH TEST CORNER
				//
				//�;
				/*
				char imgPath[256];
				snprintf(imgPath, sizeof(imgPath), "%s/covers/%s.png", USBLOADER_PATH, "zucc");
					
				FILE* fp = fopen(imgPath, "rb");
				fseek(fp, 0, SEEK_END);
				int size=ftell (fp);
				fclose (fp);
				
				sprintf(imgPath, "filesize: %d", size);
				WindowPrompt("Titolo", imgPath, 0, &cancelButton);
				*/
				/*
				if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
					WindowPrompt("Titolo", "16:9", 0, &cancelButton);
				else
					WindowPrompt("Titolo", "4:3", 0, &cancelButton);
				*/
				//LoadCurrentCover(self.gameSelected, gameList);
				
				/*
				sprintf(self.debugMsg, TX.iniNet );
				
				char myIP[16];
				if(!Net_Init(myIP))
				{
					sprintf(self.debugMsg, TX.iniNetErr );
					
					WindowPrompt (TX.error, TX.iniNetErr, &okButton, 0);
				}
				else
				{
					sprintf(self.debugMsg, TX.iniNetReady );
					
					self.inetOk = true;
				}
						
						struct block file = downloadfile("http://www.theotherzone.com/wii/diskart/160/160/zuc2.png");
						if(file.data != NULL && file.size != 7386){
							
							char imgPath[256];
							snprintf(imgPath, sizeof(imgPath), "%s/disks/%s.png", USBLOADER_PATH, "zucc");
							saveFile(imgPath, file);
							free(file.data);
							//sprintf(imgPath, "size: %d", file.size);
							WindowPrompt("Trovato", imgPath, 0, &okButton);
						}
						else{
							WindowPrompt("NON Trovato", "ma quali", 0, &okButton);
						}
				*/
						/*
						self.gsize = getGameSize(gameList);
						char sss[70];
						float gsize = 2.4;
						sprintf(sss, "Size:    %.2fGB", gsize);
						WindowPrompt("TITOLO", sss, 0, &cancelButton);
						*/
						//PAD_BUTTON
						/*Hitting 1 causes crash right now...*/
						//sysdate();
						//quit();
						//WindowPrompt("Titolo", "This is a long message using\n\the character \\n as escape sequence.\nAlso now buttons title and message are\naligned now. :)", &okButton, &cancelButton);
			}
			else if ((WPAD_ButtonsDown(0) & WPAD_BUTTON_2) || (PAD_ButtonsDown(0) & PAD_BUTTON_Y))// Check for button 1 hold
			{
				if(settings.covers3d)
				{
					if(CoverHoverCenter() && select_ready)
					{
						coverFlip[self.gameSelected].flip = !(coverFlip[self.gameSelected].flip);
					}
				}	
			}
			else
			{
				if(abs(self.select_shift) > self.scroll_speed)
				{
					select_ready = false;
					int mult = abs((int)self.select_shift);
					if(self.select_shift > 0)
					{
						self.select_shift -= mult*self.scroll_speed;
						if(!(self.shift <= self.min_cover))
							self.shift -= mult*self.scroll_speed;
					}
					else
					{
						self.select_shift += mult*self.scroll_speed;
						if(!(self.shift >= self.max_cover))
							self.shift += mult*self.scroll_speed;
					}
					
				}
				else if(abs(((int)self.shift * 10000.0) - (self.shift*10000.0))/10000.0 > (self.scroll_speed + self.scroll_speed/2.0))
				{
					select_ready = false;
					if((int)((int)(self.shift+0.5) - (int)self.shift) == 0)
						self.shift -= self.scroll_speed;
					else
						self.shift += self.scroll_speed;
				}
				else
				{
					self.select_shift = 0;
					self.shift = (int)self.shift;
					select_ready = true;
				}
			}
		}
		
		// Check for parental lock button combo A + B + 1 + 2
		if ((WPAD_ButtonsHeld(0) & WPAD_BUTTON_A) &&
			(WPAD_ButtonsHeld(0) & WPAD_BUTTON_B) &&
			(WPAD_ButtonsHeld(0) & WPAD_BUTTON_1) &&
			(WPAD_ButtonsHeld(0) & WPAD_BUTTON_2))
		{
			if (WindowPrompt(TX.parentalCtrl, TX.enableParentCtrl, &yesButton, &noButton))
				settings.parentalLock = 1;
			else
				settings.parentalLock = 0;
		}
	
		UpdateBufferedImages();
		
		// This is the main routine to draw the covers
		draw_covers();

		//Play flip sound if needed
		if((int)(self.shift+1000.5) != self.lastGameSelected)
		{
			self.lastGameSelected = (int)(self.shift+1000.5);
			SOUND_PlaySound(FX_COVER_SCROLL, 0);
		}
		
		// Check to see if it's time to draw the game launch dialog panel
		if(self.selected || self.animate_flip != 0)
		{
		
			if (settings.quickstart && !(settings.parentalLock && gameSetting.lock))
			{
				    // Quickstart used to load game, so save settings before launching
                                    struct discHdr *header = &self.gameList[self.gameSelected];
                                    char titleID[7];
                                    sprintf(titleID, "%s", header->id);
                                    strcpy(settings.lastplayed,titleID);//save this game as last game played
                                    SETTINGS_Save();
                                    if(getGameSettings(titleID, &gameSetting))
                                            apply_settings();
                                    setGameSettings(titleID, &gameSetting,1);
                                    WiiLight(0); // turn off the slot light

					if(!LaunchGame())
					{
						SETTINGS_Load(); //failed to launch so get the globals back
						return 0;
					}
                                
			}
			else if(!(settings.parentalLock && gameSetting.lock))
			{
				 // Draw the Load Game Dialog panel
				DrawLoadGameDialog(false, Button_Hover(&loadButton, pointer.p_x, pointer.p_y));
			}
                        else
                        {
                            self.animate_flip = 0;
                            self.selected = false;
                        }
		}
		else
		{
		
			if(!settings.hideScroll)
			{
				//TODO IF hiding scroll; don't let user click and drag it.
				
				// Draw the slider and corner buttons
				DrawSlider(settings.theme);
			}
			
            if(!settings.parentalLock)
                Button_Theme_Paint(&addButton, settings.theme);
			if(!settings.parentalLock)
				Button_Theme_Paint(&settingsButton, settings.theme);

			// Draw Game Title
			if(settings.coverText && (!dragging && !twisting && select_ready))
			{	
				float t = 1.0; // add a configurable text size later
				draw_game_title(self.gameSelected, t);
			}
			
		}
		
		// Check for button-pointer intersections, and rumble
		if ((!self.selected && // main screen button only
			 (((!settings.parentalLock) && Button_Hover(&addButton, pointer.p_x, pointer.p_y)) ||
			  Button_Hover(&settingsButton, pointer.p_x, pointer.p_y) ||
			  Button_Hover(&slideButton, pointer.p_x, pointer.p_y))) 
			||
			(self.selected && // game load panel is showing
			 (((!settings.parentalLock) && (Button_Hover(&deleteButton, pointer.p_x, pointer.p_y) 
			                              || Button_Hover(&gsettingsButton, pointer.p_x, pointer.p_y))) ||
			  (Button_Hover(&loadButton, pointer.p_x, pointer.p_y) ||
			   Button_Hover(&backButton, pointer.p_x, pointer.p_y))) ))
		{
			// Should we be rumbling?
			if (--self.rumbleAmt > 0)
			{
				if(settings.rumble)
					WPAD_Rumble(0,1); // Turn on Wiimote rumble
			}
			else 
				WPAD_Rumble(0,0); // Kill the rumble
		}
		else
		{ // If no button is being hovered, kill the rumble
			WPAD_Rumble(0,0);
			self.rumbleAmt = 5;
		}
		
		// Turn on Slot Glow if game launch window is showing
		if (self.selected)
		{
			if (!self.slot_glow)
			{
				WiiLight(1); // turn on the slot light
				self.slot_glow = 1;
			}
		}
		else
		{
			if (self.slot_glow)
			{
				WiiLight(0); // turn off the slot light
				self.slot_glow = 0;
			}
		}
		
		//if((WPAD_ButtonsDown(0) & WPAD_BUTTON_1))
		//{
			//settings.covers3d = !(settings.covers3d);
			//ResetBuffer();
		//}
		
		if((WPAD_ButtonsHeld(0) & WPAD_BUTTON_1))  // Button2 is the 'Get Info' button
		{
			char tIOS[20];
			char tAppInfo[30];
			sprintf(tIOS, "IOS%d rev%d", IOS_GetVersion(), IOS_GetRevision());
			sprintf(tAppInfo, "CoverFloader r%d %s", SVN_VERSION, RELEASE);
			if (settings.theme) // black text on white matte
			{
				CFreeTypeGX_DrawText(ttf18pt, 320, 30,  tAppInfo, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_CENTER);
				CFreeTypeGX_DrawText(ttf16pt, 70, 30,  tIOS, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_CENTER);
			}
			else //white text on black matte
			{
				CFreeTypeGX_DrawText(ttf18pt, 320, 30,  tAppInfo, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_CENTER);
				CFreeTypeGX_DrawText(ttf16pt, 70, 30,  tIOS, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_CENTER);
			}
			GRRLIB_ScrShot(USBLOADER_PATH "/sshot.png");

		}
	
#ifdef DEBUG
// These won't work until they get switched over to use CFreeTypeGX_DrawText
//		GRRLIB_Printf(50, 20, font_texture, 0x808080FF, 1, "Pitch: %f", self.orient.pitch);
//		GRRLIB_Printf(50, 40, font_texture, 0x808080FF, 1, "adjPitch: %f", (change_scale(self.orient.pitch, -90, 90, -0.5, 0.5)) );
//		GRRLIB_Printf(50, 60, font_texture, 0x808080FF, 1, "adjRoll: %f", (change_scale(self.orient.roll, -90, 90, -0.5, 0.5)) );
//		GRRLIB_Printf(50, 20, font_texture, 0xAA0000FF, 1, "IOS%d rev%d", IOS_GetVersion(), IOS_GetRevision());
//		GRRLIB_Printf(250, 20, font_texture, 0xAA0000FF, 1, "--DEBUG Build r%d--",SVN_VERSION);
//		GRRLIB_Printf(500, 20, font_texture, 0x808080FF, 1, "FPS: %d", FPS);
//		GRRLIB_Printf(50, 20, font_texture, 0x808080FF, 1, "Shift        : %f", self.shift);
//		GRRLIB_Printf(50, 40, font_texture, 0x808080FF, 1, "gameCnt      : %d", self.gameCnt);
//		GRRLIB_Printf(50, 60, font_texture, 0x808080FF, 1, "Min/Max cover: %d / %d", self.min_cover, self.max_cover);
//		GRRLIB_Printf(50, 40, font_texture, 0x808080FF, 1, "Pointer: %d, %d", (int)pointer.p_x, (int)pointer.p_y);
//		GRRLIB_Printf(50, 60, font_texture, 0x808080FF, 1, "X: %d, %d", (int)pointer.p_x, (int)pointer.p_y);
//		GRRLIB_Printf(50,20,font_texture,0x808080FF,1,"gameCnt:%d shift:%f lastplayed:%s lastsel: %d",self.gameCnt,self.shift,settings.lastplayed,self.lastGameSelected);
/*		void * freeMemPtr;
		int freeMem=0;
		freeMemPtr = memalign(32,1024);
		if (freeMemPtr >0)
		{
			freeMem = 0x817FFFFF - (unsigned int)freeMemPtr;
			free((void *)freeMemPtr);
		}
		GRRLIB_Printf(50,20,font_texture,0x808080FF,1,"FreeMemory %u %x",freeMem,freeMemPtr);
*/

#endif
		
		// Draw the pointing hand
		DrawCursor(pointer.p_type, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		// Main loop Render()
        GRRLIB_Render();

        FPS = CalculateFrameRate();
		
		if(shutdown == 1)
			Sys_Shutdown();
		else if(reset == 1)
			Sys_Reboot(); 
		
	} // main screen gui loop END

	// Wrap it up to leave
    GRRLIB_Exit(); 
	SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	return 0;
} 



