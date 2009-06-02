#include "bootloader.h"

extern s_settings settings;
s_gameSettings gameSetting;

u32 swap32(u32 x)
{
	return (x >> 24) | ((x << 8) & 0x00FF0000UL) | ((x >> 8) & 0x0000FF00UL) | (x << 24);
}

void apply_settings()
{
    //called after global settings are saved and right before game is launched
    //this overwrites global settings with the per-game settings
    if(gameSetting.ocarina != -1)
    {
        settings.ocarina = gameSetting.ocarina;
        settings.hooktype = gameSetting.hooktype;
        settings.language = gameSetting.language;
        settings.video = gameSetting.video;
        settings.vipatch = gameSetting.vipatch;
    }
}
