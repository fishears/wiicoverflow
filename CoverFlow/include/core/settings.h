#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#ifndef BOOTLOADER
#include "coverflow.h"
#else
#include "bootloader.h"
#endif

typedef struct {
    float coverZoom;
	float coverCamY;
	float coverCamX;
    float coverAngle;
    float coverSpacing;
    float coverFallOff;
    int drawWindow;
    int coverText;
    int theme;
    int fontColor; // stores the hex val of the theme
	
	int covers3d;
	int hideScroll;
	

	int volume;

    int sound;
    int music;
    int parentalLock;
    int rumble;
    int quickstart;
    int enablepitch; //0=OFF, 1=ON skip the load disc dialog

    int ocarina;
    int hooktype;
    int language;
    int video;
    int vipatch;
    char lastplayed[7];
    char localLanguage[7];
} s_settings;


void SETTINGS_Init(void);
void graphics_SETTINGS_Init(void);

int SETTINGS_Load();

int SETTINGS_Save();

#endif
