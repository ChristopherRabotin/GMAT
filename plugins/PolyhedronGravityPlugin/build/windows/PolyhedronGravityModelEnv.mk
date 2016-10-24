# $Id: PolyheronGravityModelEnv.mk 9321 2012-08-17 16:58:54Z tuannguyen $
# Environment settings for the PolyheronGravityModel plugin
# location of GMAT base headers and libraries
GMAT_CODE_LOCATION = ../../../../src
GMAT_BIN_LOCATION = ../../../../application/bin

# Set to 0 for Windows, 1 for Linux or Mac
LINUX_MAC = 0   

# Set to 0 for Linux, 1 for Mac 
MAC_SPECIFIC = 0

DEBUG_BUILD = 0

# Select the base library
BASE_LIBRARY = GmatBase

# BASE_LIB_LOCATION is only used on Mac
BASE_LIB_LOCATION = GMAT.app/Contents/Frameworks/

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
                    $(MAC_CPP_FLAGS)
                   

else
SHARED_EXTENSION = .so
SHARED_LIB_FLAGS = -shared -Wl --out-implib
endif

else

SHARED_EXTENSION = .dll
SHARED_LIB_FLAGS = -shared -Wl --out-implib \
                   -L$(GMAT_CODE_LOCATION)/base/lib -l$(BASE_LIBRARY)

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

# wxWidgets flags
WXCPPFLAGS = `/usr/local/bin/wx-config --cppflags`

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) -Wall \
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)
#           $(MATLAB_CPP_FLAGS) $(WXCPPFLAGS) $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE)
