ABOUT
=====
CoverFloader is a USB ISO Loader for Nintendo Wii. 
It currently supports loading game images from a WBFS partition using the latest cIOSX (currently understood
to be rev20).
You can copy your original Nintendo Wii Game Discs to a USB Storage device and then access your whole library
of games without needing to change discs. All within a rich, user-friendly interface.

COMPILING
=========
Since r1064 the project has been updated to make use of portlibs. In order to compile you will need to have
DevkitPPCr21 or higher and the relevant portlibs included in your DevkitPro environment. 
These should be downloaded from [http://sourceforge.net/projects/devkitpro/files/portlibs/ the portlibs download page].

See [http://devkitpro.org/wiki/portlibs the portlibs wiki] for details of how to set this up.

The required portlibs are:
  * zlib
  * freetype
  * mxml
  * libpng
  * sndlib
