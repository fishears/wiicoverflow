#include "filter.h"

extern s_self self;

void InitFilter(s_Filter* filter, struct discHdr *gameList)
{
	int i= 0;
	
	for(i = 0; i < self.gameCnt; i++)
	{
		struct discHdr *header = &gameList[i];
		char titleID[7];
		sprintf(titleID, "%s", header->id);
		getGameSettings(titleID, &(filter->allGames[i]));
	}
	
	filter->gameCount = self.gameCnt;
	
	filter->currentFilter = FILTER_OFF;
	
	FilterOff(filter);
}

/*Put games back in the normal order*/
void FilterOff(s_Filter* filter)
{
	int i = 0;
	for(i = 0; i < filter->gameCount; i++)
	{
		filter->filteredGames[i] = i;
	}
	
	filter->filteredGameCount = filter->gameCount;
}

/*Put games back in the normal order*/
void FilterFavorites(s_Filter* filter)
{
	int i = 0;
	int count = 0;
	for(i = 0; i < filter->gameCount; i++)
	{
		if(filter->allGames[i].favorite)
		{
			filter->filteredGames[count] = i;
			count++;
		}
	}
	
	filter->filteredGameCount = count;
}

void FilterList(s_Filter* filter, int filterId)
{
	filter->currentFilter = filterId;
	
	switch(filterId)
	{
		case FILTER_OFF:
			FilterOff(filter);
			break;
		case FILTER_FAVORITES:
			FilterFavorites(filter);
			break;
		default:
			{
				int i = 0;
				int count = 0;
				for(i = 0; i < filter->gameCount; i++)
				{
					if(filter->allGames[i].category == filterId)
					{
						filter->filteredGames[count] = i;
						count++;
					}
				}
				
				filter->filteredGameCount = count;
				
			}
			break;
	}
}

void SaveFilter(s_Filter* filter, struct discHdr *gameList)
{
	int i = 0;
	
	for(i = 0; i < filter->gameCount; i++)
	{
		if(filter->allGames[i].changed)
		{
			struct discHdr *header = &gameList[i];
			char titleID[7];
			sprintf(titleID, "%s", header->id);
			
			setGameSettings(titleID, &(filter->allGames[i]), 2);
		}
	}
}
