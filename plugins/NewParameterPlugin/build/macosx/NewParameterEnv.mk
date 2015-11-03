# $Id: Vf13OptimizerEnv.mk 532 2010-08-26 16:58:53Z  $
# Architecture flags

GMAT_BIN_DIR = bin
GMAT_LIB_DIR = lib
GMAT_PLUGIN_DIR = plugins

ifeq ($(BUILD_64BIT), 1)
MAC_ARCHITECTURE_FLAGS = -D__MAC__ -DUSE_64_BIT_LONGS
F2C_DIR = f2c64
else
MAC_ARCHITECTURE_FLAGS = -arch i386 -D__MAC__
F2C_DIR = f2c32
endif

# Environment settings for the NewParameters plugin; Mac version

GMAT_CODE_LOCATION = ../../../../src
GMAT_BIN_LOCATION = ../../../../application/$(GMAT_BIN_DIR)

# Select the base library
BASE_LIBRARY = GmatBase

# BASE_LIB_LOCATION is only used on Mac - must point to libGmatBase outside of bundle for Console GMAT
BASE_LIB_LOCATION = ../$(GMAT_BIN_DIR)/GMAT.app/Contents/Frameworks/

# Compiler options
CPP = g++
C = gcc

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

SHARED_EXTENSION = .dylib
SHARED_LIB_FLAGS = -dylib -dynamiclib -undefined dynamic_lookup \
                   -L$(GMAT_CODE_LOCATION)/base/$(GMAT_LIB_DIR) -l$(BASE_LIBRARY) \
                    $(MAC_CPP_FLAGS) $(MAC_ARCHITECTURE_FLAGS)
                   
ifeq ($(DEBUG_BUILD), 1)
OPTIMIZATIONS = -fno-strict-aliasing -fno-rtti
else
OPTIMIZATIONS = -O3 -fno-strict-aliasing -fno-rtti
endif

DESIRED_OPTIMIZATIONS =  -DSTRICT -Wall -fno-pcc-struct-return -O3 \
                 -finline-functions -funroll-loops -fno-rtti -DNO_GCC_PRAGMA \
                 -march=pentium -malign-double -fexceptions \
                 -fexpensive-optimizations

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) -Wall -fPIC  $(MAC_ARCHITECTURE_FLAGS)\
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE) $(F2C_INCLUDE)
CFLAGS = -Wall -fPIC $(F2C_INCLUDE) $(MAC_ARCHITECTURE_FLAGS)
