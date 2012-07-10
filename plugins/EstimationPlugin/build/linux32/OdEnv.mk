# $Id: OdEnv.mk 1337 2011-03-30 17:02:16Z djconway@NDC $
# Environment settings for the estimation plugin
GMAT_Code_Location = ../../../GMAT_Development/src
GMAT_Bin_Location = ../../../GMAT_Development/application/bin
GMAT_DataFile_Location = ../../../GMAT_DataFilePlugin/src

# Toggle for using the older datafile code or the plugin code
USE_DATAFILE_PLUGIN = 0

# Set to 0 for Windows, 1 for Linux or Mac
LINUX_MAC = 1

# Set to 0 for Linux, 1 for Mac 
MAC_SPECIFIC = 0

# Set to 1 to build against the MATLAB version, 0 for no MATLAB
USE_MATLAB = 1

DEBUG_BUILD = 0

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
CPP_BASE = $(OPTIMIZATIONS) -Wall \
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE)
