#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <ogcsys.h>
#include "fileParser.h"
#include "localization.h"


bool cfg_parsefile(char *fname, void (*set_func)(char*, char*))
{
	FILE *f;
	char line[200];

	f = fopen(fname, "rt");
	if (!f) 
	{
		return false;
	}

	while (fgets(line, sizeof(line), f)) 
	{
		if (line[0] == '#') continue;
		cfg_parseline(line, set_func);
	}
	return true;
}


void cfg_parseline(char *line, void (*set_func)(char*, char*))
{
	char tmp[200], name[100], val[100];
	strcopy(tmp, line, sizeof(tmp));
	char *eq = strchr(tmp, '=');
	if (!eq) return;
	*eq = 0;
	trimcopy(name, tmp, sizeof(name));
	trimcopy(val, eq+1, sizeof(val));
	set_func(name, val);
}


char* trimcopy(char *dest, char *src, int size)
{
	int len;
	
	while (*src == ' ') src++;
	len = strlen(src);
	while (len > 0 && strchr(" \r\n", src[len-1])) len--;
	if (len >= size) len = size-1;
	strncpy(dest, src, len);
	dest[len] = 0;
	return dest;
}
