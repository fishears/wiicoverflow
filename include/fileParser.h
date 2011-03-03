
#ifndef _FILEPARSER_H_
#define _FILEPARSER_H_

#include <gctypes.h>


bool cfg_parsefile(char *fname, void (*set_func)(char*, char*));
void cfg_parseline(char *line, void (*set_func)(char*, char*));
char* trimcopy(char *dest, char *src, int size);

































#endif
