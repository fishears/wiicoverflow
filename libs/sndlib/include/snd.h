/*
Copyright (c) 2008 Francisco Muñoz 'Hermes' <www.elotrolado.net>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are 
permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice, this list of 
  conditions and the following disclaimer. 
- Redistributions in binary form must reproduce the above copyright notice, this list 
  of conditions and the following disclaimer in the documentation and/or other 
  materials provided with the distribution. 
- The names of the contributors may not be used to endorse or promote products derived 
  from this software without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#ifndef __SNDLIB_H__
#define __SNDLIB_H__


#ifdef __cplusplus
extern "C" {
#endif


#ifndef NULL
#define NULL  0
#endif

#ifdef WIN32
#define _SDL_LENDIAN_ 1
#elif __WIN32__
#define _SDL_LENDIAN_ 1
#elif LINUX
#define _SDL_LENDIAN_ 1
#endif

#ifdef _SDL_LENDIAN_
#include <SDL/SDL.h>

typedef signed char   s8;
typedef unsigned char u8;

typedef signed short   s16;
typedef unsigned short u16;

typedef signed int   s32;
typedef unsigned int u32;

typedef signed long long   s64;
typedef unsigned long long u64;

#else
#include <ogc/audio.h>
#endif

#define SND_BUFFERSIZE (1024*2)  //  1024 stereo samples (32 ms at 32Khz - 21.33 ms at 48Khz)

#define MAX_SND_VOICES 16     // max playable voices

// SND return values

#define SND_OK               0
#define SND_INVALID         -1
#define SND_ISNOTASONGVOICE -2
#define SND_BUSY             1

// SND_IsActiveVoice additional return values

#define SND_UNUSED   0   // you can use this voice
#define SND_WORKING  1   // this voice is in progress
#define SND_WAITING  2   // this voice is in progress and waiting to one SND_AddVoice function (the voice handler is called continuously)

// SND_Init param
#ifdef _SDL_LENDIAN_
#define INIT_RATE_44100    0
#else
#define INIT_RATE_32000    0
#endif
#define INIT_RATE_48000    1

// SND_SetVoice format
#define VOICE_MONO_8BIT    0
#define VOICE_MONO_16BIT   1
#define VOICE_STEREO_8BIT  2
#define VOICE_STEREO_16BIT 3


// Voice volume Range

#define MIN_VOLUME 0
#define MID_VOLUME 127
#define MAX_VOLUME 255

// Pitch Range

#define MIN_PITCH      1      // 1 Hz

#define F44100HZ_PITCH 44100  // 44100Hz

#define MAX_PITCH      144000 // 144000Hz (more process time for pitch>48000)


// note codification

enum
{
NOTE_DO=0,
NOTE_DOs,
NOTE_REb=NOTE_DOs,
NOTE_RE,
NOTE_REs,
NOTE_MIb=NOTE_REs,
NOTE_MI,
NOTE_FA,
NOTE_FAs,
NOTE_SOLb=NOTE_FAs,
NOTE_SOL,
NOTE_SOLs,
NOTE_LAb=NOTE_SOLs,
NOTE_LA,
NOTE_LAs,
NOTE_SIb=NOTE_LAs,
NOTE_SI
};

enum
{
NOTE_C=0,
NOTE_Cs,
NOTE_Db=NOTE_Cs,
NOTE_D,
NOTE_Ds,
NOTE_Eb=NOTE_Ds,
NOTE_E,
NOTE_F,
NOTE_Fs,
NOTE_Gb=NOTE_Fs,
NOTE_G,
NOTE_Gs,
NOTE_Ab=NOTE_Gs,
NOTE_A,
NOTE_As,
NOTE_Bb=NOTE_As,
NOTE_B
};

#define NOTE(note,octave) (note+(octave<<3)+(octave<<2)) // final note codification. Used in Note2Freq()



/* int Note2Freq(int note, int freq_base,int note_base);

Initializes the SND lib and fix the hardware sample rate.

-- Params ---

note: Note codified to play. For example: NOTE(C,4) for note C and octave 4

freq_base: Frequency base of the sample. For example 8000Hz

note_base: Note codified of the sample. For example: NOTE(L,3) for note L and octave 3 (LA 3) 

return: frequency (in Hz)

*/

int Note2Freq(int note, int freq_base,int note_base);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/
/* General Sound Functions                                                                                                                              */
/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* void SND_Init(s32 samplerate);

Initializes the SND lib and fix the hardware sample rate.

-- Params ---

samplerate: use INIT_RATE_32000 or INIT_RATE_48000

return: nothing

*/

void SND_Init(s32 samplerate); 

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/


/* void SND_End();

De-initializes the SND lib.

return: nothing

*/

void SND_End();

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* void SND_Pause(s32 paused);

Used to pause (or not) the sound. When you call to the SND_Init() function, the sound is paused.

NOTE: When you call with SND_Pause(0) and the sound is previously paused, it call to the internal mixer handler directly to update the buffer sound.

-- Params ---

