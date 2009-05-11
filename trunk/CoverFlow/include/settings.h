#pragma once

typedef struct {
    float coverZoom;
    float coverAngle;
    float coverSpacing;
    int drawWindow;
    int coverText;
    int theme;
    int fontColor; // stores the hex val of the theme

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
} s_settings;


inline void SETTINGS_Init(void);

inline int SETTINGS_Load();

inline int SETTINGS_Save();
