#ifndef _EE_H_
#define _EE_H_

#include "coverflow.h"
#include "utils.h"
//#include "soundmanager.h"

#define AUTHORS_LEN 6

extern const u8 ee_sco_png[];
extern const u8 ee_justin_png[];
extern const u8 ee_fishears_png[];
extern const u8 ee_alex_png[];
extern const u8 ee_bob_png[];
extern const u8 ee_bbird_png[];

typedef struct{
	int sx;
	int sy;
	float angle;
	bool visible;
} s_vars;

void ee();
void eScreen();
void showCredits();

#endif
