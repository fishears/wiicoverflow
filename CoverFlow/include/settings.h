#pragma once

extern float SETTING_coverSpacing;
extern float SETTING_coverZoom; 
extern float SETTING_coverAngle;
extern int SETTING_drawWindow;

#ifndef _SETTINGS
#define _SETTINGS
float SETTING_coverZoom;
float SETTING_coverAngle;
float SETTING_coverSpacing;
int SETTING_drawWindow;
#endif /* _SYMBOL */

inline void SETTINGS_Init();

//inline bool SETTINGS_Load(char* settingsFileName);

//inline bool SETTINGS_Save(char* settingsFileName);
