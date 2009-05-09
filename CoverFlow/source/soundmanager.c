#include "soundmanager.h"
extern s_settings settings;

extern u8 bgMusic_ogg[];
extern u32 bgMusic_ogg_size;

extern u8 coverFlip_raw[];
extern u32 coverFlip_raw_size;

extern u8 coverScroll_raw[];
extern u32 coverScroll_raw_size;

extern u8 buttonClick_raw[];
extern u32 buttonClick_raw_size;


inline void SOUND_Init()
{
   SND_Init(INIT_RATE_48000); // Initialize the Sound Lib

   SND_Pause(0); // the sound loop is running now

   _sounds[FX_BG_MUSIC]     = SoundFile_Init(bgMusic_ogg, bgMusic_ogg_size, OGG_FORMAT, FX_BG_MUSIC);
   _sounds[FX_COVER_FLIP]   = SoundFile_Init(coverFlip_raw, coverFlip_raw_size, RAW_FORMAT, FX_COVER_FLIP);
   _sounds[FX_COVER_SCROLL] = SoundFile_Init(coverScroll_raw, coverScroll_raw_size, RAW_FORMAT, FX_COVER_SCROLL);
   _sounds[FX_BUTTON_CLICK] = SoundFile_Init(buttonClick_raw, buttonClick_raw_size, RAW_FORMAT, FX_BUTTON_CLICK);
}

inline int SOUND_PlaySound(int sound_id, int loops)
{
	if(sound_id < MAX_SOUNDS)
	{
			switch(_sounds[sound_id].format)
			{
					case OGG_FORMAT:
							if(settings.music)
							{
								if(loops == 0)
								{
									  StopOgg();
									  PlayOgg(mem_open((char*)_sounds[sound_id].data, _sounds[sound_id].size), 0, OGG_ONE_TIME);
									  return 1;
								}
								else
								{
									  StopOgg();
									  PlayOgg(mem_open((char*)_sounds[sound_id].data, _sounds[sound_id].size), 0, OGG_INFINITE_TIME);
									  return 1;
								}
							}
							
							return 1;
							break;
					case RAW_FORMAT:
							if(settings.sound)
							{
								SND_SetVoice(SND_GetFirstUnusedVoice(), VOICE_MONO_8BIT, 8000, 0, (char*)_sounds[sound_id].data, _sounds[sound_id].size, 30, 30, NULL);
							}
							return 1;
							break;
					default:
							return 0;
							break;
			}
	}
	
	return 0;
}

inline void SOUND_StopSound(int sound_id)
{
	if(sound_id < MAX_SOUNDS)
	{
			switch(_sounds[sound_id].format)
			{
					case OGG_FORMAT:
							StopOgg();
							return;
							break;
					case RAW_FORMAT:
							//? Not sure how to stop raw atm (usually these are just quick effects)
							return;
							break;
					default:
							return;
							break;
			}
	}
}
