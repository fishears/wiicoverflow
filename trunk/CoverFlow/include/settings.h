#pragma once

extern float SETTING_coverSpacing;
extern float SETTING_coverZoom; 
extern float SETTING_coverAngle;
extern int SETTING_drawWindow;
extern int SETTING_coverText;
extern int SETTING_theme; // 0=black, 1=white
extern int SETTING_fontColor; // 0x000000FF=black, 0xFFFFFFFF=white

extern int SETTING_sound; //0=OFF, 1=ON
extern int SETTING_music; //0=OFF, 1=ON
extern int SETTING_parentalLock; //0=OFF, 1=ON
extern int SETTING_rumble; //0=OFF, 1=ON
extern int SETTING_quickstart; //0=OFF, 1=ON skip the load disc dialog
extern int SETTING_enablepitch; //0=OFF, 1=ON pitch to zoom using B key

extern int SETTING_ocarina;
extern int SETTING_hooktype;
extern int SETTING_language;
extern int SETTING_video;
extern int SETTING_vipatch;

#ifndef _SETTINGS_
#define _SETTINGS_
float SETTING_coverZoom;
float SETTING_coverAngle;
float SETTING_coverSpacing;
int SETTING_drawWindow;
int SETTING_coverText;
int SETTING_theme;
int SETTING_fontColor; // stores the hex val of the theme

int SETTING_sound;
int SETTING_music;
int SETTING_parentalLock;
int SETTING_rumble;
int SETTING_quickstart;
int SETTING_enablepitch; //0=OFF, 1=ON skip the load disc dialog

int SETTING_ocarina;
int SETTING_hooktype;
int SETTING_language;
int SETTING_video;
int SETTING_vipatch;
#endif /* _SYMBOL */

inline void SETTINGS_Init(void);

inline int SETTINGS_Load();

inline int SETTINGS_Save();
