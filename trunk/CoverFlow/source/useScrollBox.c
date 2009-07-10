/*
 *  useScrollbox.c
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by:  scognito, F1SHE4RS, afour98, blackbird399, LoudBob11
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 */
#include "useScrollBox.h"

char *textAbout[]= {
"", 
"Gitkua      [images]",
"brwest85    [HBC Icon]",
"alexcarlosantao",
"",
"[Translators]",

"basda, Spidy1000, nolodude, Xtend,",
"pepperoni, aHMET, onyxdata, Tusk",
"",
"[Channel and Forwarder]",
"Davi92 and DjSim1",

"",
"[Cover Service]",
"zorglub07 and wiitdb.com crew",
"djtaz and wiiboxart.com crew",
"",

"the power of GRRLib",
"",
"and others...", 
"(please ask if we forgot someone!)",
"",

"",
"http://code.google.com/p/wiicoverflow/"
};

// ttf_size ( // 14, 16, 18, 20, 24 ( 0 -> Default = 16 )
int fmtAboUt[] = { 	 0, 0, 0, 0,24,
					18, 0, 0,24,18,
					 0,24,18, 0, 0,
					24,18,24,18,14,
					14,14, 0 };
 
void Init_ScrollBoxes()
{
 Init_sbAbout();
}

void Config_ScrollBoxes()
{
 Config_sbAbout();
}


void Init_sbAbout()
{
 ScrollBox_Init( &sbAbout, 100, 290, 440, 93, 1, 0xffffffdd, 0x737373FF );
}


void Config_sbAbout()
{
 ScrollBox_SetText(&sbAbout, textAbout, 23);
 ScrollBox_SetTextFormat( &sbAbout, fmtAboUt, 23);
 ScrollBox_SetTextStyling( &sbAbout, FTGX_JUSTIFY_CENTER);
 ScrollBox_SetStartPos(&sbAbout, SB_TOP );
 ScrollBox_SetDelay(&sbAbout, 7 );
 ScrollBox_SetRepeat(&sbAbout, true);
}