paused: use 0 to run or 1 to pause

return: nothing

*/

void SND_Pause(s32 paused);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* s32 SND_Is_Paused();

Return if the sound is paused or not

-- Params ---

return: 0-> running 1-> paused

*/


s32 SND_Is_Paused();

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* u32 SND_GetTime();

Get the global time (in milliseconds). This time is updated from the IRQ

-- Params ---


return: current time (in ms)

*/

u32 SND_GetTime();

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* u32 SND_GetSampleCounter();

Get the global sample counter. This counter is updated from the IRQ in steps of SND_GetSampleCounter()

NOTE: you can use this to implement one timer with high precision

-- Params ---


return: current sample

*/


u32 SND_GetSampleCounter();

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* u32 SND_GetSamplesPerTick();

Get the samples sended from the IRQ in one tick

-- Params ---


return: samples per tick

*/


u32 SND_GetSamplesPerTick();

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* void SND_SetTime(u32 time);

Set the global time (in milliseconds). This time is updated from the IRQ

-- Params ---

time: fix the current time (in ms)

return: nothing

*/

void SND_SetTime(u32 time);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* void SND_SetCallback(void (*callback)());

Set a global callback for general pourpose. This callback is called from the IRQ

-- Params ---

callback: function callback

return: nothing

*/

void SND_SetCallback(void (*callback)());

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* int SND_GetAudioRate();

return: Audio rate (can be 32000, 48000 or 44100)

*/

int SND_GetAudioRate();

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Voice Functions                                                                                                                                      */
/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* s32 SND_SetVoice(s32 voice, s32 format, s32 pitch,s32 delay, void *snd, s32 size_snd, s32 volume_l, s32 volume_r, void (*callback) (s32 voice));

Set a PCM voice to play. This function stops one previously voice. Use the SND_StatusVoice() to test the status condition

NOTE: The voices are played in stereo and 16 bits independently of the source format.

-- Params ---

voice: use one from 0 to (MAX_SND_VOICES-1)

format: PCM format from VOICE_MONO_8BIT to VOICE_STEREO_16BIT

pitch: pitch frequency (in Hz) 

delay: delay time in milliseconds (ms). Time to wait before to play the voice

snd: buffer containing the samples

size_snd: size (in bytes) of the buffer samples

volume_l: volume to the left channel from 0 to 255

volume_r: volume to the right channel from 0 to 255

callback: can be NULL or one callback function is used to implement a double buffer use. When the second buffer is empty, the callback is called sending 
          the voice number as parameter. You can use "void callback(s32 voice)" function to call SND_AddVoice() and add one voice to the second buffer.
		  NOTE: When callback is fixed the voice never stops and it turn in SND_WAITING status if success one timeout condition.

return: SND_OK or SND_INVALID

*/

s32 SND_SetVoice(s32 voice, s32 format, s32 pitch,s32 delay, void *snd, s32 size_snd, s32 volume_l, s32 volume_r, void (*callback) (s32 voice));

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* s32 SND_AddVoice(s32 voice, void *snd, s32 size_snd);

Add a PCM voice in the second buffer to play. This function requires one previously call to SND_SetVoice() and one condition status different 
        of 'SND_UNUSED' 

-- Params ---

voice: use one from 0 to (MAX_SND_VOICES-1)

snd: buffer containing the samples in the same format of the previously SND_SetVoice() use

size_snd: size (in bytes) of the buffer samples

return: SND_OK, SND_INVALID or SND_BUSY if the second buffer is not empty and de voice cannot be add

*/

s32 SND_AddVoice(s32 voice, void *snd, s32 size_snd);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* s32 SND_StopVoice(s32 voice);

Stops the voice selected. 

If the voice is used in song mode, you need to assing the samples with SND_SetSongSampleVoice() again. Use SND_PauseSongVoice() in this case to stops
        the voice without lose the samples.

-- Params ---

voice: use one from 0 to (MAX_SND_VOICES-1)

return: SND_OK, SND_INVALID

*/

s32 SND_StopVoice(s32 voice);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* s32 SND_StatusVoice(s32 voice);

Return the status of the voice selected

-- Params ---

voice: use one from 0 to (MAX_SND_VOICES-1)

return: SND_INVALID 
        SND_UNUSED   you can use this voice
        SND_WORKING  this voice is in progress
        SND_WAITING  this voice is in progress and waiting to one SND_AddVoice function (the voice handler is called continuously)

*/

s32 SND_StatusVoice(s32 voice);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* s32 SND_GetFirstUnusedVoice();

Get the first unused voice. The voice 0 is tried especially and it is the last possible result. The idea is to reserve that voice for a Mod/Ogg/MP3 
       Player or similar. So if the function return a value <1 you can be sure the rest of the voices are working.

-- Params ---

voice: use one from 0 to (MAX_SND_VOICES-1)

return: SND_INVALID or the first free voice (from 0 to (MAX_SND_VOICES-1))

*/

s32 SND_GetFirstUnusedVoice();

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* s32 SND_ChangePitchVoice(s32 voice, s32 pitch);

