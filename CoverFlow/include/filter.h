#ifndef _Filter_H_
#define _Filter_H_

#include "coverflow.h"

#define FILTER_FAVORITES -1
#define FILTER_OFF 0

typedef struct {
	s_gameSettings allGames[500];
	int gameCount;
	
	int currentFilter;
	
	int filteredGames[500];
	int filteredGameCount;
} s_Filter;

void InitFilter(s_Filter* filter);

void FilterList(s_Filter* filter, int filterId);

void SaveFilter(s_Filter* filter);


#endif //_Filter_H_
