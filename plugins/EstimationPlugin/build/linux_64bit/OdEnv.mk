# $Id: OdEnv.mk 14 2010-01-25 23:59:47Z djconway@GSFC $
# Environment settings for the estimation plugin
GMAT_Code_Location = ../../../../src
GMAT_Bin_Location = ../../../../application/bin
GMAT_DataFile_Location = ../../../GMAT_DataFilePlugin/src

# Toggle for using the older datafile code or the plugin code
USE_DATAFILE_PLUGIN = 0

# Toggle for including the ionosphere code
INCLUDE_IONOSPHERE = 0

# Set to 0 for Windows, 1 for Linux or Mac
LINUX_MAC = 1

# Set to 0 for Linux, 1 for Mac 
MAC_SPECIFIC = 0

# Set to 1 to build against the MATLAB version, 0 for no MATLAB
USE_MATLAB = 1

DEBUG_BUILD = 0

# Ionosphere code needs f2c.h and libf2c
ifeq ($(INCLUDE_IONOSPHERE), 1)
IONOSHPERE_FLAGS = -DIONOSPHERE
F2C_LIB = -lf2c
else
IONOSHPERE_FLAGS =
F2C_LIB =
endif

# Select the base library
ifeq ($(USE_MATLAB), 0)
BASE_LIBRARY = GmatBaseNoMatlab
# BASE_LIB_LOCATION is only used on Mac
BASE_LIB_LOCATION = GMATNoMatlab.app/Contents/Frameworks/
else
BASE_LIBRARY = GmatBase
# BASE_LIB_LOCATION is only used on Mac
BASE_LIB_LOCATION = GMAT.app/Contents/Frameworks/
endif

# Compiler options
CPP = g++
C = gcc

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

DESIRED_OPTIMIZATIONS =  -DSTRICT -Wall -fno-pcc-struct-return -O3 \
                 -finline-functions -funroll-loops -fno-rtti -DNO_GCC_PRAGMA \
                 -march=pentium -malign-double -fexceptions \
                 -fexpensive-optimizations

ifeq ($(DEBUG_BUILD), 1)
OPTIMIZATIONS = -fno-strict-aliasing $(WX_28_DEFINES)
else
OPTIMIZATIONS = -O3 -fno-strict-aliasing $(WX_28_DEFINES)
endif

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) $(IONOSPHERE_FLAGS) -Wall \
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE) -fPIC
CFLAGS = $(F2C_INCLUDE) -fPIC -Wall 
LINK_FLAGS = $(F2C_LIB) $(DEBUG_FLAGS)
