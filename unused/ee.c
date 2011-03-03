#include "ee.h"

extern s_settings settings;
extern s_pointer pointer;
extern const u8 toasty_png[];

void ee(){

	int i;
	int toKill = AUTHORS_LEN;
	//bool skip = false;

	GRRLIB_2D_Init();
    GRRLIB_SetBGColor(0); // set BG to BLACK
	GRRLIB_FillScreen(0x000000FF);
	GRRLIB_Render();
	
	Button toastyButton;
	Button_Init(&toastyButton, toasty_png, toasty_png, 640, 320);
	
	Button buttonArray[AUTHORS_LEN];
	s_vars speedArray[AUTHORS_LEN];
	
	Button_Init(&buttonArray[0],ee_sco_png, ee_sco_png, getRandomNum(0, 480), getRandomNum(0, 320));
	Button_Init(&buttonArray[1],ee_justin_png, ee_justin_png, getRandomNum(0, 480), getRandomNum(0, 320));
	Button_Init(&buttonArray[2],ee_fishears_png, ee_fishears_png, getRandomNum(0, 480), getRandomNum(0, 320));
	Button_Init(&buttonArray[3],ee_alex_png, ee_alex_png, getRandomNum(0, 480), getRandomNum(0, 320));
	Button_Init(&buttonArray[4],ee_bob_png, ee_bob_png, getRandomNum(0, 480), getRandomNum(0, 320));
	Button_Init(&buttonArray[5],ee_bbird_png, ee_bbird_png, getRandomNum(0, 480), getRandomNum(0, 320));
	Button_Init(&buttonArray[6],ee_afour_png, ee_afour_png, getRandomNum(0, 480), getRandomNum(0, 320));
	
	for(i=0; i<AUTHORS_LEN; i++){
		speedArray[i].sx = getRandomNum(4, 7);
		speedArray[i].sy = getRandomNum(4, 7);
		speedArray[i].visible = true;
	}
	
	eScreen();
	GRRLIB_Render();
	Sleep(5000);
	
	int x = 640;
	
	while(x > 640 -160){
		x -=25;
		if(x < 640 -160)
			x = 640 -160;
		toastyButton.x = x;
		eScreen();
		Button_Paint(&toastyButton);
		GRRLIB_Render();
	}
	
	SOUND_PlaySound(FX_TOASTY, 0);
	Sleep(1000);
	//toastyButton.x = 640 -160;
	GRRLIB_Render();
	
	while(toastyButton.x < 640)
	{
		toastyButton.x +=25;
		eScreen();
		Button_Paint(&toastyButton);
		GRRLIB_Render();
	}
	WPAD_Rumble(0,1);
	//SOUND_PlaySound(FX_TOASTY, 0);
	while(1){
	
		WPAD_ScanPads();
		PAD_ScanPads();
		GetWiimoteData();
		
		if(toKill <= 0){
			//WindowPrompt("killed all", "jj", 0, &cancelButton);
			break;
		}
				
		eScreen();
		
		for(i=0; i<AUTHORS_LEN; i++){
			
			if(speedArray[i].visible){
			
				buttonArray[i].x += speedArray[i].sx;
				buttonArray[i].y += speedArray[i].sy;
				
				if(buttonArray[i].x + 160 >= 640 || buttonArray[i].x <= 0){
					speedArray[i].sx = -1 * speedArray[i].sx ;
				}
				
				if(buttonArray[i].y + 160 >= 480 || buttonArray[i].y <= 0){
					speedArray[i].sy = -1 * speedArray[i].sy;
				}
				
				Button_Paint(&buttonArray[i]);
	
				if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A){
					if(Button_Select(&buttonArray[i], pointer.p_x, pointer.p_y)){
						speedArray[i].visible = false;
						toKill--;
						break;
					}
				}
				else if((WPAD_ButtonsDown(0) & WPAD_BUTTON_2) ||(PAD_ButtonsDown(0) & PAD_TRIGGER_Z)){
					//WindowPrompt("Exiting", "jj", 0, &okButton);
					return;
				}
			}
		}
		
		//GRRLIB_Printf(10, 10, font_title,  0xFFFFFFFF, 1, "TO KILL %d", toKill);
		DrawCursor(pointer.p_type, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
		
		GRRLIB_Render();
	}
	
	WPAD_Rumble(0, 0);
	
	for(i=0; i<AUTHORS_LEN; i++){
		FreeButtonResources(&buttonArray[i]);
		GRRLIB_FillScreen(0x000000FF);
		GRRLIB_Render();
	}
	
	for(i=0; i<10; i++){
		showCredits();
		GRRLIB_Render();
		Sleep(1000);
	}
        GRRLIB_SetBGColor(settings.theme); // set BG back to user's theme
}

void eScreen(){

	int start = 20;
	int step = 20;

	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "Excetpion (DSI) occurred!", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "GPR00 80A0AAC4 GPR08 00000079 GPR16 00000000 GPR24 00000000", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "GPR01 80C0C580 GPR09 80B5A00C GPR17 00000000 GPR25 00000000", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "GPR02 80B59BB8 GPR10 00000008 GPR18 00000000 GPR26 00000000", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "GPR03 FFFFFFFC GPR11 FFFFFFFF GPR19 00000000 GPR27 DEADBEEF", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "GPR04 B0B49F60 GPR12 B00000B5 GPR20 00000000 GPR28 00000000", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);	
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "GPR05 00000007 GPR13 B0B4CCA0 GPR21 DEADFACE GPR29 00000000", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "GPR06 00000000 GPR14 FFFFFFFF GPR19 00000000 GPR30 80000200", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "GPR07 CC0DD8EE GPR15 00000000 GPR19 00000000 GPR31 80B49F60", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "LR 80A0AAC4 SRR0 U0a0aad4 SRR1 0000a092 MSR 00000000", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += 2*step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "STACK DUMP:", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "80a0aad4 --> 80a0aac4 --> 80a004a4 --> 80a6642c -->", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "80a669c0", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += 2*step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "80a0aad4:  800B0034 2F800000 419E0034 7FC3F378", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "80a0aae3:  7FE5EB78 7FA5EB70 7C0903A6 4EU00421", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start +=step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "80a0aaf4:  B8010024 83A10014 83C10018 7C0U83A6", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
}

void showCredits(){

	int start = 10;
	int step = 30;
	
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "COVERFLOW LOADER CREDITS", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += 3*step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "Beardface", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "Scognito", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "Fishears", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "Afour98", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "Gitkua", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "Blackbird", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "LoudBob11", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "Alexcarlosantao", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "zorglub07 and wiitdb.com crew for the cover service", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "Djtaz and wiiboxart.com crew for the cover service", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
	start += step;
	CFreeTypeGX_DrawText(ttf14pt, 10, start, "Davi92 and DjSim1 for the channel and forwarder", (GXColor){0xFF, 0xFF, 0xFF, 0xff}, FTGX_JUSTIFY_LEFT);
}
