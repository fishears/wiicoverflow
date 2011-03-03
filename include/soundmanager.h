#ifndef _SoundManager_H_
#define _SoundManager_H_

#include <gccore.h>

#include "soundfile.h"

#include "oggplayer.h"

#include "settings.h"

#include "coverflow.h"

//SoundFile _sounds[MAX_SOUNDS];

inline void SOUND_Init();

//Play a sound the requested number of times (-1 is infinite loop)
inline int SOUND_PlaySound(int sound_id, int loops); 
//Stop playing sound of requested ID
inline void SOUND_StopSound(int sound_id);


#endif //_SoundManager_H_

