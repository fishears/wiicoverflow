///////////////////////////////////////////////////////////////////////////////
// Class C_App                                                               //
// Created by Hell Hibou (2008)                                              //
// Part of Wii Homebrew Launcher                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <string.h>


#include "asm.h"
#include "processor.h"
#include "app.h"

#include "Kmemory.h"

#define MAGIC_argv				0x5f617267
#define TEXT_HEADER_BLOC_BEGIN	0x48

//////////////////////////////////////////////////////////////////////////////

extern void BeforeExit  ();

typedef struct T_DolHeader 
{
	u32 text_pos	[7];
	u32 data_pos	[11];
	u32 text_start	[7];
	u32 data_start	[11];
	u32 text_size	[7];
	u32 data_size	[11];
	u32 bss_start;
	u32 bss_size;
	u32 entry_point;
} T_DolHeader;


typedef struct T_libogc_argv_data // LibOGC internal argv structure
{
	int argvMagic;		// argv magic number, set to 0x5f617267 ('_arg') if valid 
	char *commandLine;	// base address of command line, set of null terminated strings
	int length;			// total length of command line
	int argc;
	char **argv;
	char **endARGV;
} T_libogc_argv_data;	


typedef struct T_libogc_argv
{
	u8		Code;		// = 0x48
	u8		Size1;		// Hi  bits of size of text area
	u16		Size2;		// Low bits of size of text area
	u32		argvMagic;	// argv magic number, set to 0x5f617267 ('_arg') if valid 
	T_libogc_argv_data Data;
} T_libogc_argv;

///////////////////////////////////////////////////////////////////////////////

#ifndef APP_MODE_LITE

int C_App::SetLibogcArg (const void * Data, const char * CmdLine)
{
	char * MemData;
	T_libogc_argv * argv = (T_libogc_argv *) Data;
	char * CharPtr;
	u32 Size;
	char * * Argv;
	
	if (Data == NULL)		{ return 0; }
	if (CmdLine == NULL)	{ return 0; }
	if (argv->argvMagic != MAGIC_argv)			{ return 0;}
	if (argv->Code != TEXT_HEADER_BLOC_BEGIN)	{ return 0;}
	if ( (u32)((argv->Size1 * 0x10000) + argv->Size2) < sizeof(T_libogc_argv) ) { return 0; }

	Size = strlen (CmdLine) + 3;
	CharPtr = (char *) CmdLine;
	
	while (CharPtr[0] != 0x00)
	{
		if (CharPtr[0] == '\t') { Size += sizeof (char*); }
		CharPtr++;
	}
	
	MemData = (char*) MEM2_malloc(Size);
	if (MemData == NULL)	{ return 0; }
	strcpy (MemData, CmdLine);
	MemData[strlen (CmdLine)+1] = 0x00;
	MemData[strlen (CmdLine)+2] = 0x00;
	CharPtr = MemData;
	argv->Data.endARGV = Argv = (char**) &MemData[strlen (CmdLine) + 3];
	argv->Data.argvMagic	= MAGIC_argv;
	argv->Data.commandLine	= MemData;
	argv->Data.length = strlen(CmdLine)+1;
	argv->Data.argc	= 1;
	argv->Data.argv = Argv;
	
	while (*CharPtr != 0x00)
	{
		if (*CharPtr == '\t') 
		{ 
			*CharPtr = 0;
			*Argv = &CharPtr[-1];
			Argv++;
			argv->Data.argc	++;
			argv->Data.endARGV ++;
		}
		CharPtr++;
	}

	return 1;
}

#endif

///////////////////////////////////////////////////////////////////////////////

#ifndef APP_MODE_LITE

int C_App::Launch (const void * Data, const char * CmdLine)
{
	void (*EntryPoint)();
	
	u32 Level = 0;
	
	EntryPoint = (void (*)()) this->Load(Data, CmdLine, APP_ACTION_PRELOAD);
	if (EntryPoint == NULL) { return 0; }

	EntryPoint = (void(*)()) this->Load (Data, NULL, APP_ACTION_RUN);

	//BeforeExit();
	SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	_CPU_ISR_Disable (Level);
	EntryPoint();
	
	return 1;
}

#endif


///////////////////////////////////////////////////////////////////////////////