Use this function to change the voice pitch in real-time. You can use this to create audio effects.

-- Params ---

voice: use one from 0 to (MAX_SND_VOICES-1)

return: SND_OK, SND_INVALID

*/

s32 SND_ChangePitchVoice(s32 voice, s32 pitch);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* s32 SND_ChangeVolumeVoice(s32 voice, s32 volume_l, s32 volume_r);

Use this function to change the voice volume in real-time. You can use this to create audio effects.

-- Params ---

voice: use one from 0 to (MAX_SND_VOICES-1)

volume_l: volume to the left channel from 0 to 255

volume_r: volume to the right channel from 0 to 255

return: SND_OK, SND_INVALID

*/

s32 SND_ChangeVolumeVoice(s32 voice, s32 volume_l, s32 volume_r);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* u32 SND_GetTickCounterVoice(s32 voice);

Get the tick counter from the voice starts to play (without the delay time). This counter uses the same resolution of the internal sound buffer.
       For example if the lib is initilized with INIT_RATE_48000 a return value of 24000 are equal to 0.5 seconds played

USES: you can use this value to synchronize audio & video
	   

-- Params ---

voice: use one from 0 to (MAX_SND_VOICES-1)

return: Number of ticks. No error condition for this function

*/

u32 SND_GetTickCounterVoice(s32 voice);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* u32 SND_GetTimerVoice(s32 voice);

Get the time (in milliseconds) from the voice starts to play (without the delay time). 

USES: you can use this value to synchronize audio & video 

-- Params ---

voice: use one from 0 to (MAX_SND_VOICES-1)

return: time (in ms). No error condition for this function

*/

u32 SND_GetTimerVoice(s32 voice);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* s32 SND_TestPointer(s32 voice, void *pointer);

Test if the pointer is in use by the voice (as buffer)

-- Params ---

voice: use one from 0 to (MAX_SND_VOICES-1)

return: SND_INVALID, 0-> Unused, 1-> Used as buffer

*/

s32 SND_TestPointer(s32 voice, void *pointer);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Voice Functions for the song mode                                                                                                                    */
/*------------------------------------------------------------------------------------------------------------------------------------------------------*/


/* s32 SND_SetSongSampleVoice(s32 voice, s32 format, void *snd_start, void *snd_start_loop, s32 size_snd);

Set a PCM voice to use in song mode. This function stops one previously voice and reserve this voice for the song mode.
Only can be used the MONO formats. The snd_start_loop is used to restore the sound when it finish, for 'infinite' play

This function prepare the samples to play with SND_PlaySongVoice()

-- Params ---

voice: use one from 0 to (MAX_SND_VOICES-1)

format: PCM format. Only VOICE_MONO_8BIT  and VOICE_MONO_16BIT can be used (STEREO don't have sense here)

snd_start: buffer containing the samples (header)

snd_start_loop: pointer to the buffer part containing the samples for the loop (loop)

size_snd: size (in bytes) of the buffer samples (from snd_start)


return: SND_OK or SND_INVALID

*/

s32 SND_SetSongSampleVoice(s32 voice, s32 format, void *snd_start, void *snd_start_loop, s32 size_snd);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* s32 SND_PlaySongVoice(s32 voice, s32 pitch, s32 delay, s32 attack, s32 sustain, s32 decay, s32 volume_l, s32 volume_r);

Play one note of the Song usign the sound samples assigned by SND_SetSongSampleVoice()

NOTE: The voices are played in stereo and 16 bits independently of the source format.

-- Params ---

voice: use one from 0 to (MAX_SND_VOICES-1)

pitch: pitch frequency (in Hz). Use Note2Frec() for the translation

delay: delay time in milliseconds (ms). Time to wait before to play the voice

attack: attack time in milliseconds (ms). The volume is increased from 0 to the volume assigned.

sustain: sustain time in milliseconds (ms). The volume is sustained

decay: decay time in milliseconds (ms). The volume is decreased from 0 to the volume assigned

volume_l: volume to the left channel from 0 to 255

volume_r: volume to the right channel from 0 to 255


return: SND_OK, SND_INVALID, SND_ISNOTASONGVOICE (only a voice assigned with SND_PlaySongVoice() can be used)

*/

s32 SND_PlaySongVoice(s32 voice, s32 pitch, s32 delay, s32 attack, s32 sustain, s32 decay, s32 volume_l, s32 volume_r);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* s32 SND_PauseSongVoice(s32 voice, s32 pause);

This function pause or continue a SND_PlaySongVoice() running. If you want stops the song voice, use this (SND_StopVoice() reset the voice and you need
        call to SND_SetSongSampleVoice() again)

-- Params ---

voice: use one from 0 to (MAX_SND_VOICES-1)

pause: 


return: SND_OK, SND_INVALID, SND_ISNOTASONGVOICE (only a voice assigned with SND_PlaySongVoice() can be used)

*/

s32 SND_PauseSongVoice(s32 voice, s32 pause);

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
  }
#endif

#endif
