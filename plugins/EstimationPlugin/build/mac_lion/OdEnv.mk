# $Id: OdEnv.mk,v 1.1.1.1 2010/01/14 23:37:24 djc Exp $
# Environment settings for the estimation plugin
GMAT_Code_Location = ../../../../src
GMAT_Bin_Location = ../../../../application/bin
GMAT_Plugin_Location = ../../../../application/plugins
GMAT_DataFile_Location = ./../../DataFilePlugin/src  # TBD #

# Toggle for using the older datafile code or the plugin code
USE_DATAFILE_PLUGIN = 0

# Toggle for including the ionosphere code

INCLUDE_IONOSPHERE = 0

# Set to 0 for Windows, 1 for Linux or Mac
LINUX_MAC = 1

# Set to 0 for Linux, 1 for Mac 
MAC_SPECIFIC = 1

# Set to 1 if building on Snow Leopard with Carbon based wxWidgetsS
SNOWLEOPARD_WX2_8_10 = 1

DEBUG_BUILD = 0

# Ionosphere code needs f2c.h and libf2c
ifeq ($(INCLUDE_IONOSPHERE), 1)
IONOSHPERE_FLAGS = -DIONOSPHERE
F2C_INCLUDE = -IC:/f2c   # TBD for Mac
F2C_LIB = C:/MinGW/lib/libf2c.a     # TBD for Mac
else
IONOSHPERE_FLAGS =
F2C_INCLUDE = 
F2C_LIB =
endif


# Select the base library
BASE_LIBRARY = GmatBase
# BASE_LIB_LOCATION is only used on Mac
BASE_LIB_LOCATION = ../bin/GMAT.app/Contents/Frameworks/

# Compiler options
CPP = g++
C = gcc

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

DESIRED_OPTIMIZATIONS =  -DSTRICT -Wall -fno-pcc-struct-return -O3 \
                 -finline-functions -funroll-loops -DNO_GCC_PRAGMA \
                 -march=pentium -malign-double -fexceptions \
                 -fexpensive-optimizations

ifeq ($(DEBUG_BUILD), 1)
OPTIMIZATIONS = -fno-strict-aliasing $(WX_28_DEFINES) -fno-rtti
else
OPTIMIZATIONS = -O3 -fno-strict-aliasing $(WX_28_DEFINES) -fno-rtti
endif

ifeq ($(SNOWLEOPARD_WX2_8_10), 1)
ARCHITECTURE_FLAGS=-arch i386
PLATFORM_FLAGS=-fPIC
else
ARCHITECTURE_FLAGS=
PLATFORM_FLAGS=
endif


ifeq ($(MAC_CPP_FLAGS), 1)
MAC_CPP_FLAGS=-current_version 0.1 -compatibility_version 0.1 -fvisibility=default
endif

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) $(IONOSPHERE_FLAGS) -Wall \
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(F2C_INCLUDE) $(CPP_BASE) $(MAC_CPP_FLAGS) $(PLATFORM_FLAGS) $(ARCHITECTURE_FLAGS)
CFLAGS = $(F2C_INCLUDE) -Wall 
LINK_FLAGS = $(F2C_LIB) $(DEBUG_FLAGS)

