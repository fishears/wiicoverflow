#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/wii_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	source source/pngu images source/libwbfs source/core source/mload 
#DATA		:=	data bootloader sounds fonts languages
DATA		:=	data sounds fonts languages
INCLUDES	:=  include include/pngu include/libwbfs include/core include/mload 

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

CFLAGS	=	-g -Wall $(MACHDEP) $(INCLUDE)
CXXFLAGS	=	$(CFLAGS)

#LDFLAGS    =   -g $(MACHDEP) -Wl,-Map,$(notdir $@).map,--section-start,.init=0x80b00000
#LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map,--section-start,.init=0x80800100
#LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map,--section-start,.init=0x80900100
#LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map,--section-start,.init=0x80800100
#LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map,--section-start,.init=0x80900100
#LDFLAGS	=	-g $(MACHDEP) -mrvl -Wl,-Map,$(notdir $@).map -T../rvl.ld
LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map -T../rvl.ld
#LDFLAGS	=	$(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
#LIBS	:=	-lfat -lpng -lm -lz -lwiiuse -lbte -lasnd -logc -lfreetype -ltremor -lmxml -lwiikeyboard
LIBS	:=	-ltremor -lfreetype -lwiiuse -lbte -lasnd -lpng -lz -logc -lm -lfat -lmxml -lwiikeyboard
#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS)
#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
PNGFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.png)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(TTFFILES:.ttf=.ttf.o) $(PNGFILES:.png=.png.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) \
					-I$(LIBOGC_INC) \
					-I$(PORTLIBS)/include \
					-I$(PORTLIBS)/include/freetype2

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					-L$(LIBOGC_LIB)

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD): version
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

version:
	@echo Generating Version.h file...
	@echo "#pragma once" > include/version.h
	@echo -n "#define SVN_VERSION " >> include/version.h
	@echo `cat .svn/entries | head -4 | tail -1` >> include/version.h


#-------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).dol
#---------------------------------------------------------------------------------
run:
	wiiload $(OUTPUT).dol

run-win:
#	C:\\wiidev\\emu\\dolphin\\Dolphin.exe --elf=$(OUTPUT).elf
	D:\\Dolphin_3089\\Dolphin.exe --elf=$(OUTPUT).elf
	
#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)

%.png.o : %.png
	@echo $(notdir $<)
	$(bin2o)
	
%.dol.o : %.dol
	@echo $(notdir $<)
	$(bin2o)
	
%.raw.o : %.raw
	@echo $(notdir $<)
	$(bin2o)
	
%.ogg.o : %.ogg
	@echo $(notdir $<)
	$(bin2o)
	
%.ttf.o : %.ttf
	@echo $(notdir $<)
	$(bin2o)
	
%.lang.o : %.lang
	@echo $(notdir $<)
	$(bin2o)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
