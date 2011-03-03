#include "soundfile.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


inline SoundFile SoundFile_Init(unsigned char* snd_data, int size, int format, int id)
{
	SoundFile snd;
	snd.data = snd_data;
	snd.size = size;
	snd.format = format;
	
	snd.id = id;
	
	return snd;
}
