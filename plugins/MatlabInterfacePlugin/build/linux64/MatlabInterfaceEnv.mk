# $Id: MatlabInterfaceEnv.mk 9321 2011-03-30 00:20:54Z djcinsb $
# Environment settings for the MatlabInterface plugin
# location of GMAT base headers and libraries
GMAT_CODE_LOCATION = ../../../../src
GMAT_BIN_LOCATION = ../../../../application/bin

# location of MATLAB headers and libraries
MATLAB_DIR = /opt/matlab/R2009b
MATLAB_CPP_FLAGS = -D__USE_MATLAB__=1 -I$(MATLAB_DIR)/extern/include
MATLAB_LIB_DIR = -L$(MATLAB_DIR)/bin/glnxa64
MATLAB_LIBRARIES = $(MATLAB_LIB_DIR) -leng -lmx -lmat

# Set to 0 for Windows, 1 for Linux or Mac
LINUX_MAC = 1

# Set to 0 for Linux, 1 for Mac 
MAC_SPECIFIC = 0

DEBUG_BUILD = 0

# Select the base library
BASE_LIBRARY = GmatBase

# DataFile library
#MATLAB_LIBRARY = DataFile

# Compiler options
CPP = g++
C = gcc

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

SHARED_EXTENSION = .so
SHARED_LIB_FLAGS = -shared -Wl --out-implib

DESIRED_OPTIMIZATIONS =  -DSTRICT -Wall -fno-pcc-struct-return -O3 \
                 -finline-functions -funroll-loops -fno-rtti -DNO_GCC_PRAGMA \
                 -march=pentium -malign-double -fexceptions \
                 -fexpensive-optimizations

ifeq ($(DEBUG_BUILD), 1)
OPTIMIZATIONS = -fno-strict-aliasing $(WX_28_DEFINES) -fPIC
else
OPTIMIZATIONS = -O3 -fno-strict-aliasing $(WX_28_DEFINES) -fPIC
endif

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) -Wall \
           $(MATLAB_CPP_FLAGS) $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE)
