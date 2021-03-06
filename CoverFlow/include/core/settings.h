#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#ifndef BOOTLOADER
#include "coverflow.h"
#else
#include "bootloader.h"
#endif

typedef struct {
    float coverZoom;
	float mainCoverZoom;
	float coverCamY;
	float coverCamX;
    float coverAngle;
    float coverSpacing;
    float coverFallOff;
	int falldist;
    int drawWindow;
    int coverText;
    int theme;
	
	int auto_hide;
        int favorites; //favorites mode
	int covers3d;
        int hq; //High Quality 3D cover mode
	int covers3dThickness;
	int hideScroll;

	int volume;
	int sound;
    int music;
    int parentalLock;
    int rumble;
    int quickstart;
    int enablepitch; //0=OFF, 1=ON skip the load disc dialog

	int autoUpdate;

    int ocarina;
    int loser;
    int hooktype;
    int language;
    int video;
    int vipatch;
	int cios; 		// 0=cIOS249, 1=cIOS222
        char partition[16];
	int barstep;    // 0 -> 9
    char lastplayed[7];
    char localLanguage[7];
	int presetFix;  // preset gamefix, default 0
#ifdef NEWS_READER 
	char newsID[10];
	char newsDate[7];
#endif
} s_settings;


typedef struct {
    float coverZoom;
	float mainCoverZoom;
	float coverCamY;
	float coverCamX;
    float coverAngle;
    float coverSpacing;
    float coverFallOff;
	int falldist;
    int drawWindow;
    int coverText;
    int hideScroll;
	int covers3dThickness;
} gp_settings;	


void SETTINGS_Init(void);
void graphics_SETTINGS_Init(void);
void backup_gpSettings();
void restore_gpSettings();

int SETTINGS_Load();

int SETTINGS_Save();

int getRevXML();
int getLangXML();
void createEmptyWiiCoverFlowFile();
#endif
