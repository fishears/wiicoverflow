
#include "konami.h"

bool done = false;;

int konami_buffer[10] = {WPAD_BUTTON_UP, WPAD_BUTTON_UP, WPAD_BUTTON_DOWN, WPAD_BUTTON_DOWN,
						  WPAD_BUTTON_LEFT, WPAD_BUTTON_RIGHT, WPAD_BUTTON_LEFT, WPAD_BUTTON_RIGHT,
						  WPAD_BUTTON_B, WPAD_BUTTON_A};
int konami_index = 0;
int konami_test[10];

void ShiftKonami()
{
	int i = 0;
	for(i = 0; i < 9; i++)
	{
		konami_test[i] = konami_test[i+1];
	}
}

void InsertKonami(int key)
{
	if(konami_index == 9)
	{
		ShiftKonami();
	}
	else
	{
		konami_index++;
	}
	
	konami_test[konami_index] = key;
}

bool CheckKonami()
{
	//sorry, only allow konami code 1 time.
	if(done)
		return false;

	bool btn_hit = true;

	if(WPAD_ButtonsDown(0) & WPAD_BUTTON_UP)
	{
		InsertKonami(WPAD_BUTTON_UP);
	}
	else if(WPAD_ButtonsDown(0) & WPAD_BUTTON_DOWN)
	{
		InsertKonami(WPAD_BUTTON_DOWN);
	}
	else if(WPAD_ButtonsDown(0) & WPAD_BUTTON_LEFT)
	{
		InsertKonami(WPAD_BUTTON_LEFT);
	}
	else if(WPAD_ButtonsDown(0) & WPAD_BUTTON_RIGHT)
	{
		InsertKonami(WPAD_BUTTON_RIGHT);
	}
	else if(WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
	{
		InsertKonami(WPAD_BUTTON_B);
	}
	else if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
	{
		InsertKonami(WPAD_BUTTON_A);
	}
	else if(WPAD_ButtonsDown(0) & WPAD_BUTTON_1)
	{
		InsertKonami(WPAD_BUTTON_1);
	}
	else if(WPAD_ButtonsDown(0) & WPAD_BUTTON_2)
	{
		InsertKonami(WPAD_BUTTON_2);
	}
	else
	{
		btn_hit = false;
	}
	
	if(btn_hit)
	{
		int t = 0;
		for(t = 0; t < 10; t++)
		{
				
			//U U D D L R L R B A
			if(konami_test[t] != konami_buffer[t])
			{
				return false;
			}
			
		}
		
		done = true;
		return true;
	}
	
	return false;
}

