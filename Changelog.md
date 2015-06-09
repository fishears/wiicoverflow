## v1.3a (2.0 Path) ##

-NEW-
  * GUI option for ProgressBar
  * Support of Guitar Hero 3 Controller (see [r1016](https://code.google.com/p/wiicoverflow/source/detail?r=1016))
  * On-the-fly patch for New Super Mario Bros. (PAL and NTSC)
  * Playing Banner Sounds on game selection ( Thanks to oggzee )
  * ~~USB Keyboard Support~~ ( _currently under maintenance_ )
  * Keeping your changes in **titles.txt** after a new download (see [r988](https://code.google.com/p/wiicoverflow/source/detail?r=988))
  * Support of MultiDOL-games (like MPT) with 'alt.DOL from image'
  * Support of 'Hermes' cIOS 222/223
  * Edit GameID with OSK  ( useful for custom isos like GH )
  * Cover download from multiple hosts
-FIX-
  * Fixed cheats
  * Fixed hangup after booting

-UPDATE-
  * Large alt.DOL files loading from SD: or USB:
  * Using latest libogc 1.8.0
  * Using latest libfat 1.0.6
  * Using latest ported libraries

## v1.3 (Stable) (September ?? 2009) ##

-NEW-
  * Cover download from multiple hosts

-FIX-
  * 

-UPDATE-
  * 

## v1.2 (Stable) (August 18 2009) ##

-UPDATE-
  * cover downloads switched to www.muntrue.nl

-FIX-
  * SDHC support fixed
  * Improved cover loading speed
  * Cheat downloads redirected to geckocodes.org

## v1.2a (2.0 Path) (August 18 2009) ##

-UPDATE-
  * cover downloads switched to www.muntrue.nl

-NEW-
  * OSK (OnScreen-Keyboard) (qwerty)
    1. used with dynamic paths
    1. used with cheat manager
  * Method for alt.DOL from Image with autoselection implemented
  * Method for alt.DOL from SD: / USB: implemented
  * CoverFloader News Service (CFNS) (requires internet connection)
  * CoverFloader NewsReader
  * 2nd page for System Information (paths)
  * dynamic paths for all CoverFloader files and directories on SD: and USB: (editable with OSK)
  * additional support for USB-Devices
  * support for the new game hooks (found in GeckoOS 1.9.2)
  * cheat codes with variables are now editable

-FIX-
  * SDHC support fixed
  * Improved cover loading speed
  * Improved cheat compatibility (codes per cheat raised to 100)
  * Cheat downloads redirected to geckocodes.org

## v1.1  (July 24 2009) ##

-NEW-
  * Load 2D Cover if a 3D cover is not found.

-FIX-
  * Fix issue that stores invalid files for missing discs and covers ([issue344](https://code.google.com/p/wiicoverflow/issues/detail?id=344))
  * Fix typo in url-path for diskarts
  * Fix bug in wbfs\_estimate\_disc() ([issue321](https://code.google.com/p/wiicoverflow/issues/detail?id=321) thanks to hend...@freenet.de)
  * Fix bug that doesn't create codes directory for cheats ([issue326](https://code.google.com/p/wiicoverflow/issues/detail?id=326) thanks to Richard)
  * Fix flipping of zoomed 3d covers. ([issue256](https://code.google.com/p/wiicoverflow/issues/detail?id=256))

-OTHER-
  * Titles.txt is now taken from wiitdb.com


---

## v1.0  (July 7 2009) ##

-NEW-
  * Preliminary gamecube games support! (use the konami code after cover are loaded)
  * Channel available!  (thanks to Davi92 and DjSim1 )
  * Forwader available! (thanks to Davi92 and DjSim1 ) put the dol in /apps/CoverFloader/boot.dol

-FIX-
  * Improved regional cover download (thanks to frederic.martini)
  * Improved progressbar


---

## RC9 - Independence Day Edition (July 4 2009) ##

-NEW-
  * Support for country specific covers (falling back to EN for PAL games, or US for NTSC if covers are not found)
-FIX-
  * Fix code dump on adding new games if cover was not found
  * Fix bug that corrupted xml files
  * Fix some minor bugs
-OTHER-
  * Changed default cover server, current one is www.wiitdb.com
  * All images (2d/3d cover and disc art) are 6 digits only


---

## RC8 (June 29 2009) ##

-NEW-
  * Cheat Manager remembers last used cheats.
  * Fade of buttons on main screen
-FIX-
  * Use improved libfat instead of devkitpro lib fat. (Cover Load Speed Fix)
  * Fix some issues with info button/screen


---

## RC7U2 (June 27 2009) ##

-NEW-
  * members of wiiboxart.com can download covers again
  * DeviceID at System Infos
-FIX-
  * Minor bug fixes


---

## RC7U1 (June 25 2009) ##

-NEW-
  * Game fixes are selectable per game, 'none' is default setting
  * "presetFix" in wiicoverflow.xml, (read about [r701](https://code.google.com/p/wiicoverflow/source/detail?r=701))
  * Online cheat blocker - if you cheat, you can't go online in the game
  * Batch cheat download in settings menu... (No longer have to do each individually)
  * Buttons auto hide on main screen when not hovering nearby. (Turn on/off auto hide in config file)
-FIX-
  * Hide info button when parental controls on
  * Minor bug fixes


---

## RC7 (June 22 2009) ##

-NEW-
  * Improved GUI behaviour
  * Center cover 'pops' towards the user
  * Easing in/out of covers and dialogs
  * Added information window
  * Cheat Manager (beta). Downloads cheats and lets you select which to use

-FIX-
  * Anti\_002\_fix from WiiPower working with cIOS rev10  (Indiana Jones, MySims Racing, Ghostbusters, Punch-up all work)
  * error 002 fix from WiiPower working with cIOS rev13a (Indy, MySimsRacing, GBusters all work)
  * Other memory issues solved
  * Minor bug fixes
  * Updated right edge 3D case graphic

-OTHER
  * Internal cleanup, removing redundant init calls
  * Faster Cover Caching


---

## RC6 (June 07 2009) ##

-NEW-
  * Huge UI Overhaul - new buttons, graphics, layout, look and feel
  * Improved install menu, use only with 2D covers
  * Improved translations
  * Gamecube PAD support for booting games / returning to menu (L/R triggers)
  * Add Parental Locking of games. (Games aren't hidden yet; but when locked, won't boot in parental mode.)

-FIX-
  * Lot of memory issues solved
  * Code cleanup and minor bugs fixed


---

## RC5 (May 30 2009) ##

-FIX-
  * Fixed True Type memory leak ([issue176](https://code.google.com/p/wiicoverflow/issues/detail?id=176))
  * Wrong Covers ([issue168](https://code.google.com/p/wiicoverflow/issues/detail?id=168))
  * Fixed swapped download for 2D/3D covers
  * Settings Menu Clean up.
  * Fix for game install errors.

---

## RC4 (May 29 2009) ##

-FIX-
  * Fixed sorting issues ([issue155](https://code.google.com/p/wiicoverflow/issues/detail?id=155))
  * Configurable 3D Covers (Turn on/off in graphics settings menu.)
> > NOTE: You have to redownload covers the first time you turn on 3D
  * Buffer stability fixes (mem issues)
  * Language updates
  * GFX Tweaks
  * Font cleanup
-NEW-
  * The last game played will be the default game when the Loader is launched

---

## RC3 Update (25 May 2009) ##

-FIX-
  * Fixed transparent reflection problem ([issue73](https://code.google.com/p/wiicoverflow/issues/detail?id=73))
  * Fixed a bug that download black image when a cover is not found ([issue134](https://code.google.com/p/wiicoverflow/issues/detail?id=134))
  * Fixed a bug that my crash wii if wireless signal is lost: now network is initialized each time a button that requires a download is clicked ([issue138](https://code.google.com/p/wiicoverflow/issues/detail?id=138))
  * Fixed a post installation dump ([issue128](https://code.google.com/p/wiicoverflow/issues/detail?id=128))
  * Fixed slow setting menu bug ([issue131](https://code.google.com/p/wiicoverflow/issues/detail?id=131))
  * Fixed Code dump on too many covers ([issue146](https://code.google.com/p/wiicoverflow/issues/detail?id=146)) (RC3 Update)
  * Fixed Ocarina/Patching causing code dump ([issue151](https://code.google.com/p/wiicoverflow/issues/detail?id=151)) (RC3 Update)
  * Other minor fixes we forgot, and general code cleanup

-NEW-
  * New Graphic Setting - Falloff
  * Wiimote battery support (enhanced (RC3 Update))
  * New buttons images
  * Support for titles.txt, downloadable from settings menu

---

## RC2 (20 May 2009) ##

-FIX-
  * Fixed crash after installing a game ([issue110](https://code.google.com/p/wiicoverflow/issues/detail?id=110))
  * Fixed a bug that may lead a crash when exiting to HBC ([issue113](https://code.google.com/p/wiicoverflow/issues/detail?id=113))
  * Fixed a bug that caused swapping of wiimotes when entering loader ([issue115](https://code.google.com/p/wiicoverflow/issues/detail?id=115))
  * Fixed some issues with bad images ([issue105](https://code.google.com/p/wiicoverflow/issues/detail?id=105), [issue109](https://code.google.com/p/wiicoverflow/issues/detail?id=109))
  * Fixed over scroll ([issue108](https://code.google.com/p/wiicoverflow/issues/detail?id=108))
  * Finished White Theme ([issue27](https://code.google.com/p/wiicoverflow/issues/detail?id=27))
  * Fixed Cover Zoom available with Parental Lock on ([Issue63](https://code.google.com/p/wiicoverflow/issues/detail?id=63))
  * Fixed Progress Bar limit - sometimes exceeded container
  * Fixed a number of bugs when user had 0, 1 or 2 games installed
  * Fixed missing pointer after usb drive retry
-NEW-
  * Settings buttons hidden if parental control is enabled
  * New slider and buttons graphics
  * Cover art is now anti-aliased for improved quality when zooming
  * Slot light glows when game is selected ([issue95](https://code.google.com/p/wiicoverflow/issues/detail?id=95))
  * Sensible limits added to Graphics Settings values
-OTHER-
  * Reorganized code (splitting main.c in several files, removed unused ones)

---

## RC1 update1 (15-may-09) ##

-FIX-
  * Crash when a gamecube controller is connected
-NEW-
  * Reorganized load window