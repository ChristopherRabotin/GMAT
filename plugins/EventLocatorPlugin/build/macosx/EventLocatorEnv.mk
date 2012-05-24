# $Id$
# Architecture flags
ifeq ($(BUILD_64BIT), 1)
GMAT_BIN_DIR = bin64
GMAT_LIB_DIR = lib64
GMAT_PLUGIN_DIR = plugins64
MAC_ARCHITECTURE_FLAGS = 
F2C_DIR = f2c64
else
GMAT_BIN_DIR = bin
GMAT_LIB_DIR = lib
GMAT_PLUGIN_DIR = plugins
MAC_ARCHITECTURE_FLAGS = -arch i386
F2C_DIR = f2c32
endif

# Environment settings for the event locator plugin; Linux 64-bit version

GMAT_CODE_LOCATION = ../../../../src
GMAT_BIN_LOCATION = ../../../../application/$(GMAT_BIN_DIR)

# Set to 1 for debugging and profiling
DEBUG_BUILD = 0

# Select the base library
BASE_LIBRARY = GmatBase

# BASE_LIB_LOCATION is only used on Mac - must point to libGmatBase outside of bundle for Console GMAT
BASE_LIB_LOCATION = ../$(GMAT_BIN_DIR)/GMAT.app/Contents/Frameworks/

# Uncomment if we ever require f2c
##F2C_DIR = ../../../../GMAT_3rdParty/f2c
#F2C_DIR = ../../../../GMAT_3rdParty/cspice
#F2C_LIBDIR = $(F2C_DIR)/lib
#F2C_HEADERDIR = $(F2C_DIR)/include
#F2C_LIB = -L$(F2C_LIBDIR) -lf2c -lm

# Compiler options
CPP = g++
C = gcc

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

ifeq ($(DEBUG_BUILD), 1)
OPTIMIZATIONS = -fno-strict-aliasing -funroll-loops -fno-rtti  -ffriend-injection
else
OPTIMIZATIONS = -O3 -fno-strict-aliasing -funroll-loops -fno-rtti  -ffriend-injection
endif

PLATFORM_FLAGS=-fPIC

SHARED_EXTENSION = .dylib
SHARED_LIB_FLAGS = -dylib -dynamiclib -two_levelnamespace -undefined dynamic_lookup -headerpad_max_install_names\
                   -L$(GMAT_CODE_LOCATION)/base/$(GMAT_LIB_DIR) -l$(BASE_LIBRARY) $(PLATFORM_FLAGS)\
                    $(MAC_CPP_FLAGS) -lgcc_s.1 $(MAC_ARCHITECTURE_FLAGS) -bind_at_load
               
# $(WXLINKFLAGS) $(MATLAB_LIBRARIES    
# Set options for debugging and profiling
ifeq ($(DEBUG_BUILD), 1)
DEBUG_FLAGS = -g
else
DEBUG_FLAGS = 
endif

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) -Wall $(PLATFORM_FLAGS)\
           $(PROFILE_FLAGS) $(DEBUG_FLAGS) $(MAC_ARCHITECTURE_FLAGS)

CPPFLAGS = $(CPP_BASE)
CFLAGS = -Wall $(PLATFORM_FLAGS) $(MAC_ARCHITECTURE_FLAGS)
