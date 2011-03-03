///////////////////////////////////////////////////////////////////////////////
// Define errors codes                                                       //
// Created by Hell Hibou (2008)                                              //
// Part of Wii Homebrew Launcher                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef _INCLUDE_UOBIHLLEH_T_ERROR
#define _INCLUDE_UOBIHLLEH_T_ERROR

	#include <errno.h>

	#define NO_ERROR			0

	#define ERROR_UNKNOW			-(__ELASTERROR + 10)
	#define ERROR_NULL_VALUE		-(__ELASTERROR + 11)
	#define ERROR_BAD_PARAM			-(__ELASTERROR + 12)
	#define ERROR_BAD_DATA			-(__ELASTERROR + 13)
	#define ERROR_OUT_OF_MEMORY		-(__ELASTERROR + 14)
	#define ERROR_SYSTEM			-(__ELASTERROR + 15)
	#define ERROR_NO_NETWORK		-(__ELASTERROR + 16)
	#define ERROR_BAD_PROTOCOL		-(__ELASTERROR + 17)
	#define ERROR_NOT_INITIALIZED	-(__ELASTERROR + 18)
#endif

///////////////////////////////////////////////////////////////////////////////
