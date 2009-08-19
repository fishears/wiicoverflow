#ifndef _UTILS_H_
#define _UTILS_H_

#include "coverflow.h"

/* Constants */
#define KB_SIZE         1024.0
#define MB_SIZE         1048576.0
#define GB_SIZE         1073741824.0

#define REQUIRED_IOS_REV 10 //this is the minimum cIOS Revision required for full loader support

/* Macros */
#define round_up(x,n)   (-(-(x) & -(n)))

/* Prototypes */
u32 swap32(u32);
void setSysdate(char* ret_string);
void getSysdate(char* now, char* ret_string);
void ios_version_check();
u8 CalculateFrameRate();
void apply_settings();
void apply_fixsettings();
//bool ifConfig(char *ip);
bool networkInit(char *ip);
void quit();
void WiiLight(int enable);
void UpdateBufferedImages();
void DragSlider(int xPos);
int getRandomNum(int min, int max);
float easeOutQuint(float t,float b , float c, float d);
float easeInQuint(float t,float b , float c, float d);
float easeOutQuad(float t,float b , float c, float d);
float easeInQuad(float t,float b , float c, float d);
float easeOutCubic(float t,float b , float c, float d);
char * ParseTokenedUrl(char * sitebase,char * subdirectory, char * url, char* username, char* password, char * lang, char * region, char *id);
int DebTxt( char * Msg);
#endif
