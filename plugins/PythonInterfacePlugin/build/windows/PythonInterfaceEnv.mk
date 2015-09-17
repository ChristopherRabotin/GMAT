# $Id$
# Environment settings for the MatlabInterface plugin
# location of GMAT base headers and libraries
GMAT_CODE_LOCATION = ../../../../src
GMAT_BIN_LOCATION = ../../../../application/bin

# location of MATLAB headers and libraries
MATLAB_DIR = C:/Program\ Files/MATLAB/R2009B
MATLAB_CPP_FLAGS = -D__USE_MATLAB__=1 -I$(MATLAB_DIR)/extern/include
MATLAB_LIB_DIR = -L$(MATLAB_DIR)/bin/win32
MATLAB_LIBRARIES = $(MATLAB_LIB_DIR) -leng -lmx -lmat

# Set to 0 for Windows, 1 for Linux or Mac
LINUX_MAC = 0   

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

ifeq ($(LINUX_MAC), 1)

ifeq ($(MAC_SPECIFIC), 1)
SHARED_EXTENSION = .dylib
SHARED_LIB_FLAGS = -bundle -two_levelnamespace -undefined dynamic_lookup \
                   -L$(GMAT_CODE_LOCATION)/base/lib -l$(BASE_LIBRARY) \
                    $(MATLAB_IF_LIBRARY) \
                    $(MAC_CPP_FLAGS)
                   

else
SHARED_EXTENSION = .so
SHARED_LIB_FLAGS = -shared -Wl --out-implib
endif

else

SHARED_EXTENSION = .dll
SHARED_LIB_FLAGS = $(MATLAB_LINK_FLAGS) -shared -Wl --out-implib \
                   -L$(GMAT_CODE_LOCATION)/base/lib -l$(BASE_LIBRARY) \
                   $(MATLAB_LIBRARIES) \

endif

OPTIMIZATIONS =  -DSTRICT -Wall -fno-pcc-struct-return -O3 \
                 -finline-functions -funroll-loops -fno-rtti -DNO_GCC_PRAGMA \
                 -march=pentium -malign-double -fexceptions \
                 -fexpensive-optimizations

# Set options for debugging and profiling
ifeq ($(DEBUG_BUILD), 1)
DEBUG_FLAGS = -g
else
DEBUG_FLAGS = 
endif


# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) \
           $(MATLAB_CPP_FLAGS) $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE)
