# $Id$
# Environment settings for the extra propagator plugin
GMAT_CODE_LOCATION = /home/djc/TS_Code/GmatDevelopment/src
GMAT_BIN_LOCATION  = /home/djc/TS_Code/GmatDevelopment/application/bin

# Set to 0 for Windows, 1 for Linux or Mac
LINUX_MAC = 1

# Set to 0 for Linux, 1 for Mac 
MAC_SPECIFIC = 0

DEBUG_BUILD = 0

# Select the base library
BASE_LIBRARY = GmatBase
# BASE_LIB_LOCATION is only used on Mac
BASE_LIB_LOCATION = GMAT.app/Contents/Frameworks/

EXTRA_PROPAGATOR_CPP_FLAGS = -fPIC
EXTRA_PROPAGATOR_LINK_FLAGS = -fPIC

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
                    $(MAC_CPP_FLAGS) -arch i386
                   

else
SHARED_EXTENSION = .so
SHARED_LIB_FLAGS = $(EXTRA_PROPAGATOR_LINK_FLAGS) -shared -Wl
endif

else

SHARED_EXTENSION = .dll
SHARED_LIB_FLAGS = $(EXTRA_PROPAGATOR_LINK_FLAGS) -shared -Wl --out-implib \
                   -L$(GMAT_CODE_LOCATION)/base/lib -lGmatBase
endif

DESIRED_OPTIMIZATIONS =  -DSTRICT -Wall -fno-pcc-struct-return -O3 \
                 -finline-functions -funroll-loops -fno-rtti -DNO_GCC_PRAGMA \
                 -march=pentium -malign-double -fexceptions \
                 -fexpensive-optimizations

ifeq ($(DEBUG_BUILD), 1)
OPTIMIZATIONS = -fno-strict-aliasing -ffriend-injection $(WX_28_DEFINES)
else
OPTIMIZATIONS = -O3 -fno-strict-aliasing -ffriend-injection $(WX_28_DEFINES)
endif

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) $(EXTRA_PROPAGATOR_CPP_FLAGS) -Wall \
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE)
