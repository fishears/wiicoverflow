#ifndef _SoundFile_H_
#define _SoundFile_H_

#include <gccore.h>


typedef struct{
	unsigned char* data;
	int format;
	int size;
	int id; //Play Id
} SoundFile;

inline SoundFile SoundFile_Init(unsigned char* snd_data, int size, int format, int id);


#endif //_SoundFile_H_

