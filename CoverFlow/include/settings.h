#pragma once

extern float SETTING_coverSpacing;
extern float SETTING_coverZoom; 
extern float SETTING_coverAngle;
extern int SETTING_drawWindow;
extern int SETTING_coverText;

#ifndef _SETTINGS_
#define _SETTINGS_
float SETTING_coverZoom;
float SETTING_coverAngle;
float SETTING_coverSpacing;
int SETTING_drawWindow;
int SETTING_coverText;
#endif /* _SYMBOL */

inline void SETTINGS_Init(void);

inline int SETTINGS_Load();

inline int SETTINGS_Save();
