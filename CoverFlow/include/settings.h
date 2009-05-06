#pragma once

extern float SETTING_coverSpacing;
extern float SETTING_coverZoom; 
extern float SETTING_coverAngle;
extern int SETTING_drawWindow;
extern int SETTING_coverText;
extern int SETTING_theme; // 0=black, 1=white
extern int SETTING_fontColor; // 0x000000FF=black, 0xFFFFFFFF=white

#ifndef _SETTINGS_
#define _SETTINGS_
float SETTING_coverZoom;
float SETTING_coverAngle;
float SETTING_coverSpacing;
int SETTING_drawWindow;
int SETTING_coverText;
int SETTING_theme;
int SETTING_fontColor; // stores the hex val of the theme
#endif /* _SYMBOL */

inline void SETTINGS_Init(void);

inline int SETTINGS_Load();

inline int SETTINGS_Save();
